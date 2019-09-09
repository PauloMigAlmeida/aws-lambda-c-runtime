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

/*
 * implementation based on the official AWS CPP SDK example:
 * https://github.com/awsdocs/aws-doc-sdk-examples/blob/master/cpp/example_code/s3/list_buckets.cpp
 */

#include "awsbridge.h"

#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/Bucket.h>

invocation_response list_buckets() {
    std::string contentType ("text/plain; charset=utf-8");
    std::string ret("Your Amazon S3 buckets: ");

    Aws::SDKOptions options;
    Aws::InitAPI(options);
    {
        Aws::S3::S3Client s3_client;
        auto outcome = s3_client.ListBuckets();

        if (outcome.IsSuccess()) {
            Aws::Vector <Aws::S3::Model::Bucket> bucket_list = outcome.GetResult().GetBuckets();

            for (auto const &bucket : bucket_list) {
                ret += bucket.GetName().c_str() + std::string(", ");
            }
        } else {
            ret += std::string("ListBuckets error: ") + outcome.GetError().GetExceptionName().c_str() + std::string(" - ")
                      + outcome.GetError().GetMessage().c_str();
        }
    }
    Aws::ShutdownAPI(options);

    char* payload = (char*)malloc(ret.length() + 1);
    strcpy(payload, ret.c_str());

    char* content_type = (char*)malloc(contentType.length() + 1);
    strcpy(content_type, contentType.c_str());

    return success(payload, content_type);
}