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
#include <stdio.h>

#define MAX_SERVICE_LOGIC_RETRIES 3

/* Implementations */
void service_logic_setup(void) {
    service_integration_init();
}

void service_logic_cleanup(void){
    service_integration_cleanup();
}

bool service_logic_get_next(invocation_request **req) {
    size_t retries = 0;

    while (retries < MAX_SERVICE_LOGIC_RETRIES) {
        next_outcome next_outcome = request_get_next();
        if (!next_outcome.success) {
            printf("HTTP request was not successful. HTTP response code: %d. Retrying..\n", next_outcome.res_code);
            retries++;
        } else {
            *req = next_outcome.request;
            return true;
        }
    }

    printf("Exhausted all retries... Exiting!\n");
    return false;
}

void service_logic_post_result(invocation_request *request, invocation_response *response){
    size_t retries = 0;

    post_result_outcome result_outcome;
    result_outcome.success = false;

    while (retries < MAX_SERVICE_LOGIC_RETRIES) {
        result_outcome = request_post_result(request, response);
        if (!result_outcome.success) {
            printf("HTTP request was not successful. HTTP response code: %d. Retrying..\n", result_outcome.res_code);
            retries++;
        } else
            break;
    }

    if(!result_outcome.success)
        printf("Exhausted all retries... Exiting!\n");
}