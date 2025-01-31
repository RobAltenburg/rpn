//
//  functions.hpp
//  rpn2
//
//  Created by Robert Altenburg on 1/30/25.
//

#ifndef functions_hpp
#define functions_hpp
#include "main.hpp"
#include "DoubleVector.hpp"

#include <stdio.h>
double toRadians (double num, State& state);
double fromRadians (double num, State& state);

void funcAdd(DoubleVector& dv, State& state);
void funcSubtract(DoubleVector& dv, State& state);
void funcMultiply(DoubleVector& dv, State& state);
void funcDivide(DoubleVector& dv, State& state);
void funcPower(DoubleVector& dv, State& state);
void funcRoot(DoubleVector& dv, State& state);
void funcReciprocal(DoubleVector& dv, State& state);
void funcChangeSign(DoubleVector& dv, State& state);
void funcModulo(DoubleVector& dv, State& state);
void funcLog(DoubleVector& dv, State& state);
void funcLog10(DoubleVector& dv, State& state);
void funcInverseLog10(DoubleVector& dv, State& state);
void funcInverseLog(DoubleVector& dv, State& state);
void funcSum(DoubleVector& dv, State& state);
void funcGamma(DoubleVector& dv, State& state);
void funcFactorial(DoubleVector& dv, State& state);

void funcSin(DoubleVector& dv, State& state);
void funcCos(DoubleVector& dv, State& state);
void funcTan(DoubleVector& dv, State& state);
void funcArcSin(DoubleVector& dv, State& state);
void funcArcCos(DoubleVector& dv, State& state);
void funcArcTan(DoubleVector& dv, State& state);
void funcArcTan2(DoubleVector& dv, State& state);

void funcSinh(DoubleVector& dv, State& state);
void funcCosh(DoubleVector& dv, State& state);
void funcTanh(DoubleVector& dv, State& state);
void funcArcSinh(DoubleVector& dv, State& state);
void funcArcCosh(DoubleVector& dv, State& state);
void funcArcTanh(DoubleVector& dv, State& state);

void funcStore(DoubleVector& dv, State& state);
void funcRecall(DoubleVector& dv, State& state);
void funcMemoryClear(DoubleVector& dv, State& state);

void funcPop(DoubleVector& dv, State& state);
void funcSwap(DoubleVector& dv, State& state);

void funcPi(DoubleVector& dv, State& state);
void funcE(DoubleVector& dv, State& state);

void funcHypotenuse(DoubleVector& dv, State& state);

void funcDegrees(DoubleVector& dv, State& state);
void funcRadians(DoubleVector& dv, State& state);
void funcGradians(DoubleVector& dv, State& state);
 
// Copy the stack to memory
void funcSave(DoubleVector& dv, State& state);
void funcRestore(DoubleVector& dv, State& state);

#endif /* functions_hpp */
