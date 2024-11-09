//
//  readLineWithNumber.hpp
//  rpn
//
//  Created by Robert Altenburg on 11/8/24.
//

#ifndef readLineWithNumber_hpp
#define readLineWithNumber_hpp

#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include "main.hpp"

int readLineWithVarient(std::variant<double, std::monostate> &number, std::string &text);

#endif /* readLineWithNumber_hpp */
