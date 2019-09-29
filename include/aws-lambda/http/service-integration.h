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
#ifndef AWS_LAMBDA_C_RUNTIME_SERVICE_INTEGRATION_H
#define AWS_LAMBDA_C_RUNTIME_SERVICE_INTEGRATION_H

#include <stdbool.h>
#include "aws-lambda/c-runtime/runtime.h"

typedef struct {
    bool success;
    invocation_request* request;
    int res_code;
} next_outcome;

typedef struct {
    bool success;
    int res_code;
} post_result_outcome;

void service_integration_init(void);

void service_integration_cleanup(void);

next_outcome request_get_next(void);

post_result_outcome request_post_result(invocation_request *request, invocation_response *response);

#endif