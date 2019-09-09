# Example using the AWS C++ SDK with Lambda

We'll build a lambda that lists all S3 buckets in your account (given that the Lambda IAM role has permissions to s3:ListBuckets) and return them as text/plain content.

## Build the AWS C++ SDK
Start by building the SDK from source.
```bash
$ mkdir ~/install
$ git clone https://github.com/aws/aws-sdk-cpp.git
$ cd aws-sdk-cpp
$ mkdir build
$ cd build
$ cmake .. -DBUILD_ONLY="s3" \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=OFF \
  -DCUSTOM_MEMORY_MANAGEMENT=OFF \
  -DCMAKE_INSTALL_PREFIX=~/install \
  -DENABLE_UNITY_BUILD=ON

$ make
$ make install
```

## Build the Runtime
Now let's build the C Lambda runtime, so in a separate directory clone this repository and follow these steps:

```bash
$ git clone https://github.com/PauloMigAlmeida/aws-lambda-c-runtime.git
$ cd aws-lambda-c-runtime
$ mkdir build
$ cd build
$ cmake .. -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=~/install \
$ make
$ make install
```

## Build the application
The last step is to build the Lambda function in `main.c` and run the packaging command as follows:

```bash
$ cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=~/install
$ make
$ make make aws-lambda-package-s3-list-buckets
```

You should now have a zip file called `s3-list-buckets.zip`. Follow the instructions in the main README to upload it and invoke the lambda.
