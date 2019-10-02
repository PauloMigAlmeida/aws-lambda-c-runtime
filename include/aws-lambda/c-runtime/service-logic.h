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

#ifndef AWS_LAMBDA_C_RUNTIME_SERVICE_LOGIC_H
#define AWS_LAMBDA_C_RUNTIME_SERVICE_LOGIC_H

#include "aws-lambda/c-runtime/runtime.h"
#include "aws-lambda/http/service-integration.h"

void service_logic_setup(void);

void service_logic_cleanup(void);

bool service_logic_get_next(invocation_request** req);

void service_logic_post_result(invocation_request *request, invocation_response *response);

#endif //AWS_LAMBDA_C_RUNTIME_SERVICE_LOGIC_H