//
//  functions.hpp
//  rpn
//
//  Created by Robert Altenburg on 11/8/24.
//

#ifndef functions_hpp
#define functions_hpp

#include "VectorWrapper.hpp"

// basic math
void funcAdd(VectorWrapper &stack);
void funcSubtract(VectorWrapper &stack);
void funcMultiply(VectorWrapper &stack);
void funcDivide(VectorWrapper &stack);
void funcPower(VectorWrapper &stack);
void funcReciprocal(VectorWrapper &stack);
void funcChs(VectorWrapper &stack);

#endif /* functions_hpp */
