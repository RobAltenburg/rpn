#include "rpn.h"
#include "operators.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <cstdlib>

// Constructor
RPNCalculator::RPNCalculator() 
    : angleMode_(AngleMode::RADIANS), scale_(15) {
}

// ============================================================================
// STACK OPERATIONS
// ============================================================================
void RPNCalculator::pushStack(double value) {
    stack_.push(value);
}

double RPNCalculator::popStack() {
    if (stack_.empty()) {
        return 0.0;
    }
    double value = stack_.top();
    stack_.pop();
    return value;
}

double RPNCalculator::peekStack() const {
    if (stack_.empty()) {
        return 0.0;
    }
    return stack_.top();
}

bool RPNCalculator::isStackEmpty() const {
    return stack_.empty();
}

size_t RPNCalculator::stackSize() const {
    return stack_.size();
}

void RPNCalculator::clearStack() {
    while (!stack_.empty()) {
        stack_.pop();
    }
}

void RPNCalculator::printStack() const {
    if (stack_.empty()) {
        std::cout << "0" << std::endl;
        return;
    }
    
    std::stack<double> temp = stack_;
    std::stack<double> reverse;
    
    while (!temp.empty()) {
        reverse.push(temp.top());
        temp.pop();
    }
    
    int level = 0;
    while (!reverse.empty()) {
        std::cout << level++ << ": " << std::setprecision(scale_) << reverse.top() << std::endl;
        reverse.pop();
    }
}

void RPNCalculator::removeTrailingZeros() {
    if (stack_.empty()) return;
    
    std::vector<double> temp;
    // Pop all elements into temp vector
    while (!stack_.empty()) {
        temp.push_back(stack_.top());
        stack_.pop();
    }
    
    // Find first non-zero from the end (which is bottom of stack)
    size_t firstNonZero = 0;
    for (size_t i = temp.size(); i > 0; --i) {
        if (temp[i-1] != 0.0) {
            firstNonZero = i;
            break;
        }
    }
    
    // Push back non-zero elements (in reverse order to maintain stack order)
    // If firstNonZero is 0, all elements were zero, so push nothing (empty stack)
    for (size_t i = firstNonZero; i > 0; --i) {
        stack_.push(temp[i-1]);
    }
}

// ============================================================================
// ANGLE MODE OPERATIONS
// ============================================================================
void RPNCalculator::setAngleMode(const std::string& mode) {
    if (mode == "degrees") {
        angleMode_ = AngleMode::DEGREES;
    } else if (mode == "radians") {
        angleMode_ = AngleMode::RADIANS;
    } else if (mode == "gradians") {
        angleMode_ = AngleMode::GRADIANS;
    }
}

int RPNCalculator::getScale() const {
    return scale_;
}

double RPNCalculator::toRadians(double angle) const {
    if (angleMode_ == AngleMode::DEGREES) {
        return angle * M_PI / 180.0;
    } else if (angleMode_ == AngleMode::GRADIANS) {
        return angle * M_PI / 200.0;
    }
    return angle;  // already in radians
}

double RPNCalculator::fromRadians(double angle) const {
    if (angleMode_ == AngleMode::DEGREES) {
        return angle * 180.0 / M_PI;
    } else if (angleMode_ == AngleMode::GRADIANS) {
        return angle * 200.0 / M_PI;
    }
    return angle;  // keep in radians
}

// ============================================================================
// MEMORY OPERATIONS
// ============================================================================
void RPNCalculator::storeMemory(int location, double value) {
    memory_[location] = value;
}

double RPNCalculator::recallMemory(int location) const {
    auto it = memory_.find(location);
    if (it != memory_.end()) {
        return it->second;
    }
    return 0.0;
}

// ============================================================================
// OUTPUT OPERATIONS
// ============================================================================
void RPNCalculator::print(double value) const {
    std::cout << std::setprecision(scale_) << value << std::endl;
}

void RPNCalculator::printError(const std::string& message) const {
    std::cerr << message << std::endl;
}

// ============================================================================
// NUMBER VALIDATION
// ============================================================================
bool RPNCalculator::isNumber(const std::string& token) const {
    if (token.empty()) return false;
    
    size_t start = 0;
    if (token[0] == '-' || token[0] == '+') {
        if (token.length() == 1) return false;
        start = 1;
    }
    
    bool hasDecimal = false;
    bool hasExponent = false;
    
    for (size_t i = start; i < token.length(); i++) {
        if (token[i] == '.') {
            if (hasDecimal || hasExponent) return false;
            hasDecimal = true;
        } else if (token[i] == 'e' || token[i] == 'E') {
            if (hasExponent) return false;
            if (i == start) return false;
            hasExponent = true;
            if (i + 1 < token.length() && (token[i + 1] == '+' || token[i + 1] == '-')) {
                i++;
                if (i + 1 >= token.length()) return false;
            }
        } else if (!isdigit(token[i])) {
            return false;
        }
    }
    return true;
}

// ============================================================================
// OPERATOR EXTRACTION
// ============================================================================
std::string RPNCalculator::extractOperator(const std::string& token, size_t& opStart) const {
    // Get all registered operators from the registry
    OperatorRegistry& registry = OperatorRegistry::instance();
    std::vector<std::string> ops = registry.getAllNames();
    
    // Add special commands that aren't in the operator registry
    ops.push_back("sto");
    ops.push_back("rcl");
    
    // Sort by length (longest first) to match longest operator first
    std::sort(ops.begin(), ops.end(), [](const std::string& a, const std::string& b) {
        return a.length() > b.length();
    });
    
    for (const auto& op : ops) {
        if (token.length() >= op.length()) {
            size_t pos = token.length() - op.length();
            if (token.substr(pos) == op) {
                opStart = pos;
                return op;
            }
        }
    }
    return "";
}

