/*
 * Copyright (2019) - Paulo Miguel Almeida
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "aws-lambda/http/service-integration.h"
#include "aws-lambda/http/response.h"
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#define HTTP_HEADER_CONTENT_TYPE "Content-Type: "
#define HTTP_HEADER_CONTENT_LENGTH "Content-Length: "

static CURL *curl;
static char base_url[256];
static char *next_endpoint, *result_endpoint;

/* Prototypes */
static inline char *build_url(char *b_url, char *path);

void service_integration_init(void) {
    strcpy(base_url, "http://");
    char *env;
    if ((env = getenv("AWS_LAMBDA_RUNTIME_API"))) {
        printf("LAMBDA_SERVER_ADDRESS defined in environment as: %s\n", env);
        strcat(base_url, env);
    }
    next_endpoint = build_url(base_url, "/2018-06-01/runtime/invocation/next");
    result_endpoint = build_url(base_url, "/2018-06-01/runtime/invocation/");

    curl_global_init(CURL_GLOBAL_ALL);
    http_response_init();
}

void service_integration_cleanup(void) {
    curl_global_cleanup();
    free(next_endpoint);
    free(result_endpoint);
    http_response_cleanup();
}

void set_default_curl_options(void) {
    // lambda freezes the container when no further tasks are available. The freezing period could be longer than the
    // request timeout, which causes the following get_next request to fail with a timeout error.
    curl_easy_reset(curl);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 0L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 1L);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(curl, CURLOPT_TCP_NODELAY, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);
}

void set_curl_next_options(void) {
    set_default_curl_options();
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(curl, CURLOPT_URL, next_endpoint);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data_callback);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, write_header_callback);
}

void set_curl_post_result_options(void) {
    set_default_curl_options();
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_data_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data_callback);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, write_header_callback);
}

next_outcome request_get_next(void) {
    // initialise struct with sentinel values
    next_outcome ret;
    ret.success = false;
    ret.res_code = -1; // REQUEST_NOT_MADE
    ret.request.payload = NULL;
    ret.request.request_id = NULL;
    ret.request.xray_trace_id = NULL;
    ret.request.client_context = NULL;
    ret.request.cognito_identity = NULL;
    ret.request.function_arn = NULL;

    http_response_clear();
    curl = curl_easy_init();
    if (curl) {
        set_curl_next_options();
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, get_user_agent_header());
        printf("Making request to %s\n", next_endpoint);
        CURLcode curl_code = curl_easy_perform(curl);
        curl_slist_free_all(headers);

        if (curl_code != CURLE_OK) {
            printf("CURL returned error code %d - %s\n", curl_code, curl_easy_strerror(curl_code));
            printf("Failed to get next invocation. No Response from endpoint\n");
        } else {
            printf("CURL response body: %s\n", http_response_get_content());
            ret.success = true;

            long resp_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &resp_code);
            ret.res_code = (int) resp_code;

            ret.request.payload = strdup(http_response_get_content());
            if (has_header(REQUEST_ID_HEADER)) {
                ret.request.request_id = strdup(get_header(REQUEST_ID_HEADER));
                printf("ret.request.request_id: %s\n", ret.request.request_id);
            }

            if (has_header(TRACE_ID_HEADER)) {
                ret.request.xray_trace_id = strdup(get_header(TRACE_ID_HEADER));
                printf("ret.request.xray_trace_id: %s\n", ret.request.xray_trace_id);
            }

            if (has_header(CLIENT_CONTEXT_HEADER)) {
                ret.request.client_context = strdup(get_header(CLIENT_CONTEXT_HEADER));
                printf("ret.request.client_context: %s\n", ret.request.client_context);
            }

            if (has_header(COGNITO_IDENTITY_HEADER)) {
                ret.request.cognito_identity = strdup(get_header(COGNITO_IDENTITY_HEADER));
                printf("ret.request.cognito_identity: %s\n", ret.request.cognito_identity);
            }

            if (has_header(FUNCTION_ARN_HEADER)) {
                ret.request.function_arn = strdup(get_header(FUNCTION_ARN_HEADER));
                printf("ret.request.function_arn: %s\n", ret.request.function_arn);
            }

            //TODO add/create handler for deadline-ms-handler
//            if (resp.has_header(DEADLINE_MS_HEADER)) {
//                auto const& deadline_string = resp.get_header(DEADLINE_MS_HEADER);
//                unsigned long ms = strtoul(deadline_string.c_str(), nullptr, 10);
//                assert(ms > 0);
//                assert(ms < ULONG_MAX);
//                req.deadline += std::chrono::milliseconds(ms);
//                logging::log_info(
//                        LOG_TAG,
//                        "Received payload: %s\nTime remaining: %" PRId64,
//                        req.payload.c_str(),
//                        static_cast<int64_t>(req.get_time_remaining().count()));
//            }
        }
        curl_easy_cleanup(curl);
    }
    return ret;
}

post_result_outcome request_post_result(char *request_id, invocation_response *response) {
    post_result_outcome ret;
    ret.success = false;
    ret.res_code = -1; // REQUEST_NOT_MADE

    http_response_clear();
    curl = curl_easy_init();
    if (curl) {
        // this ought to be enough space to accommodate request_id and the last url segment (including \0).
        char *request_url = malloc(strlen(result_endpoint) + 128);
        strcpy(request_url, result_endpoint);
        strcat(request_url, request_id);
        strcat(request_url, (response->success) ? "/response" : "/error");
        printf("Making request to %s\n", request_url);

        set_curl_post_result_options();
        curl_easy_setopt(curl, CURLOPT_URL, request_url);
        struct curl_slist *headers = NULL;

        char *content_type_h = malloc(strlen(HTTP_HEADER_CONTENT_TYPE) + strlen(response->content_type) + 1);
        strcpy(content_type_h, HTTP_HEADER_CONTENT_TYPE);
        strcat(content_type_h, response->content_type);
        headers = curl_slist_append(headers, content_type_h);
        headers = curl_slist_append(headers, get_user_agent_header());

        printf("calculating content length... %lu bytes\n", strlen(response->payload));
        char *content_length_v = malloc(16); //Lambda payload in bytes -> 7 digits + 1 null char + future proof fat
        sprintf(content_length_v, "%lu", strlen(response->payload));

        char *content_length_h = malloc(strlen(HTTP_HEADER_CONTENT_LENGTH) + strlen(content_length_v) + 1);
        sprintf(content_length_h, "%s%s", HTTP_HEADER_CONTENT_LENGTH, content_length_v);

        headers = curl_slist_append(headers, content_length_h);

        curl_request_write_t req_write;
        req_write.readptr = response->payload;
        req_write.sizeleft = strlen(response->payload);

        curl_easy_setopt(curl, CURLOPT_READDATA, &req_write);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        CURLcode curl_code = curl_easy_perform(curl);
        curl_slist_free_all(headers);

        if (curl_code != CURLE_OK) {
            printf("CURL returned error code %d - %s\n", curl_code, curl_easy_strerror(curl_code));
        }else{
            printf("CURL response body: %s\n", http_response_get_content());

            long http_response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_response_code);
            ret.res_code = (int)http_response_code;
            ret.success = (http_response_code >= 200 && http_response_code <= 299);
        }

        curl_easy_cleanup(curl);
        free(request_url);
        free(content_type_h);
        free(content_length_h);
        free(content_length_v);
    }
    return ret;
}

/* Utility functions */
static inline char *build_url(char *b_url, char *path) {
    char *dest = malloc(strlen(b_url) + strlen(path) + 1);
    strcpy(dest, b_url);
    strcat(dest, path);
    return dest;
}

