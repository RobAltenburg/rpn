//
//  VectorWrapper.cpp
//  rpn
//
//  Created by Robert Altenburg on 11/8/24.
//

#include "VectorWrapper.hpp"

void VectorWrapper::push_back(long double value) {
    vec.push_back(value);
}

void VectorWrapper::clear() {
    vec.clear();
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