// ============================================================================
// TOKEN PROCESSING
// ============================================================================
void RPNCalculator::processToken(const std::string& token) {
    if (token.empty()) return;
    
    OperatorRegistry& registry = OperatorRegistry::instance();
    
    // Handle sto command
    if (token == "sto") {
        if (stack_.size() < 2) {
            printError("Error: Need location and value on stack");
            return;
        }
        double locDouble = stack_.top();
        stack_.pop();
        if (locDouble != std::floor(locDouble)) {
            stack_.push(locDouble);
            printError("Error: Memory location must be an integer");
            return;
        }
        int location = static_cast<int>(locDouble);
        double value = stack_.top();
        memory_[location] = value;
        return;
    }
    
    // Handle rcl command
    if (token == "rcl") {
        if (stack_.empty()) {
            printError("Error: Need location on stack");
            return;
        }
        int location = static_cast<int>(stack_.top());
        stack_.pop();
        double value = recallMemory(location);
        stack_.push(value);
        print(value);
        return;
    }
    
    // Handle scale command
    if (token == "scale") {
        if (stack_.empty()) {
            std::cout << "Current scale: " << scale_ << std::endl;
        } else {
            int newScale = static_cast<int>(stack_.top());
            stack_.pop();
            if (newScale >= 0 && newScale <= 15) {
                scale_ = newScale;
                std::cout << "Scale set to " << scale_ << std::endl;
            } else {
                std::cout << "Error: Scale must be between 0 and 15" << std::endl;
                stack_.push(newScale);
            }
        }
        return;
    }
    
    // Check if token ends with an operator (e.g., "1+", "5*", "45tan")
    if (token.length() > 1) {
        size_t opStart;
        std::string op = extractOperator(token, opStart);
        
        if (!op.empty() && opStart > 0) {
            std::string numPart = token.substr(0, opStart);
            if (isNumber(numPart)) {
                double num = std::stod(numPart);
                stack_.push(num);
                std::cout << std::setprecision(scale_) << num << std::endl;
                
                // Execute operator via registry
                const Operator* opObj = registry.getOperator(op);
                if (opObj) {
                    opObj->execute(*this);
                } else if (op == "sto" || op == "rcl") {
                    processToken(op);  // Handle special commands
                }
                return;
            }
        }
    }
    
    // Check if it's a number
    if (isNumber(token)) {
        double num = std::stod(token);
        stack_.push(num);
        std::cout << std::setprecision(scale_) << num << std::endl;
        return;
    }
    
    // Check if it's a registered operator
    const Operator* op = registry.getOperator(token);
    if (op) {
        op->execute(*this);
        return;
    }
    
    // Unknown token
    printError("Error: Invalid input '" + token + "'");
}

void RPNCalculator::processStatement(const std::string& statement) {
    std::istringstream iss(statement);
    std::string token;
    
    while (iss >> token) {
        processToken(token);
    }
}

void RPNCalculator::processLine(const std::string& line) {
    // Handle empty line (Enter pressed) - duplicate top of stack or print 0
    if (line.empty() || line.find_first_not_of(" \t") == std::string::npos) {
        if (stack_.empty()) {
            std::cout << "0" << std::endl;
        } else {
            double top = stack_.top();
            stack_.push(top);
            std::cout << std::setprecision(scale_) << top << std::endl;
        }
        removeTrailingZeros();
        return;
    }
    
    // Split by semicolons to handle multi-statement lines
    std::string current;
    for (char c : line) {
        if (c == ';') {
            if (!current.empty()) {
                processStatement(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }
    if (!current.empty()) {
        processStatement(current);
    }
    removeTrailingZeros();
}

// ============================================================================
// CONFIGURATION
// ============================================================================
void RPNCalculator::loadConfig() {
    const char* home = getenv("HOME");
    if (!home) return;
    
    std::string configPath = std::string(home) + "/.rpn";
    std::ifstream configFile(configPath);
    if (!configFile.is_open()) return;
    
    std::string line;
    while (std::getline(configFile, line)) {
        if (line.empty() || line[0] == '#') continue;
        
        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;
        
        if (cmd == "deg") {
            angleMode_ = AngleMode::DEGREES;
        } else if (cmd == "rad") {
            angleMode_ = AngleMode::RADIANS;
        } else if (cmd == "grd") {
            angleMode_ = AngleMode::GRADIANS;
        } else if (cmd == "scale") {
            int s;
            if (iss >> s && s >= 0 && s <= 15) {
                scale_ = s;
            }
        } else if (cmd == "mem") {
            int loc;
            double val;
            if (iss >> loc >> val) {
                memory_[loc] = val;
            }
        }
    }
    configFile.close();
}

// ============================================================================
// MAIN RUN LOOP
// ============================================================================
void RPNCalculator::run() {
    loadConfig();
    
    std::cout << "RPN Calculator (type 'q' to quit, 'p' to print stack, 'c' to clear)" << std::endl;
    std::cout << "Operators: + - * / % ^ ! sqrt sin cos tan atan atan2 ln log exp abs neg inv gamma" << std::endl;
    std::cout << "Stack commands: p(rint), c(clear), d(uplicate), r(everse top 2), copy, pop, sto, rcl" << std::endl;
    std::cout << "Modes: deg (degrees), rad (radians), grd (gradians)" << std::endl;
    std::cout << "Settings: scale (set precision from stack, or show current if stack empty)" << std::endl;
    
    std::string line;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break;
        
        if (line == "q" || line == "quit" || line == "exit") break;
        
        processLine(line);
    }
}
