//
//  readLineWithNumber.cpp
//  rpn
//
//  Created by Robert Altenburg on 11/8/24.
//

#include "readLineWithNumber.hpp"

// TODO:  fix this so it parses correctly

bool readLineWithNumber(double &number, std::string &text) {
    std::string line;
    std::getline(std::cin, line);

    std::istringstream iss(line);
    bool hasNumber = (iss >> number)  ? true : false;

    // Extract the remaining text
    std::getline(iss >> std::ws, text);
    
    if (!hasNumber) {
        text = line; // If no number, the entire line is text
    }

    return hasNumber;
}
