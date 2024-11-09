//
//  functions.cpp
//  rpn
//
//  Created by Robert Altenburg on 11/8/24.
//

#include "functions.hpp"

void printDetails(State &state, const std::string& message) {
    if (state.verbose) {
        std::cout << message << std::endl;
    }
}

// conversions
double drgConversion (State &state) {
    if (state.drg == 0) {           // convert degrees to rad
        return M_PI / 180.0;
    } else if (state.drg == 1) {    // leave it rad
        return 1;
    } else  {                       // convert gradians to rad
        return 0.01570796;
    }
}

// tests
bool isInteger(double number) {
    return number == static_cast<int>(number);
}

// basic math
void funcAdd(VectorWrapper &stack, State &state) {
    printDetails(state, "x = x + y"); 
    stack.push_back(stack.pop() + stack.pop());
}

void funcSum(VectorWrapper &stack, State &state) {
    printDetails(state, "Sum the entire stack");
    double sum = 0;
    int64_t size = (int64_t) stack.size();
    for (int64_t i = size; i > 0; --i) {
        sum += stack.pop();
    }
    stack.push_back(sum);
}

void funcSubtract(VectorWrapper &stack, State &state) {
    printDetails(state, "x = y - x"); 
    double y = stack.pop();
    stack.push_back(stack.pop() - y);
    
}
void funcMultiply(VectorWrapper &stack, State &state) {
    printDetails(state, "x = x * y");
    stack.push_back(stack.pop() * stack.pop());
    
}
void funcDivide(VectorWrapper &stack, State &state) {
    printDetails(state, "x = x / y");
    double y = stack.pop();
    if (y == 0) {
        stack.push_back(0);
        throw(ERROR_NAN);
    } else {
        stack.push_back(stack.pop() / y);
    }
}

void funcPower(VectorWrapper &stack, State &state) {
    printDetails(state, "x = x ^ y");
    double y = stack.pop();
    stack.push_back(pow(stack.pop(), y));
}

void funcRoot(VectorWrapper &stack, State &state) {
    printDetails(state, "x = x ^ (1/y)");
    double denominator = stack.pop();
    if (denominator == 0){
        stack.push_back(0);
        throw(ERROR_NAN);
    } else {
        double y = 1 / denominator;
        stack.push_back(pow(stack.pop(), y));
    }
}

void funcReciprocal(VectorWrapper &stack, State &state) {
    printDetails(state, "x = 1/x");
    double denominator = stack.pop();
    if (denominator == 0){
        stack.push_back(0);
        throw(ERROR_NAN);
    } else {
        stack.push_back(1.0 / stack.pop());
    }
}

void funcChs(VectorWrapper &stack, State &state){
    printDetails(state, "x = x * -1");
    stack.push_back(stack.pop() * -1.0);
}

void funcModulo(VectorWrapper &stack, State &state) {
    printDetails(state, "x = x % y");
    double y = stack.pop();
    if (y == 0){
        stack.push_back(0);
        throw(ERROR_NAN);
    } else {
        stack.push_back(std::fmod(stack.pop(), y));
    }
}

void funcLog10(VectorWrapper &stack, State &state) {
    printDetails(state, "x = log10(x)");
    stack.push_back(std::log10(stack.pop()));
}

void funcLog(VectorWrapper &stack, State &state) {
    printDetails(state, "x = log(x)");
    stack.push_back(std::log(stack.pop()));
}

void func10toX(VectorWrapper &stack, State &state) {
    printDetails(state, "x = 10^x");
    stack.push_back(pow(10,stack.pop()));
}

void funcEtoX(VectorWrapper &stack, State &state) {
    printDetails(state, "x = e^x");
    stack.push_back(pow(M_E,stack.pop()));
}

// trig
void funcSin(VectorWrapper &stack, State &state){
    printDetails(state, "x = sin(x)");
    stack.push_back(std::sin(stack.pop() * drgConversion(state)));
}

void funcArcSin(VectorWrapper &stack, State &state){
    printDetails(state, "x = asin(x)");
    stack.push_back(std::asin(stack.pop()) / drgConversion(state));
}

