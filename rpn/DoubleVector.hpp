//
//  DoubleVector.hpp
//  rpn2
//
//  Created by Robert Altenburg on 1/30/25.
//

#ifndef DoubleVector_hpp
#define DoubleVector_hpp

#include <stdio.h>
#include <vector>

class DoubleVector {
public:
    void push(double value);
    double pop();
    double x();
    double y();
    double z();
    double at(int n);
    unsigned long size();
    void removeLeadingZeros();
    const std::vector<double>& getVector() const;

private:
    std::vector<double> vec;
};


#endif /* DoubleVector_hpp */
