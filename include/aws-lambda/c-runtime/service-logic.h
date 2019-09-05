#include "aws-lambda/c-runtime/runtime.h"
#include "aws-lambda/http/service-integration.h"

void service_logic_setup();
void service_logic_cleanup();
invocation_request* service_logic_get_next();