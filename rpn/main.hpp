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

#define MODE_DEC 0
#define MODE_BIN 1
#define MODE_OCT 2
#define MODE_HEX 3

// numbers higher than this are always scientific notation.
#define MAX_FIX 1e+10

struct State {
    int drg;  // 0 = degrees, 1 = radians, 2 = gradians
    double memory[MEMORY_SIZE] = {0}; //memory slots 0 to MEMORY_SIZE - 1
    int precision = 10;
    int mode = MODE_DEC;
    std::string last_command = "";
};

std::string formatDouble(double value, State& state);
void displayHelp(void);

#endif /* main_hpp */
