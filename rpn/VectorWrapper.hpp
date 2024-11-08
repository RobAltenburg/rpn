//
//  VectorWrapper.hpp
//  rpn
//
//  Created by Robert Altenburg on 11/8/24.
//

#ifndef VectorWrapper_hpp
#define VectorWrapper_hpp

#include <vector>
#include <stdexcept>
#include <iostream>

class VectorWrapper {
private:
    std::vector<double> vec;
    
public:
    void push_back(double);
    void clear();
    double pop();
    double look();
    void print();
};



#endif /* VectorWrapper_hpp */
