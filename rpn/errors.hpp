//
//  errors.hpp
//  rpn
//
//  Created by Robert Altenburg on 11/9/24.
//

#ifndef errors_hpp
#define errors_hpp

#include <stdio.h>
#include <iostream>
#include <map>
#include <string>
#include "functions.hpp"


enum class ErrorCode {
    NOT_A_NUMBER,
    BAD_PIPE,
    BAD_MEMORY_LOCATION,
    UNKNOWN_FUNCTION
    // Add more error codes as needed and update messages in errors.cpp
};


void processError(ErrorCode);

void loadErrorMessages(const std::string& locale);
std::string getErrorMessage(ErrorCode code);

#endif /* errors_hpp */
