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

#ifndef AWS_LAMBDA_C_RUNTIME_RESPONSE_H
#define AWS_LAMBDA_C_RUNTIME_RESPONSE_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "aws-lambda/c-runtime/version.h"

// shared structs
typedef struct {
    const char *readptr;
    size_t sizeleft;
} curl_request_write_t;

// constants
static char *const LOG_TAG = "LAMBDA_RUNTIME";
static char *const REQUEST_ID_HEADER = "lambda-runtime-aws-request-id";
static char *const TRACE_ID_HEADER = "lambda-runtime-trace-id";
static char *const CLIENT_CONTEXT_HEADER = "lambda-runtime-client-context";
static char *const COGNITO_IDENTITY_HEADER = "lambda-runtime-cognito-identity";
static char *const DEADLINE_MS_HEADER = "lambda-runtime-deadline-ms";
static char *const FUNCTION_ARN_HEADER = "lambda-runtime-invoked-function-arn";

// life-cycle
void http_response_init(void);

void http_response_cleanup(void);

void http_response_clear(void);

const char *http_response_get_content(void);

char *get_header(const char *header);

bool has_header(const char *header);

// Curl-related callbacks
size_t write_data_callback(char *ptr, size_t size, size_t nmemb, void *userdata);

size_t write_header_callback(char *ptr, size_t size, size_t nmemb, void *userdata);

size_t read_data_callback(char *buffer, size_t size, size_t nmemb, void *userdata);

// Utility functions
static inline char *get_user_agent_header(void) {
    static char buf[50];
    snprintf(buf, sizeof(buf) - 1, "User-Agent: AWS_Lambda_C/%s", get_version());
    return buf;
}

#endif //AWS_LAMBDA_C_RUNTIME_RESPONSE_H
