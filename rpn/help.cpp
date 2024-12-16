//
//  help.cpp
//  rpn
//
//  Created by Robert Altenburg on 12/16/24.
//

#include "help.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <string>

// Initialize the data map
const std::map<std::string, std::string> dataMap = {
    {"tenX", "x=10^x"},
    {"add", "x=(x + y)"}, // duplicate
    {"+", "x=(x + y)"},
    {"acos", "x=ArcCos(x)"},
    {"arccos", "x=ArcCos(x)"},
    {"asin", "x=ArcSin(x)"},
    {"arcsin", "x=ArcSin(x)"},
    {"atan", "x=ArcTan(x)"},
    {"arctan", "x=ArcTan(x)"},
    {"atan2", "x=ArcTan(y/x)"}, // atan(y/x)
    {"arctan2", "x=ArcTan(y/x)"}, // atan(y/x)
    {"chs", "x=-1 * x"}, // change sign x
    {"copy", "copy x to clipboard"}, // copy x to the clipboard
    {"cp", "copy x to clipboard"},
    {"cos", "x=cos(x)"},
    {"dms","x:deg -> z:deg, y:min, x:sec"},
    {"/", "x=x/y"}, // x / y
    {"deg", "z:deg, y:min, x:sec -> x:deg"},
    {"e", "Constant e"},
    {"eX", "x=e^x"},
    {"lerp","funcLerp"},
    {"log", "x = log(x)"},
    {"log10", "x = log10(x)"},
    {"%", "x = x % y"},
    {"mod", "x = x % y"},
    {"*", "x = x * y"},
    {"pi", "Constant pi"},
    {"cdr", "discard x"},
    {"d", "discard x"},// delete x
    {"pop", "discard x"},
    {"**", "x = y^x"}, // y ^ x
    {"^", "x = y^x"}, // y ^ x
    {"rcl", "x = memory location x"},
    {"r", "x = 1/x"},
    {"root", "x = y^(1/x)"},
    {"sin", "x=sin(x)"},
    {"sto", "store y in location x"},
    {"-", "x = x - y"}, // x - y
    {"sum", "x = sum of stack"},
    {"swp", "swap x and y"},
    {"tan", "x=tan(x)"}
};

// Helper function to trim whitespace from a string
std::string trim(const std::string& str) {
    auto start = str.begin();
    while (start != str.end() && std::isspace(*start)) {
        ++start;
    }

    auto end = str.end();
    do {
        --end;
    } while (std::distance(start, end) > 0 && std::isspace(*end));

    return std::string(start, end + 1);
}

// Function to print all keys
void printAllHelpKeys() {
    int count = 0;
    for (const auto& pair : dataMap) {
        std::cout << std::setw(15) << std::left << pair.first;
        if (++count % 5 == 0) {
            std::cout << std::endl;
        }
    }
    if (count % 5 != 0) {
        std::cout << std::endl;
    }
    std::cout << "For additional help, type \"? <key>\"" <<std::endl;
}

// Function to print text for a specified key
void printTextForHelpKey(const std::string& key) {
    auto it = dataMap.find(key);
    if (it != dataMap.end()) {
        std::cout << it->second << std::endl;
    } else {
        std::cout << "Key not found" << std::endl;
    }
}


// Function to handle user input
void returnHelp(const std::string& entry) {
    if (entry.length() > 1) {
        std::string key = trim(entry.substr(1)); // Extract and trim the key after '?'
        printTextForHelpKey(key);
    } else {
        printAllHelpKeys();
    }
}
