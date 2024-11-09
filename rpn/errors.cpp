//
//  errors.cpp
//  rpn
//
//  Created by Robert Altenburg on 11/9/24.
//

#include "errors.hpp"

void processError(ErrorCode code) {
    
    std::cout << "Error: " <<  getErrorMessage(code) << std::endl; // Interactuive
}

// Create a map to hold error messages
std::map<ErrorCode, std::string> errorMessageMap;

// Function to load error messages based on locale
void loadErrorMessages(const std::string& locale) {
    if (locale == "en") {
        errorMessageMap[ErrorCode::NOT_A_NUMBER] = "Not a number";
        errorMessageMap[ErrorCode::BAD_PIPE] = "Unable to open pipe to pbcopy";
        errorMessageMap[ErrorCode::BAD_MEMORY_LOCATION] = "Bad memory location, must be 0 to " + std::to_string(MEMORY_SIZE - 1);
        errorMessageMap[ErrorCode::UNKNOWN_FUNCTION] = "Unknown function";
    } else if (locale == "de") {
        // other
    }
    // Add more locales as needed
}

// Function to get the error message for a given error code
std::string getErrorMessage(ErrorCode code) {
        return errorMessageMap.at(code);
}

