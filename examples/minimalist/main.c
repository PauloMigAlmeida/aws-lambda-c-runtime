//
// Created by CORP\paulo on 9/3/19.
//
#include "aws-lambda/c-runtime/runtime.h"

invocation_response my_handler(invocation_request request){
    if(rand() == 42)
        return failure("Something went wrong","Application-Error");
    else
        return success("All good","text/plain");
}

int main(void){
    run_handler(&my_handler);
    return 0;
}