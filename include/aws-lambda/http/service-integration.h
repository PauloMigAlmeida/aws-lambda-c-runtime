#pragma once

#include <stdbool.h>
#include "aws-lambda/c-runtime/runtime.h"

typedef struct {
    bool success;
    invocation_request request;
    int res_code; // TODO ver se vou precisar de fato desse campo
} next_outcome;

void service_integration_init();
void service_integration_cleanup();

next_outcome request_get_next();