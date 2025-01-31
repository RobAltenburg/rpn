//
//  main.hpp
//  rpn2
//
//  Created by Robert Altenburg on 1/29/25.
//

#ifndef main_hpp
#define main_hpp

#include <iostream>
#include <regex>
#include <string>
#include <sstream>
#include <unistd.h> // delay loop

#include <map>
#include <set>
#include "termbox2.h"
//#include "functions.hpp"


#define MEMORY_SIZE 100

#define ERROR_NAN -999
#define ERROR_MEM -888

#define DEGREES 0
#define RADIANS 1
#define GRADIANS 2

struct State {
    int drg;  // 0 = degrees, 1 = radians, 2 = gradians
    double memory[MEMORY_SIZE] = {0}; //memory slots 0 to MEMORY_SIZE - 1
    int precision = 10;
};

std::string formatDouble(double value, State& state);

#endif /* main_hpp */