void funcCos(VectorWrapper &stack, State &state){
    printDetails(state, "x = cos(x)");
    double result = std::cos(stack.pop() * drgConversion(state));
    double epsilon = std::numeric_limits<double>::epsilon();
    if (std::abs(result) < epsilon) {
        stack.push_back(0);
    } else {
        stack.push_back(result);
    }
}

void funcArcCos(VectorWrapper &stack, State &state){
    printDetails(state, "x = acos(x)");
    stack.push_back(std::acos(stack.pop()) / drgConversion(state));
}

void funcTan(VectorWrapper &stack, State &state){
    printDetails(state, "x = tan(x)");
    double value = stack.pop() * drgConversion(state);
    if (value == M_PI / 2) {    // catch tan(90)
        stack.push_back(value);
        throw (ERROR_NAN);
    } else {
        stack.push_back(std::tan(value)); //todo: catch error for tan(90)
    }
}

void funcArcTan(VectorWrapper &stack, State &state){
    printDetails(state, "x = atan(x)");
    stack.push_back(std::atan(stack.pop()) / drgConversion(state));
}

void funcArcTan2(VectorWrapper &stack, State &state){
    printDetails(state, "x = atan2(x,y)");
    double y = stack.pop();
    stack.push_back(std::atan2(stack.pop(), y) / drgConversion(state));
}

void funcDMStoDeg(VectorWrapper &stack, State &state){
    printDetails(state, "z:deg, y:min, x:sec -> x:deg");
    double degrees = stack.pop() / 3600.0;
    degrees += stack.pop() / 60.0;
    degrees += stack.pop();
    stack.push_back(degrees);
}

void funcDegtoDMS(VectorWrapper &stack, State &state){
    printDetails(state, "x:deg -> z:deg, y:min, x:sec");
    double value = stack.pop();
    double degrees = std::floor(value);
    double fractionalPart = value - degrees;
    double minutesDecimal = fractionalPart * 60;
    double minutes = std::floor(minutesDecimal);
    double seconds = (minutesDecimal - minutes) * 60;
    stack.push_back(std::floor(degrees)); // push degree
    stack.push_back(std::floor(minutes)); // push min
    stack.push_back(std::floor(seconds)); // push sec
}

void funcSin(VectorWrapper &stack, State &state);

// stack
void funcPop(VectorWrapper &stack, State &state){
    stack.pop();
}

void funcSwap(VectorWrapper &stack, State &state){
    printDetails(state, "swap x and y");
    double x = stack.pop();
    double y = stack.pop();
    stack.push_back(x);
    stack.push_back(y);
}

// constants
void funcPi(VectorWrapper &stack, State &state){
    stack.push_back(M_PI);
}

void funcE(VectorWrapper &stack, State &state){
    stack.push_back(M_E);
}

// Store a value to memory
void funcStore(VectorWrapper &stack, State &state){
    double slot = stack.pop();
    double value = stack.pop();
    
    if (isInteger(slot) && slot <= (MEMORY_SIZE - 1)) {
        state.memory[(int) slot] = value;
    } else {
        throw(MEMORY_SIZE);
    }
    stack.push_back(value);
}

// Recall a value to memory
void funcRecall(VectorWrapper &stack, State &state){
    double slot = stack.pop();
    
    if (isInteger(slot) && slot <= (MEMORY_SIZE - 1)) {
        stack.push_back(state.memory[(int) slot]);
    } else {
        throw(MEMORY_SIZE);
    }
}

 
// copy x to the clipboard
void funcCopy(VectorWrapper &stack, State &state) {
    printDetails(state, "Copy x to the clipboard");
    double value = stack.pop();
    stack.push_back(value); // put it back
    
    // Convert the double x to a string
    std::ostringstream oss;
    oss << value;
    std::string text = oss.str();
    
    // Open a pipe to the pbcopy command
    FILE* pipe = popen("pbcopy", "w");
    if (pipe == nullptr) {
        throw(ERROR_COPY);
    } else {
        // Write the text to the pipe
        fwrite(text.c_str(), sizeof(char), text.size(), pipe);
        
        // Close the pipe
        pclose(pipe);
    }
}

