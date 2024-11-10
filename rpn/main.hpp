//
//  main.hpp
//  rpn
//
//  Created by Robert Altenburg on 11/9/24.
//

#ifndef main_h
#define main_h

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cctype>
#include <map>
#include <functional>
#include <unistd.h>

#define MEMORY_SIZE 100

struct State {
    int drg;  // 0 = degrees, 1 = radians, 2 = gradians
    bool verbose = false;
    bool interactive = false;
    bool eof = false;
    double memory[MEMORY_SIZE]; //memory slots 0 to MEMORY_SIZE - 1
};

#include "VectorWrapper.hpp"
#include "functions.hpp"
#include "readLineWithVariant.hpp"
#include "errors.hpp"

using VariantType = std::variant<double, std::monostate>;
int readLineWithVariant(VariantType &number, std::string &text);

#endif /* main_h */
