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

#include "aws-lambda/http/response.h"
#include "aws-lambda/ext/string-builder.h"
#include "aws-lambda/ext/hashmap.h"
#include "aws-lambda/c-runtime/utils.h"
#include <string.h>
#include <ctype.h>
#include <assert.h>

static str_builder_t *body_sb;
static map_t *header_map;

/* Prototypes */
static inline bool is_char_trimmable(int ch);

static inline char *trim(char *src);

static inline char *strlwr(char *str);

void http_response_init(void) {
    body_sb = str_builder_create();
    header_map = hashmap_new();
}

void http_response_cleanup(void) {
    str_builder_destroy(body_sb);
    hashmap_free(header_map);
}

void http_response_clear(void) {
    str_builder_clear(body_sb);
    hashmap_free(header_map);
    header_map = hashmap_new();
}

const char *http_response_get_content(void) {
    return str_builder_peek(body_sb);
}

char *get_header(const char *header) {
    char *h_tmp = malloc(strlen(header) + 1);
    FAIL_IF(!h_tmp)
    strcpy(h_tmp, header);

    char *value = NULL;
    assert(hashmap_get(header_map, h_tmp, (void **) &value) == MAP_OK);

    SAFE_FREE(h_tmp);
    return value;
}

bool has_header(const char *header) {
    char *h_tmp = malloc(strlen(header) + 1);
    FAIL_IF(!h_tmp)
    strcpy(h_tmp, header);

    int ret = hashmap_has_key(header_map, h_tmp);

    SAFE_FREE(h_tmp);
    return ret == MAP_OK;
}

/* Curl callback functions */
size_t write_data_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
    str_builder_add_str(body_sb, ptr, nmemb);
    return nmemb;
}

size_t write_header_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
    char *const delimiter = ":";
    char *raw_header = strdup(ptr);
    char *header_name = NULL;
    char *header_value = NULL;

    // split
    char *split_ptr = strtok(raw_header, delimiter);
    if (split_ptr) {
        header_name = strdup(split_ptr);
        split_ptr = strtok(NULL, delimiter);
        if (split_ptr)
            header_value = strdup(split_ptr);
    }

    // trim
    if (header_name && header_value) {
        printf("Header name: <%s>\n", header_name);
        header_name = strlwr(trim(header_name));
        header_value = trim(header_value);
        printf("Header value: <%s>\n", header_value);
        int error = hashmap_put(header_map, header_name, header_value);
        assert(error == MAP_OK);
    } else
        SAFE_FREE(header_name); // free dynamic allocation that didn't match the previous if stmt.

    SAFE_FREE(raw_header);
    return nmemb;
}

size_t read_data_callback(char *buffer, size_t size, size_t nmemb, void *userdata) {
    curl_request_write_t *wt = (curl_request_write_t *) userdata;
    size_t buffer_size = size * nmemb;

    if (wt->sizeleft) {
        /* copy as much as possible from the source to the destination */
        size_t copy_this_much = wt->sizeleft;
        if (copy_this_much > buffer_size)
            copy_this_much = buffer_size;
        memcpy(buffer, wt->readptr, copy_this_much);

        wt->readptr += copy_this_much;
        wt->sizeleft -= copy_this_much;
        return copy_this_much; /* we copied this many bytes */
    }

    return 0; /* no more data left to deliver */
}

static inline char *trim(char *src) {
    char *ret = NULL;
    size_t src_len = strlen(src);
    char *p;

    // trailing
    for (size_t i = src_len - 1; i > 0; i--) {
        p = src + i;
        if (is_char_trimmable(*p))
            *p = '\0';
        else
            break; // no need to go all the way down to the first char
    }

    // leading
    for (size_t i = 0; i < src_len - 1; i++) {
        p = src + i;
        if (!is_char_trimmable(*p)) {
            ret = malloc(strlen(p) + 1);
            FAIL_IF(!ret)
            strcpy(ret, p);
            SAFE_FREE(src);
            break;
        }
    }
    return ret;
}

static inline bool is_char_trimmable(int ch) {
    return (ch < -1 || ch > 255) || (ch == 8 || ch == 9 || ch == 10 || ch == 13 || ch == 32);
}

static inline char *strlwr(char *str) {
    unsigned char *p = (unsigned char *) str;
    while (*p) {
        *p = tolower((unsigned char) *p);
        p++;
    }
    return str;
}