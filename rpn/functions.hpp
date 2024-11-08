//
//  functions.hpp
//  rpn
//
//  Created by Robert Altenburg on 11/8/24.
//

#ifndef functions_hpp
#define functions_hpp

#include "VectorWrapper.hpp"
#include <limits>
#include <cmath>

#define ERROR_INFINITY 1

// struct State
//
struct State {
    int drg;  // 0 = degrees, 1 = radians, 2 = gradians
    double memory[10]; //memory slots 0 to 9
};

// helper functions
double drgConversion (State &state);
bool isInteger(double);

// basic math
uint8_t funcAdd(VectorWrapper &stack, State &state);
uint8_t funcSubtract(VectorWrapper &stack, State &state);
uint8_t funcMultiply(VectorWrapper &stack, State &state);
uint8_t funcDivide(VectorWrapper &stack, State &state);
uint8_t funcPower(VectorWrapper &stack, State &state);
uint8_t funcRoot(VectorWrapper &stack, State &state);
uint8_t funcReciprocal(VectorWrapper &stack, State &state);
uint8_t funcChs(VectorWrapper &stack, State &state);
uint8_t funcModulo(VectorWrapper &stack, State &state);
uint8_t funcLog(VectorWrapper &stack, State &state);
uint8_t funcLog10(VectorWrapper &stack, State &state);
uint8_t func10toX(VectorWrapper &stack, State &state);
uint8_t funcEtoX(VectorWrapper &stack, State &state);

// trig
uint8_t funcSin(VectorWrapper &stack, State &state);
uint8_t funcCos(VectorWrapper &stack, State &state);
uint8_t funcTan(VectorWrapper &stack, State &state);
uint8_t funcArcSin(VectorWrapper &stack, State &state);
uint8_t funcArcCos(VectorWrapper &stack, State &state);
uint8_t funcArcTan(VectorWrapper &stack, State &state);
uint8_t funcArcTan2(VectorWrapper &stack, State &state);

// stack
uint8_t funcPop(VectorWrapper &stack, State &state);
uint8_t funcSwap(VectorWrapper &stack, State &state);

//constants
uint8_t funcPi(VectorWrapper &stack, State &state);
uint8_t funcE(VectorWrapper &stack, State &state);

//memory
uint8_t funcStore(VectorWrapper &stack, State &state);
uint8_t funcRecall(VectorWrapper &stack, State &state);

#endif /* functions_hpp */
