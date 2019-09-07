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

#include "aws-lambda/c-runtime/service-logic.h"
#include "aws-lambda/http/service-integration.h"

#include <stdlib.h>
#include <stdio.h>

#define MAX_SERVICE_LOGIC_RETRIES 3

/* Implementations */
void service_logic_setup(void) {
    service_integration_init();
}

void service_logic_cleanup(void){
    service_integration_cleanup();
}

invocation_request* service_logic_get_next() {
    size_t retries = 0;
    size_t const max_retries = MAX_SERVICE_LOGIC_RETRIES;

    while (retries < max_retries) {
        next_outcome next_outcome = request_get_next();
        if (!next_outcome.success) {
            printf("HTTP request was not successful. HTTP response code: %d. Retrying..\n", next_outcome.res_code);
            retries++;
        } else {
            invocation_request* req = malloc(sizeof(invocation_request));
            *req = next_outcome.request;
            return req;
        }
    }

    printf("Exhausted all retries... Exiting!\n");
    return NULL;
}

void service_logic_post_result(char *request_id, invocation_response *response){
    size_t retries = 0;
    size_t const max_retries = MAX_SERVICE_LOGIC_RETRIES;

    post_result_outcome result_outcome;
    result_outcome.success = false;

    while (retries < max_retries) {
        result_outcome = request_post_result(request_id, response);
        if (!result_outcome.success) {
            printf("HTTP request was not successful. HTTP response code: %d. Retrying..\n", result_outcome.res_code);
            retries++;
        } else
            break;
    }

    if(!result_outcome.success)
        printf("Exhausted all retries... Exiting!\n");
}