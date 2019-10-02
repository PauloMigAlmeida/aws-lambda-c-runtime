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

#ifndef AWS_LAMBDA_C_RUNTIME_UTILS_H
#define AWS_LAMBDA_C_RUNTIME_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FAIL_IF(x)                                  \
if ((x)) {                                          \
    fprintf(stderr,                                 \
    "[FAIL] Test FAILED on line %d\n", __LINE__);   \
    exit(1);                                        \
}                                                   \

void saferFree(void **pp);
#define SAFE_FREE(p) saferFree((void**)&(p))

#define SAFE_STRDUP(target, content)                                \
target = strdup(content);                                           \
if ((!target)) {                                                    \
    fprintf(stderr,                                                 \
    "[FAIL] Error when duplicating str on line %d\n", __LINE__);    \
    exit(1);                                                        \
}                                                                   \


#endif //AWS_LAMBDA_C_RUNTIME_UTILS_H
