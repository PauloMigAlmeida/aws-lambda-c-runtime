//
// Created by CORP\paulo on 9/3/19.
//

#include <stdio.h>
#include "aws-lambda/c-runtime/runtime.h"

invocation_response my_handler(invocation_request request){
    return success("OK","application/json");
//    return failure("OK","zedou");
}

int main(void){
    run_handler(&my_handler);
}