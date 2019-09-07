#include "aws-lambda/c-runtime/runtime.h"
#include "aws-lambda/http/service-integration.h"

void service_logic_setup(void);
void service_logic_cleanup(void);
invocation_request* service_logic_get_next(void);
void service_logic_post_result(char *request_id, invocation_response *response);