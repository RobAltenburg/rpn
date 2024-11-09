//
//  VectorWrapper.cpp
//  rpn
//
//  Created by Robert Altenburg on 11/8/24.
//

#include "VectorWrapper.hpp"

void VectorWrapper::push_back(double value) {
    vec.push_back(value);
}

void VectorWrapper::clear() {
    vec.clear();
}

int64_t VectorWrapper::size() {
    return vec.size();
}

double VectorWrapper::pop() {
    if (vec.empty()) {
        return 0.0; // Return 0 if the vector is empty
    }
    double value = vec.back();
    vec.pop_back();
    return value;
}

double VectorWrapper::look() {
    if (vec.empty()) {
        return 0.0; // Return 0 if the vector is empty
    }
    return vec.back();
}

void VectorWrapper::print()  {      // print the contents of the vector
   int64_t size = (int64_t) vec.size();
    
    for (int64_t i = size - 1; i >= 0; --i) {
        if (i < 0) {
            break;
        } else if (i == 0) {
            std::cout << "x: ";
        } else if (i == 1) {
            std::cout << "y: ";
        } else if (i == 2) {
            std::cout << "z: ";
        } else {
            std::cout << i<< ": ";
        }
        std::cout << vec[size - i - 1] << std::endl;
        
    }
    
}

