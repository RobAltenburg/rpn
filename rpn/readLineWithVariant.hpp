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

#include <variant>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <set>
#include "main.hpp"

int readLineWithVarient(std::variant<double, std::monostate> &number, std::string &text);

void setNonBlockingInput();
void resetBlockingInput();
bool kbhit();

#endif /* readLineWithNumber_hpp */
