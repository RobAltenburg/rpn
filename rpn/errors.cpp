//
//  errors.cpp
//  rpn
//
//  Created by Robert Altenburg on 11/9/24.
//

#include "errors.hpp"

void processError(int code) {
    
    std::cout << "Error: " <<  errorMessages[code] << std::endl; // Interactuive
}
