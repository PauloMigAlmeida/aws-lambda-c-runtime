cmake_minimum_required(VERSION 3.12)
set(CMAKE_C_STANDARD 11)
project(s3-list-buckets)
enable_language(C CXX)

find_package(aws-lambda-c-runtime)
find_package(AWSSDK COMPONENTS s3)

include_directories(${PROJECT_SOURCE_DIR}/include)
add_executable(
        ${PROJECT_NAME}
        "src/main.c"
        "src/awsbridge.cpp"
)

target_link_libraries(${PROJECT_NAME} PRIVATE aws-lambda-c-runtime ${AWSSDK_LINK_LIBRARIES})

target_compile_options(${PROJECT_NAME} PRIVATE
    "-Wall"
    "-Wextra"
    "-Wconversion"
    "-Wshadow"
    "-Wno-sign-conversion")

target_compile_features(${PROJECT_NAME} PRIVATE "c_std_11")

aws_lambda_package_target(${PROJECT_NAME})

