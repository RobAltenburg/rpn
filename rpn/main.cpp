//
//  main.cpp
//  rpn
//
//  Created by Robert Altenburg on 11/7/24.
//

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cctype>
#include <map>
#include <functional>
#include <unistd.h>

#include "VectorWrapper.hpp"
#include "functions.hpp"
#include "readLineWithNumber.hpp"
#include "errors.hpp"

// Map input codes to their functions
void callFunction(const std::string &functionName, VectorWrapper &stack, State &runState) {
    
    // Lookup table that gives the name of the function and the actual function to run
    std::map<std::string, std::function<void(VectorWrapper&, State&)>> functionTable = {
        // in the following, x is the top of the stack y is the next element
        {"+", funcAdd},
        {"add", funcAdd}, // duplicate
        {"sum", funcSum},
        {"-", funcSubtract}, // x - y
        {"*", funcMultiply},
        {"/", funcDivide},  // x / y
        {"^", funcPower},  // y ^ x
        {"**", funcPower}, // y ^ x
        {"root", funcRoot},
        {"log", funcLog},
        {"log10", funcLog10},
        {"tenX", func10toX},
        {"eX", funcEtoX},
        {"r", funcReciprocal},
        {"chs", funcChs},   // change sign x
        {"mod", funcModulo},
        {"sin", funcSin},
        {"asin", funcArcSin},
        {"cos", funcCos},
        {"acos", funcArcCos},
        {"tan", funcTan},
        {"atan", funcArcTan},
        {"atan2", funcArcTan2},  // atan(y/x)
        {"pop", funcPop},
        {"cdr", funcPop},
        {"swp", funcSwap},
        {"pi", funcPi},
        {"e", funcE},
        {"sto", funcStore},
        {"rcl", funcRecall},
        {"copy", funcCopy}, // copy x to the clipboard
        {"cp", funcCopy},
        {"deg", funcDMStoDeg},
        {"dms",funcDegtoDMS}
    };

    // Find the function in the table
    auto it = functionTable.find(functionName);
    if (it != functionTable.end()) {
        // Call the function
        it->second(stack, runState);
    } else {
        throw(ERROR_UNKNOWN_FUNCTION);
    }
}


int main(int argc, const char * argv[]) {
    VectorWrapper stack;        // vector for the stack
    double number;              // input number
    std::string entry;          // input line
    bool runFlag = true;        // process loop
    State runState = {0};       // run state
    
    // Is the program running interactivly, or is it being piped data
    if (isatty(fileno(stdin))) {
        std::cout << "rpn:" << std::endl; // Interactuive
        runState.interactive = true;
    } else {
       // reading data from a pipe
        runState.interactive = false; // this should be the default
    }
     
    
    while (runFlag) {
        stack.print();
        if (runState.interactive) { // only prompt in interactive mode
            std::cout << "> ";
        }
        
        bool hasNumber = readLineWithNumber(number, entry); // read a line from cin
        
        if (hasNumber) {
            stack.push_back(number); // if a number was entered, push it to the stack
        }
        
        // process commands and functions
        if (entry.empty()) {
            // No function found, skipping
        } else if (entry == "q") { // quit
            runFlag = false;
        } else if (entry == "c") { // clear the stack
            stack.clear();
        } else if (entry == "look") { // print the stack
            stack.print();
        } else if (entry == "v") { // turn verbose on
            runState.verbose = true;
            std::cout << "verbose mode on" << "\n";
        } else if (entry == "!v" || entry == "v!") { // turn verbose off
            runState.verbose = false;
            std::cout << "verbose mode off" << "\n";
        } else {                       // process the function
            try {
                callFunction(entry, stack, runState);
            } catch (int errorCode) {
                processError(errorCode);
            }
        }
        entry = ""; // clear the entry
        

    }
    // could make this the default funcCopy(stack, runState); // leave the value in x in the clipboard
    return 0;
}
