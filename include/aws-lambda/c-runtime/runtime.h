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

#ifndef AWS_LAMBDA_C_RUNTIME_RUNTIME_H
#define AWS_LAMBDA_C_RUNTIME_RUNTIME_H

#include <stdbool.h>

typedef struct {
    /**
     * The user's payload represented as a UTF-8 string.
     */
    char *payload;

    /**
     * An identifier unique to the current invocation.
     */
    char *request_id;

    /**
     * X-Ray tracing ID of the current invocation.
     */
    char *xray_trace_id;

    /**
     * Information about the client application and device when invoked through the AWS Mobile SDK.
     */
    char *client_context;

    /**
     * Information about the Amazon Cognito identity provider when invoked through the AWS Mobile SDK.
     */
    char *cognito_identity;

    /**
     * The ARN requested. This can be different in each invoke that executes the same version.
     */
    char *function_arn;

} invocation_request;

typedef struct {
    /**
     * The output of the function which is sent to the lambda caller.
     */
    char *payload;

    /**
     * The MIME type of the payload.
     * This is always set to 'application/json' in unsuccessful invocations.
     */
    char *content_type;

    /**
     * Flag to distinguish if the contents are for successful or unsuccessful invocations.
     */
    bool success;
} invocation_response;

/**
 * Define a lesser crypt way of defining the pointer of the handler function
 */
typedef void (*handler_ptr)(invocation_request*, invocation_response**);

/**
 * Create a successful invocation response with the given payload and content-type.
 */
invocation_response* success(char *payload, char *content_type);

/**
 * Create a failure response with the given error message and error type.
 * The content-type is always set to application/json in this case.
 */
invocation_response* failure(char *error_message, char *error_type);

// Entry method
void run_handler(handler_ptr handler);

#endif //AWS_LAMBDA_C_RUNTIME_RUNTIME_H