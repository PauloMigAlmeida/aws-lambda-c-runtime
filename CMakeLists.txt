cmake_minimum_required(VERSION 3.13)
project(aws-lambda-c-runtime
        VERSION 0.0.1
        LANGUAGES C)

set(CMAKE_C_STANDARD 11)

add_library(${PROJECT_NAME}
        "src/runtime.c"
        "src/service-logic.c"
        "src/service-integration.c"
        "src/string-builder.c"
        "src/hashmap.c"
        "${CMAKE_CURRENT_BINARY_DIR}/version.c"
)

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})

target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

find_package(CURL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE CURL::libcurl)

target_include_directories(${PROJECT_NAME} PRIVATE ${CURL_INCLUDE_DIRS})

#versioning
configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/src/version.c.in"
        "${CMAKE_CURRENT_BINARY_DIR}/version.c"
        NEWLINE_STYLE LF)

# installation
install(FILES
        "include/aws-lambda/ext/string-builder.h"
        "include/aws-lambda/ext/hashmap.h"
        "include/aws-lambda/http/jsmh.h"
        "include/aws-lambda/http/service-integration.h"
        "include/aws-lambda/c-runtime/runtime.h"
        "include/aws-lambda/c-runtime/version.h"
        "include/aws-lambda/c-runtime/service-logic.h"
        DESTINATION "include/aws-lambda/c-runtime")

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}-targets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

configure_file("${CMAKE_SOURCE_DIR}/cmake/${PROJECT_NAME}-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
        @ONLY)

export(EXPORT "${PROJECT_NAME}-targets")

install(EXPORT "${PROJECT_NAME}-targets"
        DESTINATION "lib/${PROJECT_NAME}/cmake/"
        )

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
        DESTINATION "lib/${PROJECT_NAME}/cmake/")

install(PROGRAMS "${CMAKE_SOURCE_DIR}/packaging/packager"
        DESTINATION "lib/${PROJECT_NAME}/cmake/")