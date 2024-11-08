//
//  functions.cpp
//  rpn
//
//  Created by Robert Altenburg on 11/8/24.
//

#include "functions.hpp"
#include <cmath>

// basic math
void funcAdd(VectorWrapper &stack) {
    stack.push_back(stack.pop() + stack.pop());
}
void funcSubtract(VectorWrapper &stack) {
    double temp = stack.pop();
    stack.push_back(stack.pop() - temp);
}
void funcMultiply(VectorWrapper &stack) {
    stack.push_back(stack.pop() * stack.pop());
}
void funcDivide(VectorWrapper &stack) {
    double temp = stack.pop();
    stack.push_back(stack.pop() / temp);
}
void funcPower(VectorWrapper &stack) {
    double temp = stack.pop();
    stack.push_back(pow(stack.pop(), temp));
}
void funcReciprocal(VectorWrapper &stack) {
    stack.push_back(1.0 / stack.pop());
}
void funcChs(VectorWrapper &stack) {
    stack.push_back(stack.pop() * -1.0);
}
