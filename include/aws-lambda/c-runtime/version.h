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

#ifndef AWS_LAMBDA_C_RUNTIME_VERSION_H
#define AWS_LAMBDA_C_RUNTIME_VERSION_H

/**
 * Returns the major component of the library version.
 */
unsigned char get_version_major(void);

/**
 * Returns the minor component of the library version.
 */
unsigned char get_version_minor(void);

/**
 * Returns the patch component of the library version.
 */
unsigned char get_version_patch(void);

/**
 * Returns the semantic version of the library in the form Major.Minor.Patch
 */
char const *get_version(void);

#endif //AWS_LAMBDA_C_RUNTIME_VERSION_H