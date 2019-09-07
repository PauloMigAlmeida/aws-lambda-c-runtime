cmake_minimum_required(VERSION 3.12)
project(minimalist
        VERSION 0.0.1
        LANGUAGES C)

set(CMAKE_C_STANDARD 11)

find_package(aws-lambda-c-runtime)

add_executable(${PROJECT_NAME} "main.c")

target_link_libraries(${PROJECT_NAME} aws-lambda-c-runtime)

aws_lambda_package_target(${PROJECT_NAME})