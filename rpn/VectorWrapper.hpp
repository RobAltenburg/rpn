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

class VectorWrapper {
private:
    std::vector<long double> vec;
    
public:
    void push_back(long double);
    void clear();
    double pop();
    double look();
};



#endif /* VectorWrapper_hpp */
