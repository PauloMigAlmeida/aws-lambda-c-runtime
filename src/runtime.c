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

#include "aws-lambda/c-runtime/runtime.h"
#include "aws-lambda/c-runtime/version.h"
#include "aws-lambda/c-runtime/service-logic.h"
#include "aws-lambda/c-runtime/utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** Prototypes */
void cleanup(invocation_request **request, invocation_response **response);

/**
 * Create a successful invocation response with the given payload and content-type.
 */
invocation_response* success(char* payload, char* content_type){
    invocation_response* r = malloc(sizeof(invocation_response));
    FAIL_IF(!r)
    r->success = true;
    SAFE_STRDUP(r->content_type, content_type)
    SAFE_STRDUP(r->payload, payload)
    return r;
}

/**
 * Create a failure response with the given error message and error type.
 * The content-type is always set to application/json in this case.
 */
invocation_response* failure(char* error_message, char* error_type){
    char* template = "{\"errorMessage\":\"%s\",\"errorType\":\"%s\", \"stackTrace\":[]}";

    invocation_response* r = malloc(sizeof(invocation_response));
    FAIL_IF(!r)
    r->success = false;
    SAFE_STRDUP(r->content_type,"application/vnd.aws.lambda.error+json")

    char *tmp_str = malloc(strlen(template) + strlen(error_message) + strlen(error_type) + 1);
    FAIL_IF(!tmp_str)
    sprintf(tmp_str, template, error_message, error_type);
    r->payload = tmp_str;

    return r;
}

// Entry method
void run_handler(handler_ptr handler){
    printf("Initializing the C Lambda Runtime version %s\n", get_version());

    service_logic_setup();

    invocation_request *req = NULL;
    invocation_response *res =  NULL;
    while(service_logic_get_next(&req)){
        printf("Invoking user handler\n");
        handler(req, &res);
        printf("Invoking user handler completed.\n");
        printf("\tresponse payload: %s\n", res->payload);

        service_logic_post_result(req, res);
        cleanup(&req, &res);
    }

    service_logic_cleanup();

}

void cleanup(invocation_request **request, invocation_response **response){
    // freeing up dynamic allocated resources
    SAFE_FREE((*request)->payload);
    SAFE_FREE((*request)->request_id);
    SAFE_FREE((*request)->xray_trace_id);
    SAFE_FREE((*request)->client_context);
    SAFE_FREE((*request)->cognito_identity);
    SAFE_FREE((*request)->function_arn);
    SAFE_FREE(*request);

    SAFE_FREE((*response)->payload);
    SAFE_FREE((*response)->content_type);
    SAFE_FREE((*response));
}