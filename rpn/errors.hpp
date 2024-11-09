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

#define ERROR_NAN                 0
#define ERROR_COPY                  1
#define ERROR_BAD_MEMORY_LOCATION   2
#define ERROR_UNKNOWN_FUNCTION      3

const std::string errorMessages[] = {
    "Not a number",
    "Failed to open pipe to pbcopy",
    "Bad memory location",  //        std::cout << "Memory must be an integer from 0 through " << MEMORY_SIZE -1 << std::endl;
    "Unknown function"
};

void processError(int);

#endif /* errors_hpp */
