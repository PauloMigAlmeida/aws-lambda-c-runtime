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

void my_handler(invocation_request *request, invocation_response **response){
    *response = success("Hello World from AWS Lambda C Runtime","text/plain");
//    *response = failure("Hello World from AWS Lambda C Runtime","Application_Error");
}

int main(void){
    run_handler(&my_handler);
    return 0;
}