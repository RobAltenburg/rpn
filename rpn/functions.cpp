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
uint8_t funcAdd(VectorWrapper &stack, State &state) {
    printDetails(state, "x = x + y"); 
    stack.push_back(stack.pop() + stack.pop());
    return 0;
}

uint8_t funcSum(VectorWrapper &stack, State &state) {
    printDetails(state, "Sum the entire stack");
    double sum = 0;
    int64_t size = (int64_t) stack.size();
    for (int64_t i = size; i > 0; --i) {
        sum += stack.pop();
    }
    stack.push_back(sum);
    return 0;
}

uint8_t funcSubtract(VectorWrapper &stack, State &state) {
    printDetails(state, "x = y - x"); 
    double y = stack.pop();
    stack.push_back(stack.pop() - y);
    return 0;
}
uint8_t funcMultiply(VectorWrapper &stack, State &state) {
    printDetails(state, "x = x * y");
    stack.push_back(stack.pop() * stack.pop());
    return 0;
}
uint8_t funcDivide(VectorWrapper &stack, State &state) {
    printDetails(state, "x = x / y");
    double y = stack.pop();
    stack.push_back(stack.pop() / y);
    return 0;
}
uint8_t funcPower(VectorWrapper &stack, State &state) {
    printDetails(state, "x = x ^ y");
    double y = stack.pop();
    stack.push_back(pow(stack.pop(), y));
    return 0;
}

uint8_t funcRoot(VectorWrapper &stack, State &state) {
    printDetails(state, "x = x ^ (1/y)");
    double y = 1 / stack.pop();
    stack.push_back(pow(stack.pop(), y));
    return 0;
}

uint8_t funcReciprocal(VectorWrapper &stack, State &state) {
    printDetails(state, "x = 1/x");
    stack.push_back(1.0 / stack.pop());
    return 0;
}
uint8_t funcChs(VectorWrapper &stack, State &state){
    printDetails(state, "x = x * -1");
    stack.push_back(stack.pop() * -1.0);
    return 0;
}

uint8_t funcModulo(VectorWrapper &stack, State &state) {
    printDetails(state, "x = x % y");
    double y = stack.pop();
    stack.push_back(std::fmod(stack.pop(), y));
    return 0;
}

uint8_t funcLog10(VectorWrapper &stack, State &state) {
    printDetails(state, "x = log10(x)");
    stack.push_back(std::log10(stack.pop()));
    return 0;
}

uint8_t funcLog(VectorWrapper &stack, State &state) {
    printDetails(state, "x = log(x)");
    stack.push_back(std::log(stack.pop()));
    return 0;
}

uint8_t func10toX(VectorWrapper &stack, State &state) {
    printDetails(state, "x = 10^x");
    stack.push_back(pow(10,stack.pop()));
    return 0;
}

uint8_t funcEtoX(VectorWrapper &stack, State &state) {
    printDetails(state, "x = e^x");
    stack.push_back(pow(M_E,stack.pop()));
    return 0;
}

// trig
uint8_t funcSin(VectorWrapper &stack, State &state){
    printDetails(state, "x = sin(x)");
    stack.push_back(std::sin(stack.pop() * drgConversion(state)));
    return 0;
}

uint8_t funcArcSin(VectorWrapper &stack, State &state){
    printDetails(state, "x = asin(x)");
    stack.push_back(std::asin(stack.pop()) / drgConversion(state));
    return 0;
}

uint8_t funcCos(VectorWrapper &stack, State &state){
    printDetails(state, "x = cos(x)");
    double result = std::cos(stack.pop() * drgConversion(state));
    double epsilon = std::numeric_limits<double>::epsilon();
    if (std::abs(result) < epsilon) {
        stack.push_back(0);
    } else {
        stack.push_back(result);
    }
    
    return 0;
}

uint8_t funcArcCos(VectorWrapper &stack, State &state){
    printDetails(state, "x = acos(x)");
    stack.push_back(std::acos(stack.pop()) / drgConversion(state));
    return 0;
}

uint8_t funcTan(VectorWrapper &stack, State &state){
    printDetails(state, "x = tan(x)");
    double value = stack.pop() * drgConversion(state);
    if (value == M_PI / 2) {    // catch tan(90)
        stack.push_back(0);
        return ERROR_INFINITY;
    } else {
        stack.push_back(std::tan(value)); //todo: catch error for tan(90)
        return 0;
    }
}

uint8_t funcArcTan(VectorWrapper &stack, State &state){
    printDetails(state, "x = atan(x)");
    stack.push_back(std::atan(stack.pop()) / drgConversion(state));
    return 0;
}

uint8_t funcArcTan2(VectorWrapper &stack, State &state){
    printDetails(state, "x = atan2(x,y)");
    double y = stack.pop();
    stack.push_back(std::atan2(stack.pop(), y) / drgConversion(state));
    return 0;
}


// stack
uint8_t funcPop(VectorWrapper &stack, State &state){
    stack.pop();
    return 0;
}

uint8_t funcSwap(VectorWrapper &stack, State &state){
    printDetails(state, "swap x and y");
    double x = stack.pop();
    double y = stack.pop();
    stack.push_back(x);
    stack.push_back(y);
    return 0;
}

// constants
uint8_t funcPi(VectorWrapper &stack, State &state){
    stack.push_back(M_PI);
    return 0;
}

uint8_t funcE(VectorWrapper &stack, State &state){
    stack.push_back(M_E);
    return 0;
}

// Store a value to memory
uint8_t funcStore(VectorWrapper &stack, State &state){
    double slot = stack.pop();
    double value = stack.pop();
    
    if (isInteger(slot) && slot <= (MEMORY_SIZE - 1)) {
        state.memory[(int) slot] = value;
    } else {
        std::cout << "Memory must be an integer from 0 through 9" << std::endl;
    }
    stack.push_back(value);
    return 0;
}

// Recall a value to memory
uint8_t funcRecall(VectorWrapper &stack, State &state){
    double slot = stack.pop();
    
    if (isInteger(slot) && slot <= (MEMORY_SIZE - 1)) {
        stack.push_back(state.memory[(int) slot]);
    } else {
        std::cout << "Memory must be an integer from 0 through 9" << std::endl;
    }
    return 0;
}

 
// copy x to the clipboard
uint8_t funcCopy(VectorWrapper &stack, State &state) {
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
        std::cerr << "Failed to open pipe to pbcopy" << std::endl;
        return ERROR_COPY;
    }

    // Write the text to the pipe
    fwrite(text.c_str(), sizeof(char), text.size(), pipe);

    // Close the pipe
    pclose(pipe);
    return 0;
}

