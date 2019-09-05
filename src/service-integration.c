#include "aws-lambda/http/service-integration.h"
#include "aws-lambda/ext/string-builder.h"
#include "aws-lambda/ext/hashmap.h"
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <assert.h>

static CURL *curl;
static char base_url[128];
static char *next_endpoint, *init_error_endpoint, *result_endpoint;
static str_builder_t *body_sb;
static map_t *header_map;

/* Prototypes */
static inline char* build_url(char* b_url, char* path);
static inline void trim(char* src);
static inline bool is_char_trimmable(int ch);
size_t write_data_callback(char *ptr, size_t size, size_t nmemb, void *userdata);
size_t write_header_callback(char* ptr, size_t size, size_t nmemb, void* userdata);


void service_integration_init(){
    strcpy(base_url, "http://");
    char *env;
    if ((env = getenv("AWS_LAMBDA_RUNTIME_API"))) {
        printf("LAMBDA_SERVER_ADDRESS defined in environment as: %s\n", env);
        strcat(base_url, env);
    }
    init_error_endpoint = build_url(base_url, "/2018-06-01/runtime/init/error");
    next_endpoint = build_url(base_url, "/2018-06-01/runtime/invocation/next");
    result_endpoint = build_url(base_url, "/2018-06-01/runtime/invocation/");

    curl_global_init(CURL_GLOBAL_ALL);
    body_sb = str_builder_create();
    header_map = hashmap_new();
}

void service_integration_cleanup(){
    curl_global_cleanup();
    free(next_endpoint);
    free(init_error_endpoint);
    free(result_endpoint);
    str_builder_destroy(body_sb);

    hashmap_free(header_map);
}

void set_curl_next_options(){
    // lambda freezes the container when no further tasks are available. The freezing period could be longer than the
    // request timeout, which causes the following get_next request to fail with a timeout error.
    curl_easy_reset(curl);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 0L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 1L);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(curl, CURLOPT_TCP_NODELAY, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);

    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(curl, CURLOPT_URL, next_endpoint);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data_callback);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, write_header_callback);
}

next_outcome request_get_next() {
    next_outcome ret;
    ret.success = false;
    ret.res_code = -1; // REQUEST_NOT_MADE

    str_builder_clear(body_sb);
    curl = curl_easy_init();
    if(curl){
        set_curl_next_options();
        CURLcode curl_code = curl_easy_perform(curl);

        if (curl_code != CURLE_OK) {
            printf("CURL returned error code %d - %s\n", curl_code, curl_easy_strerror(curl_code));
            printf("Failed to get next invocation. No Response from endpoint\n");
        }else{
            printf("CURL response body: %s\n", str_builder_peek(body_sb));
            ret.success = true;
            long resp_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &resp_code);
            ret.res_code = (int)resp_code;
            ret.request.payload = strdup(str_builder_peek(body_sb));
        }
        curl_easy_cleanup(curl);
    }
    return ret;
}

/* Curl callback functions */
size_t write_data_callback(char *ptr, size_t size, size_t nmemb, void *userdata){
    str_builder_add_str(body_sb, ptr, nmemb);
    return nmemb;
}

size_t write_header_callback(char* ptr, size_t size, size_t nmemb, void* userdata){
    char* const delimiter = ":";
    char* raw_header = strdup(ptr);
    char* header_name = NULL;
    char* header_value = NULL;

    // split
    char* split_ptr = strtok(raw_header, delimiter);
    if(split_ptr){
        header_name = strdup(split_ptr);
        split_ptr = strtok(NULL, delimiter);
        if(split_ptr)
            header_value = strdup(split_ptr);
    }

    // trim
    if(header_name && header_value){
        trim(header_name);
        trim(header_value);
//        printf("Header name: <%s>\n", header_name);
//        printf("Header value: <%s>\n", header_value);
        int error = hashmap_put(header_map, header_name, header_value);
        assert(error==MAP_OK);
    }else
        free(header_name); // free dynamic allocation that didn't match the previous if stmt.

    free(raw_header);
    return nmemb;
}

/* Utility functions */
static inline char* build_url(char* b_url, char* path){
    char* dest = malloc(strlen(b_url) + strlen(path) + 1);
    strcpy(dest, b_url);
    strcat(dest, path);
    return dest;
}

static inline void trim(char* src){
    size_t src_len = strlen(src);
    char* p;

    // trailing
    for(size_t i = src_len - 1; i > 0; i--){
        p = src + i;
        if(is_char_trimmable(*p))
            *p = '\0';
        else
            break; // no need to go all the way down to the first char
    }

    // leading
    for(size_t i = 0; i < src_len - 1; i++){
        p = src + i;
        if(!is_char_trimmable(*p)){
            strcpy(src, p);
            break;
        }
    }
}

static inline bool is_char_trimmable(int ch){
    return (ch < -1 || ch > 255) || (ch == 8 || ch == 9 || ch == 10 || ch == 13 || ch == 32);
}