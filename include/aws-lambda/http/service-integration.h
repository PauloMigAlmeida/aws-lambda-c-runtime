#pragma once

#include <stdbool.h>
#include "aws-lambda/c-runtime/runtime.h"

typedef struct {
    bool success;
    invocation_request request;
    int res_code;
} next_outcome;

typedef struct {
    bool success;
    int res_code;
} post_result_outcome;

void service_integration_init(void);
void service_integration_cleanup(void);

next_outcome request_get_next(void);
post_result_outcome request_post_result(char *request_id, invocation_response* response);