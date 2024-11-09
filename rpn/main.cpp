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

#include "VectorWrapper.hpp"
#include "functions.hpp"
#include "readLineWithNumber.hpp"

// Function to call the appropriate function based on the input string
void callFunction(const std::string &functionName, VectorWrapper &stack, State &runState) {
    
    uint8_t errorCode;
    // Lookup table that gives the name of the function and the actual function to run
    std::map<std::string, std::function<uint8_t(VectorWrapper&, State&)>> functionTable = {
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
        {"swp", funcSwap},
        {"pi", funcPi},
        {"e", funcE},
        {"sto", funcStore},
        {"rcl", funcRecall},
        {"copy", funcCopy}, // copy x to the clipboard
        {"cp", funcCopy}
        
    };

    // Find the function in the table
    auto it = functionTable.find(functionName);
    if (it != functionTable.end()) {
        // Call the function
        errorCode = it->second(stack, runState);
        if (errorCode > 0) {
            std::cerr << "Error: " << errorCode << std::endl;
        }
    } else {
        std::cerr << "Error: Function " << functionName << " not found" << std::endl;
    }
}


int main(int argc, const char * argv[]) {
    VectorWrapper stack;        // vector for the stack
    double number;              // input number
    std::string entry;          // input line
    bool runFlag = true;        // process loop
    State runState = {0};       // run state
    
    while (runFlag) {
        stack.print();
        std::cout << "> ";
        bool hasNumber = readLineWithNumber(number, entry);
        
        if (hasNumber) {
            stack.push_back(number); // if a number was entered, push it
        }
        
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
        } else if (entry == "!v") { // turn verbose off
            runState.verbose = false;
        } else {                       // process the function
            callFunction(entry, stack, runState);
            //std::cout << stack.look() << std::endl;
            entry = ""; // clear the entry
        }
            
        //std:getline(std::cin, line);
        //std::cout << line << "\n";

    }
    // could make this the default funcCopy(stack, runState); // leave the value in x in the clipboard
    return 0;
}
