//
//  DoubleVector.cpp
//  rpn2
//
//  Created by Robert Altenburg on 1/30/25.
//

#include "DoubleVector.hpp"

double DoubleVector::x() {
    if (vec.empty()) {
        return 0.0; // Return 0 if the vector is empty
    }
    return vec.back(); // Access the last element without removing it
}

double DoubleVector::y() {
    if (vec.size() < 2) {
        return 0.0; // Return 0 if the vector has fewer than 2 elements
    }
    return vec[vec.size() - 2]; // Access the second-to-last element
}

double DoubleVector::z() {
    if (vec.size() < 3) {
        return 0.0; // Return 0 if the vector has fewer than 2 elements
    }
    return vec[vec.size() - 3]; // Access the second-to-last element
}

void DoubleVector::push(double value) {
    vec.push_back(value);
}

double DoubleVector::pop() {
    if (vec.empty()) {
        return 0.0;
    } else {
        double value = vec.back();
        vec.pop_back();
        return value;
    }
}

unsigned long DoubleVector::size() {
    return vec.size();
}

const std::vector<double>& DoubleVector::getVector() const {
    return vec;
}

double DoubleVector::at(int n) {
   if (vec.size() > n) {
       n++;
        return vec[vec.size() - n];
    } else {
        return 0;
    }

}


void DoubleVector::removeLeadingZeros() {
    while (!vec.empty() && (vec[0] == 0)) {
        vec.erase(vec.begin());
    }
}
