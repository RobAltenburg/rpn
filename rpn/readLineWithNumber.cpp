//
//  readLineWithNumber.cpp
//  rpn
//
//  Created by Robert Altenburg on 11/8/24.
//

#include "readLineWithNumber.hpp"

int readLineWithVarient(VariantType &number, std::string &text) {
    std::string line;
    std::getline(std::cin, line);
    
    std::istringstream iss(line);
    double tempNumber;
    if (iss >> tempNumber) {
        number = tempNumber;
        std::getline(iss >> std::ws, text); // extract the remainder of the line
    } else {
        number = std::monostate{};      // the number is null
        text = line;                    // the whole line is text
    }
    if (std::cin.eof()) {
        return EOF;
    } else {
        return 0;
    }

}
