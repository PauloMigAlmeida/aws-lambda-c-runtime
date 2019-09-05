#pragma once
#include <stdbool.h>

typedef struct{
    /**
     * The user's payload represented as a UTF-8 string.
     */
    char* payload;

    /**
     * An identifier unique to the current invocation.
     */
    char* request_id;

    /**
     * X-Ray tracing ID of the current invocation.
     */
    char* xray_trace_id;

    /**
     * Information about the client application and device when invoked through the AWS Mobile SDK.
     */
    char* client_context;

    /**
     * Information about the Amazon Cognito identity provider when invoked through the AWS Mobile SDK.
     */
    char* cognito_identity;

    /**
     * The ARN requested. This can be different in each invoke that executes the same version.
     */
    char* function_arn;

//    /**
//     * Function execution deadline counted in milliseconds since the Unix epoch.
//     */
//    std::chrono::time_point<std::chrono::system_clock> deadline;

//    /**
//     * The number of milliseconds left before lambda terminates the current execution.
//     */
//    inline std::chrono::milliseconds get_time_remaining() const;
} invocation_request;

typedef struct{
    /**
     * The output of the function which is sent to the lambda caller.
     */
    char* payload;

    /**
     * The MIME type of the payload.
     * This is always set to 'application/json' in unsuccessful invocations.
     */
    char* content_type;

    /**
     * Flag to distinguish if the contents are for successful or unsuccessful invocations.
     */
    bool success;
} invocation_response;


/**
 * Create a successful invocation response with the given payload and content-type.
 */
invocation_response success(char* payload, char* content_type);

/**
 * Create a failure response with the given error message and error type.
 * The content-type is always set to application/json in this case.
 */
invocation_response failure(char* error_message, char* error_type);

// Entry method
void run_handler(invocation_response (*handler)(invocation_request request));