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

// Function to call the appropriate function based on the input string
void callFunction(const std::string &functionName, VectorWrapper &stack) {
    // Lookup table
    std::map<std::string, std::function<void(VectorWrapper&)>> functionTable = {
        {"+", funcAdd},
        {"-", funcSubtract},
        {"*", funcMultiply},
        {"/", funcDivide},
        {"^", funcPower},
        {"r", funcReciprocal},
        {"chs", funcChs}
    };

    // Find the function in the table
    auto it = functionTable.find(functionName);
    if (it != functionTable.end()) {
        // Call the function
        it->second(stack);
    } else {
        std::cerr << "Error: Function " << functionName << " not found" << std::endl;
    }
}

bool readLineWithNumber(long double &number, std::string &text) {
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


int main(int argc, const char * argv[]) {
    VectorWrapper stack;  // vector for the stack
    long double number;              // input number
    std::string entry;       // input line
    bool runFlag = true;        // process loop
    
    while (runFlag) {
        bool hasNumber = readLineWithNumber(number, entry);
        
        if (hasNumber) {
            stack.push_back(number); // if a number was entered, push it
        }
        
        if (entry.empty()) {
            //std::cout << "no function" << std::endl;
        } else if (entry == "q") {
            runFlag = false;
        } else if (entry == "c") {
            stack.clear();
        }else {
            callFunction(entry, stack);
            std::cout << stack.look() << std::endl;
            entry = "";
        }
            
        //std:getline(std::cin, line);
        //std::cout << line << "\n";

    }
    return 0;
}
