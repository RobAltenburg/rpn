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

#include "VectorWrapper.hpp"
#include "functions.hpp"
#include "readLineWithNumber.hpp"
#include "errors.hpp"

using VariantType = std::variant<double, std::monostate>;
int readLineWithVariant(VariantType &number, std::string &text);

#endif /* main_h */
