#include "aws-lambda/c-runtime/runtime.h"
#include "aws-lambda/c-runtime/version.h"
#include "aws-lambda/c-runtime/service-logic.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Create a successful invocation response with the given payload and content-type.
 */
invocation_response success(char* payload, char* content_type){
    invocation_response r;
    r.success = true;
    r.content_type = content_type;
    r.payload = payload;
    return r;
}

/**
 * Create a failure response with the given error message and error type.
 * The content-type is always set to application/json in this case.
 */
invocation_response failure(char* error_message, char* error_type){
    char* template = "{\"errorMessage\":\"%s\",\"errorType\":\"%s\", \"stackTrace\":[]})";

    invocation_response r;
    r.success = false;
    r.content_type = "application/json";

    char *tmp_str = malloc(strlen(template) + strlen(error_message) + strlen(error_type) + 1);
    sprintf(tmp_str, template, error_message, error_type);
    r.payload = tmp_str;

    return r;
}

// Entry method
void run_handler(invocation_response (*handler)(invocation_request request)){
    printf("Initializing the C Lambda Runtime version %s\n", get_version());

    service_logic_setup();

    invocation_request *req = service_logic_get_next();
    if(req != NULL){
        printf("Invoking user handler\n");
        invocation_response res =  handler(*req);
        printf("Invoking user handler completed.\n");
        printf("\tresponse payload: %s\n", res.payload);

        // freeing up dynamic allocated resources
        if(!res.success)
            free(res.payload); // failure callbacks allocates a dynamic string
        free(req->payload);
        free(req);
    }

    service_logic_cleanup();

}