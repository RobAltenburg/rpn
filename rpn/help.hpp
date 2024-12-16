//
//  help.hpp
//  rpn
//
//  Created by Robert Altenburg on 12/16/24.
//

#ifndef HELP_MAP_HPP
#define HELP_MAP_HPP

#include <map>
#include <string>

// Define the data map
extern const std::map<std::string, std::string> dataMap;

// Function declarations
void printAllHelpKeys();
void printTextForHelpKey(const std::string& key);
void returnHelp(const std::string& entry);

#endif // HELP_MAP_HPP
