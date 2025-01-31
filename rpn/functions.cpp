//
//  functions.cpp
//  rpn2
//
//  Created by Robert Altenburg on 1/30/25.
//

#include "functions.hpp"

// conversions
double toRadians (double num, State& state) {
    if (state.drg == 0) {           // convert degrees to rad
        return num * (M_PI / 180.0);
    } else if (state.drg == 1) {    // leave it rad
        return num;
    } else  {                       // convert gradians to rad
        return num * 0.01570796;
    }
}

double fromRadians (double num, State& state) {
    if (state.drg == 0) {           // convert degrees to rad
        return num * (180.0 / M_PI);
    } else if (state.drg == 1) {    // leave it rad
        return num;
    } else  {                       // convert gradians to rad
        return num * 63.66198;
    }
}

// Basic functions
void funcAdd(DoubleVector& dv, State& state) {
    dv.push(dv.pop() + dv.pop());
}
void funcSubtract(DoubleVector& dv, State& state) {
    double y = dv.pop();
    dv.push(dv.pop() - y);
}
void funcMultiply(DoubleVector& dv, State& state) {
    dv.push(dv.pop() * dv.pop());
}
void funcDivide(DoubleVector& dv, State& state) {
    double x = dv.pop();
    if (x==0) throw ERROR_NAN;
    dv.push(dv.pop() / x);
}

void funcModulo(DoubleVector& dv, State& state){
    double x = dv.pop();
    if (x == 0) throw ERROR_NAN;
    dv.push(fmod(dv.pop(),x));
}

void funcPower(DoubleVector& dv, State& state) {
    double y = dv.pop();
    dv.push(pow(dv.pop() , y));
}
void funcRoot(DoubleVector& dv, State& state) {
    double x = dv.pop();
    if (x == 0) {
        throw ERROR_NAN;
    } else {
        dv.push(pow(dv.pop() , 1/x));
    }
}
void funcChangeSign(DoubleVector& dv, State& state) {
    dv.push(dv.pop() * -1.0);
}

void funcReciprocal(DoubleVector& dv, State& state) {
    double x = dv.pop();
    if (x == 0) {
        throw ERROR_NAN;
    } else {
        dv.push(1/x);
    }
}

void funcSin(DoubleVector& dv, State& state){
    double rads = toRadians(dv.pop(), state);
    dv.push(sin(rads));
}
void funcCos(DoubleVector& dv, State& state){
    double rads = toRadians(dv.pop(), state);
    dv.push(cos(rads));
}
void funcTan(DoubleVector& dv, State& state){
    double rads = toRadians(dv.pop(), state);
    //todo trap errors
    dv.push(tan(rads));
}
void funcArcSin(DoubleVector& dv, State& state){
    dv.push(fromRadians(asin(dv.pop()),state));
}
void funcArcCos(DoubleVector& dv, State& state){
    dv.push(fromRadians(acos(dv.pop()),state));
}
void funcArcTan(DoubleVector& dv, State& state){
    dv.push(fromRadians(atan(dv.pop()),state));
}
void funcArcTan2(DoubleVector& dv, State& state){
    double x = dv.pop();
    if (x == 0) {
        throw ERROR_NAN;
    } else {
        dv.push(fromRadians(atan2(dv.pop(), x),state));
    }
}


void funcSinh(DoubleVector& dv, State& state){
    double rads = toRadians(dv.pop(), state);
    dv.push(sinh(rads));
}
void funcCosh(DoubleVector& dv, State& state){
    double rads = toRadians(dv.pop(), state);
    dv.push(cosh(rads));
}
void funcTanh(DoubleVector& dv, State& state){
    double rads = toRadians(dv.pop(), state);
    //todo trap errors
    dv.push(tanh(rads));
}
void funcArcSinh(DoubleVector& dv, State& state){
    dv.push(fromRadians(asinh(dv.pop()),state));
}
void funcArcCosh(DoubleVector& dv, State& state){
    dv.push(fromRadians(acosh(dv.pop()),state));
}
void funcArcTanh(DoubleVector& dv, State& state){
    dv.push(fromRadians(atanh(dv.pop()),state));
}

void funcPop(DoubleVector& dv, State& state) {
    dv.pop();
}

void funcLog(DoubleVector& dv, State& state) {
    dv.push(log(dv.pop()));
}

void funcInverseLog(DoubleVector& dv, State& state) {
    dv.push(pow(M_E, dv.pop()));
}

void funcLog10(DoubleVector& dv, State& state) {
    dv.push(log10(dv.pop()));
}
                    
void funcInverseLog10(DoubleVector& dv, State& state) {
    dv.push(pow(10, dv.pop()));
}

void funcSwap(DoubleVector& dv, State& state) {
    double x = dv.pop();
    double y = dv.pop();
    dv.push(x);
    dv.push(y);
}

void funcStore(DoubleVector& dv, State& state) {
    int slot = (int) dv.pop();
    if (slot > MEMORY_SIZE) {
        throw ERROR_MEM;
    } else {
        state.memory[slot] = dv.x();
    }
}
void funcRecall(DoubleVector& dv, State& state) {
    int slot = (int) dv.pop();
    if (slot > MEMORY_SIZE) {
        throw ERROR_MEM;
    } else {
        dv.push(state.memory[slot]);
    }
}

void funcMemoryClear(DoubleVector& dv, State& state) {
    for (int i = 0; i < MEMORY_SIZE; i++) {
        state.memory[i] = 0;
    }
}

void funcPi(DoubleVector& dv, State& state) {
    dv.push(M_PI);
}

void funcE(DoubleVector& dv, State& state) {
    dv.push(M_E);
}

void funcHypotenuse(DoubleVector& dv, State& state) {
    double x = dv.pop();
    double y = dv.pop();
    dv.push(hypot(x,y));
}


void funcDegrees(DoubleVector& dv, State& state) {
    state.drg = DEGREES;
}

void funcRadians(DoubleVector& dv, State& state) {
    state.drg = RADIANS;
}

void funcGradians(DoubleVector& dv, State& state) {
    state.drg = GRADIANS;
}
 

// sum the entire stack
void funcSum(DoubleVector& dv, State& state) {
    double value = 0;
    for (int i = 0; i <= dv.size() + 1; i++) {
        value += dv.pop();
    }
    dv.push(value);
}


// sum the entire stack
void funcSave(DoubleVector& dv, State& state) {
    if (dv.size() > MEMORY_SIZE) {
        throw ERROR_MEM;
    } else {
        // clear the memory
        state.memory[0] = dv.size();    // store the stack size in 0
        for (int i = 1; i <= state.memory[0]; i++) {
                state.memory[i] = dv.at(i - 1);
            }
        }
    }
    
void funcRestore(DoubleVector& dv, State& state) {
    // todo, trap non int in memory[0]
    for (int i = (int) state.memory[0]; i > 0; i--) {
        dv.push(state.memory[i]);
    }
    
}
