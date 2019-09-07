//
// Created by CORP\paulo on 9/3/19.
//

#include <stdio.h>
#include "aws-lambda/c-runtime/runtime.h"

invocation_response my_handler(invocation_request request){
    return success("All good","text/plain");
//    return failure("Something went wrong","Application-Error");
}

int main(void){
    run_handler(&my_handler);
    return 0;
}