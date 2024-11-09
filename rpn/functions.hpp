//
//  functions.hpp
//  rpn
//
//  Created by Robert Altenburg on 11/8/24.
//

#ifndef functions_hpp
#define functions_hpp

#include "VectorWrapper.hpp"
#include "errors.hpp"
#include "main.hpp"
#include <limits>
#include <cmath>
#include <string>
#include <iostream>
#include <sstream>


// struct State
//
struct State {
    int drg;  // 0 = degrees, 1 = radians, 2 = gradians
    double memory[MEMORY_SIZE]; //memory slots 0 to MEMORY_SIZE - 1
    bool verbose = false;
    bool interactive = false;
};

// helper functions
double drgConversion (State &state);
bool isInteger(double);
void printDetails(State &state, const std::string& message);

// basic math
void funcAdd(VectorWrapper &stack, State &state);
void funcSum(VectorWrapper &stack, State &state);
void funcSubtract(VectorWrapper &stack, State &state);
void funcMultiply(VectorWrapper &stack, State &state);
void funcDivide(VectorWrapper &stack, State &state);
void funcPower(VectorWrapper &stack, State &state);
void funcRoot(VectorWrapper &stack, State &state);
void funcReciprocal(VectorWrapper &stack, State &state);
void funcChs(VectorWrapper &stack, State &state);
void funcModulo(VectorWrapper &stack, State &state);
void funcLog(VectorWrapper &stack, State &state);
void funcLog10(VectorWrapper &stack, State &state);
void func10toX(VectorWrapper &stack, State &state);
void funcEtoX(VectorWrapper &stack, State &state);

// trig
void funcSin(VectorWrapper &stack, State &state);
void funcCos(VectorWrapper &stack, State &state);
void funcTan(VectorWrapper &stack, State &state);
void funcArcSin(VectorWrapper &stack, State &state);
void funcArcCos(VectorWrapper &stack, State &state);
void funcArcTan(VectorWrapper &stack, State &state);
void funcArcTan2(VectorWrapper &stack, State &state);

//tertiary
void funcDMStoDeg(VectorWrapper &stack, State &state);
void funcDegtoDMS(VectorWrapper &stack, State &state);

// stack
void funcPop(VectorWrapper &stack, State &state);
void funcSwap(VectorWrapper &stack, State &state);

//constants
void funcPi(VectorWrapper &stack, State &state);
void funcE(VectorWrapper &stack, State &state);

//interpolation
void funcLerp(VectorWrapper &stack, State &state);

//memory
void funcStore(VectorWrapper &stack, State &state);
void funcRecall(VectorWrapper &stack, State &state);
void funcCopy(VectorWrapper &stack, State &state);
//void funcPaste(VectorWrapper &stack, State &state);

#endif /* functions_hpp */
