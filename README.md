[![GitHub](https://img.shields.io/github/license/PauloMigAlmeida/aws-lambda-c-runtime.svg)](https://github.com/PauloMigAlmeida/aws-lambda-c-runtime/blob/master/LICENSE)
[![Build Status](https://travis-ci.com/PauloMigAlmeida/aws-lambda-c-runtime.svg?branch=master)](https://travis-ci.com/PauloMigAlmeida/aws-lambda-c-runtime)

## AWS Lambda C Runtime

C implementation of the lambda runtime API. 

## Disclaimer
Most implementation details of this project is based on Marco Magdy's nice work on the [awslabs/aws-lambda-cpp](https://github.com/awslabs/aws-lambda-cpp)
repository. 

Documentation and packaging mechanisms have been either adapted,ported or copied since I agree with most of the development
 decisions made for the Lambda Cpp runtime. Hence, all the credits should go to the AWS' guys.

## Design Goals
1. Negligible cold-start overhead (single digit millisecond).
2. Freedom of choice in compilers, build platforms and C standard library versions.

## Building and Installing the Runtime
Since AWS Lambda runs on GNU/Linux, you should build this runtime library and your logic on GNU/Linux as well.

### Prerequisites
Make sure you have the following packages installed first:
1. CMake (version 3.12 or later)
1. git
1. make
1. zip
1. libcurl-devel (on Debian-basded distros it's libcurl4-openssl-dev)

In a terminal, run the following commands:
```bash
git clone https://github.com/PauloMigAlmeida/aws-lambda-c-runtime.git
cd aws-lambda-c-runtime
mkdir build
cd build
# GCC compiler optimisation breaks the runtime..Only use debug build type
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=~/lambda-install
make && make install
```

To consume this library in a project that is also using CMake, you would do:

```cmake
cmake_minimum_required(VERSION 3.13)
project(demo
        VERSION 0.0.1
        LANGUAGES C)
set(CMAKE_C_STANDARD 11)
find_package(aws-lambda-c-runtime)
add_executable(${PROJECT_NAME} "main.c")
target_link_libraries(${PROJECT_NAME} aws-lambda-c-runtime)
# this line creates a target that packages your binary and zips it up
aws_lambda_package_target(${PROJECT_NAME})
```

And here is how a sample `main.c` would look like:
```c
#include "aws-lambda/c-runtime/runtime.h"

void my_handler(invocation_request *request, invocation_response **response){
    if(rand() == 42)
        *response = failure("Hello World from AWS Lambda C Runtime","Application_Error");
    else
        *response = success("Hello World from AWS Lambda C Runtime","text/plain");  
}

int main(void){
    run_handler(&my_handler);
    return 0;
}
```

And finally, here's how you would package it all. Run the following commands from your application's root directory:

```bash
$ mkdir build
$ cd build
$ cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=~/lambda-install
$ make
$ make aws-lambda-package-demo
```
The last command above `make aws-lambda-package-demo` will create a zip file called `demo.zip` in the current directory.

Now, create an IAM role and the Lambda function via the AWS CLI.

First create the following trust policy JSON file

```
$ cat trust-policy.json
{
 "Version": "2012-10-17",
  "Statement": [
    {
      "Effect": "Allow",
      "Principal": {
        "Service": ["lambda.amazonaws.com"]
      },
      "Action": "sts:AssumeRole"
    }
  ]
}

```
Then create the IAM role:

```bash
$ aws iam create-role --role-name lambda-demo --assume-role-policy-document file://trust-policy.json
```

Note down the role Arn returned to you after running that command. We'll need it in the next steps:

Attach the following policy to allow Lambda to write logs in CloudWatch:
```bash
$ aws iam attach-role-policy --role-name lambda-demo --policy-arn arn:aws:iam::aws:policy/service-role/AWSLambdaBasicExecutionRole
```

Make sure you attach the appropriate policies and/or permissions for any other AWS services that you plan on using.

And finally, create the Lambda function:

```
$ aws lambda create-function --function-name demo \
--role <specify role arn from previous step here> \
--runtime provided --timeout 15 --memory-size 128 \
--handler demo --zip-file fileb://demo.zip
```

And to invoke the function:
```bash
$ aws lambda invoke --function-name demo --payload '{"my_cool_payload":"yay!"}' output.txt
```

## Can I integrate it with the AWS SDK ?
Yes, you can. However, as you might already know AWS doesn't offer a SDK in C language but does offer one in C++. 

There are some examples on how to make this work in the [examples folder of this project](https://github.com/PauloMigAlmeida/aws-lambda-c-runtime/tree/master/examples/) =]

## Supported Compilers
Any *fully* compliant C11 compiler targeting GNU/Linux x86-64 should work. Please avoid compiler versions that provide half-baked C11 support.

- Use GCC v5.x or above
- Use Clang v3.3 or above

## Packaging, ABI, GNU C Library, Oh My!
Lambda runs your code on some version of Amazon Linux. It would be a less than ideal customer  experience if you are forced to build your application on that platform and that platform only.

However, the freedom to build on any linux distro brings a challenge. The GNU C Library ABI. There is no guarantee the platform used to build the Lambda function has the same GLIBC version as the one used by AWS Lambda. In fact, you might not even be using GNU's implementation. For example you could build a C Lambda function using musl libc.

To ensure that your application will run correctly on Lambda, we must package the entire C runtime library with your function.
If you choose to build on the same [Amazon Linux version used by lambda](https://docs.aws.amazon.com/lambda/latest/dg/current-supported-versions.html), you can avoid packaging the C runtime in your zip file.
This can be done by passing the `NO_LIBC` flag in CMake as follows:

```cmake
aws_lambda_package_target(${PROJECT_NAME} NO_LIBC)
```

## FAQ & Troubleshooting
1. **Why is the zip file so large? what are all those files?**
   Typically, the zip file is large because we have to package the entire C standard library.
   You can reduce the size by doing some or all of the following:
   - Ensure you're building in release mode `-DCMAKE_BUILD_TYPE=Release`
   - If possible, build your function using musl libc, it's tiny. The easiest way to do this, assuming your code is portable, is to build on Alpine linux, which uses musl libc by default.
1. **How to upload a zip file that's bigger than 50MB via the CLI?**
    Upload your zip file to S3 first:
    ```bash
    $ aws s3 cp demo.zip s3://mys3bucket/demo.zip
    ```
    NOTE: you must use the same region for your S3 bucket as the lambda.

    Then you can create the Lambda function this way:

    ```bash
    $ aws lambda create-function --function-name demo \
    --role <specify role arn here> \
    --runtime provided --timeout 15 --memory-size 128 \
    --handler demo
    --code "S3Bucket=mys3bucket,S3Key=demo.zip"
    ```
1. **My code is crashing, how can I debug it?**
   
   - Run your code locally on an Amazon Linux AMI or Docker container to reproduce the problem
     - If you go the AMI route, [use the official one](https://docs.aws.amazon.com/lambda/latest/dg/current-supported-versions.html) recommended by AWS Lambda 
     - If you go the Docker route, use the following command to launch a container running AL2017.03
       `$ docker run -v /tmp:/tmp -it --security-opt seccomp=unconfined amazonlinux:2017.03`
       The `security-opt` argument is necessary to run `gdb`, `strace`, etc.
1. **CURL problem with the SSL CA cert**
   - Make sure you are using a `libcurl` version built with OpenSSL, or one of its flavors (BoringSSL, LibreSSL)
   - Make sure you tell `libcurl` where to find the CA bundle file.
   - You can try hitting the non-TLS version of the endpoint if available. (Not Recommended).

## License

This library is licensed under the Apache 2.0 License. 
