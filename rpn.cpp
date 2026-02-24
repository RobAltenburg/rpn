// Copyright (C) 2026  Rob Altenburg <rca@qrpc.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

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
#include <stdexcept>
#include <locale>
#include <clocale>
#include <readline/readline.h>
#include <readline/history.h>

// Constructor
RPNCalculator::RPNCalculator()
    : angleMode_(AngleMode::RADIANS), scale_(15), callDepth_(0),
      lastX_(0.0), stackLiftEnabled_(true),
      recordingSlot_(-1), recordingName_(""),
      isPlayingMacro_(false), definingOp_(""),
      decimalSeparator_('.'), thousandsSeparator_(','), localeFormatting_(true),
      outputPrefix_("\t→ "), autobindXYZ_(true) {
    detectLocaleSeparators();
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
        std::cout << outputPrefix_ << "0" << std::endl;
        return;
    }

    std::stack<double> temp = stack_;
    std::stack<double> reverse;

    while (!temp.empty()) {
        reverse.push(temp.top());
        temp.pop();
    }

    int level = reverse.size() - 1;
    while (!reverse.empty()) {
        std::string label;
        if (autobindXYZ_ && level == 0) {
            label = "x";
        } else if (autobindXYZ_ && level == 1) {
            label = "y";
        } else if (autobindXYZ_ && level == 2) {
            label = "z";
        } else if (autobindXYZ_ && level == 3) {
            label = "t";
        } else {
            label = std::to_string(level);
        }
        std::cout << outputPrefix_ << label << ": " << formatNumber(reverse.top()) << std::endl;
        reverse.pop();
        level--;
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

void RPNCalculator::setScale(int s) {
    if (s >= 0 && s <= 15) {
        scale_ = s;
    }
}

void RPNCalculator::setAutobind(bool enabled) {
    autobindXYZ_ = enabled;
}

bool RPNCalculator::getAutobind() const {
    return autobindXYZ_;
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
// NAMED VARIABLE OPERATIONS
// ============================================================================
bool RPNCalculator::storeVariable(const std::string& name, double value) {
    // Check if name would shadow an operator
    OperatorRegistry& registry = OperatorRegistry::instance();
    if (registry.hasOperator(name)) {
        return false;  // Cannot shadow operator
    }
    // Also check built-in commands
    if (name == "sto" || name == "rcl" || name == "scale" || name == "fmt" ||
        name == "q" || name == "quit" || name == "exit") {
        return false;
    }
    // Prevent assignment to reserved operator-local variables (if autobind is enabled)
    if (autobindXYZ_ && (name == "x" || name == "y" || name == "z" || name == "t")) {
        return false;
    }
    namedVariables_[name] = value;
    return true;
}

bool RPNCalculator::hasVariable(const std::string& name) const {
    return namedVariables_.find(name) != namedVariables_.end();
}

double RPNCalculator::recallVariable(const std::string& name) const {
    auto it = namedVariables_.find(name);
    if (it != namedVariables_.end()) {
        return it->second;
    }
    return 0.0;
}

// ============================================================================
// TEMPORARY OPERATOR OPERATIONS
// ============================================================================
bool RPNCalculator::hasNamedMacro(const std::string& name) const {
    return namedMacros_.find(name) != namedMacros_.end();
}

const std::vector<std::string>* RPNCalculator::getNamedMacro(const std::string& name) const {
    auto it = namedMacros_.find(name);
    if (it != namedMacros_.end()) {
        return &it->second;
    }
    return nullptr;
}

void RPNCalculator::executeMacro(const std::string& name) {
    if (isPlayingMacro_) {
        printError("Error: Nested temporary operator execution not supported");
        return;
    }
    
    const auto* macro = getNamedMacro(name);
    if (!macro) {
        printError("Error: No temporary operator named '" + name + "'");
        return;
    }
    
    isPlayingMacro_ = true;
    
    // Auto-bind x, y, z, t to top 4 stack positions (same as operators)
    bool hadX = false, hadY = false, hadZ = false, hadT = false;
    double oldX = 0.0, oldY = 0.0, oldZ = 0.0, oldT = 0.0;
    
    if (autobindXYZ_) {
        // Save previous values if they exist
        hadX = hasVariable("x");
        hadY = hasVariable("y");
        hadZ = hasVariable("z");
        hadT = hasVariable("t");
        oldX = hadX ? recallVariable("x") : 0.0;
        oldY = hadY ? recallVariable("y") : 0.0;
        oldZ = hadZ ? recallVariable("z") : 0.0;
        oldT = hadT ? recallVariable("t") : 0.0;
        
        // Peek at top 4 values (x=top, y=second, z=third, t=fourth)
        size_t size = stackSize();
        if (size >= 1) {
            double x = peekStack();
            namedVariables_["x"] = x;
        }
        if (size >= 2) {
            double topVal = popStack();
            double y = peekStack();
            pushStack(topVal);
            namedVariables_["y"] = y;
        }
        if (size >= 3) {
            double xVal = popStack();
            double yVal = popStack();
            double z = peekStack();
            pushStack(yVal);
            pushStack(xVal);
            namedVariables_["z"] = z;
        }
        if (size >= 4) {
            double xVal = popStack();
            double yVal = popStack();
            double zVal = popStack();
            double t = peekStack();
            pushStack(zVal);
            pushStack(yVal);
            pushStack(xVal);
            namedVariables_["t"] = t;
        }
    }
    
            // Execute temporary operator body
    for (const auto& t : *macro) {
        processToken(t);
    }
    
    // Restore previous x, y, z, t values (or remove if they didn't exist)
    if (autobindXYZ_) {
        if (hadX) {
            namedVariables_["x"] = oldX;
        } else {
            namedVariables_.erase("x");
        }
        if (hadY) {
            namedVariables_["y"] = oldY;
        } else {
            namedVariables_.erase("y");
        }
        if (hadZ) {
            namedVariables_["z"] = oldZ;
        } else {
            namedVariables_.erase("z");
        }
        if (hadT) {
            namedVariables_["t"] = oldT;
        } else {
            namedVariables_.erase("t");
        }
    }
    
    isPlayingMacro_ = false;
}

// ============================================================================
// USER-DEFINED OPERATOR OPERATIONS
// ============================================================================
bool RPNCalculator::registerUserOperator(const std::string& name, const std::string& description,
                                          const std::vector<std::string>& tokens) {
    OperatorRegistry& registry = OperatorRegistry::instance();
    if (registry.hasOperator(name)) {
        // Allow re-registration of user-defined operators (overwrite)
        const Operator* existing = registry.getOperator(name);
        if (existing->category != OperatorCategory::USER) {
            return false;  // Cannot shadow built-in operator
        }
    }
    // Capture tokens by value in the lambda
    std::vector<std::string> capturedTokens = tokens;
    registry.registerOperator({name, OperatorType::NULLARY, OperatorCategory::USER,
        [capturedTokens](RPNCalculator& calc) {
            if (calc.callDepth_ >= 100) {
                calc.printError("Error: Maximum recursion depth exceeded");
                return;
            }
            calc.callDepth_++;
            
            // Auto-bind x, y, z, t to top 4 stack positions (non-destructive peek) if enabled
            bool hadX = false, hadY = false, hadZ = false, hadT = false;
            double oldX = 0.0, oldY = 0.0, oldZ = 0.0, oldT = 0.0;
            
            if (calc.autobindXYZ_) {
                // Save previous values if they exist
                hadX = calc.hasVariable("x");
                hadY = calc.hasVariable("y");
                hadZ = calc.hasVariable("z");
                hadT = calc.hasVariable("t");
                oldX = hadX ? calc.recallVariable("x") : 0.0;
                oldY = hadY ? calc.recallVariable("y") : 0.0;
                oldZ = hadZ ? calc.recallVariable("z") : 0.0;
                oldT = hadT ? calc.recallVariable("t") : 0.0;
                
                // Peek at top 4 values (x=top, y=second, z=third, t=fourth)
                size_t size = calc.stackSize();
                if (size >= 1) {
                    double x = calc.peekStack();
                    calc.namedVariables_["x"] = x;
                }
                if (size >= 2) {
                    double topVal = calc.popStack();
                    double y = calc.peekStack();
                    calc.pushStack(topVal);
                    calc.namedVariables_["y"] = y;
                }
                if (size >= 3) {
                    double xVal = calc.popStack();
                    double yVal = calc.popStack();
                    double z = calc.peekStack();
                    calc.pushStack(yVal);
                    calc.pushStack(xVal);
                    calc.namedVariables_["z"] = z;
                }
                if (size >= 4) {
                    double xVal = calc.popStack();
                    double yVal = calc.popStack();
                    double zVal = calc.popStack();
                    double t = calc.peekStack();
                    calc.pushStack(zVal);
                    calc.pushStack(yVal);
                    calc.pushStack(xVal);
                    calc.namedVariables_["t"] = t;
                }
            }
            
            // Execute operator body
            for (const auto& t : capturedTokens) {
                calc.processToken(t);
            }
            
            // Restore previous x, y, z, t values (or remove if they didn't exist)
            if (calc.autobindXYZ_) {
                if (hadX) {
                    calc.namedVariables_["x"] = oldX;
                } else {
                    calc.namedVariables_.erase("x");
                }
                if (hadY) {
                    calc.namedVariables_["y"] = oldY;
                } else {
                    calc.namedVariables_.erase("y");
                }
                if (hadZ) {
                    calc.namedVariables_["z"] = oldZ;
                } else {
                    calc.namedVariables_.erase("z");
                }
                if (hadT) {
                    calc.namedVariables_["t"] = oldT;
                } else {
                    calc.namedVariables_.erase("t");
                }
            }
            
            calc.callDepth_--;
        }, description});
    return true;
}

void RPNCalculator::saveUserOperator(const std::string& name, const std::string& description,
                                      const std::vector<std::string>& tokens) {
    const char* home = getenv("HOME");
    if (!home) return;
    std::string configPath = std::string(home) + "/.rpn";

    // Read existing config, remove any previous definition of this operator
    std::vector<std::string> lines;
    std::ifstream inFile(configPath);
    if (inFile.is_open()) {
        std::string line;
        while (std::getline(inFile, line)) {
            // Skip existing definition of this operator
            std::istringstream iss(line);
            std::string cmd, existingName;
            iss >> cmd >> existingName;
            if (cmd == "operator" && existingName == name) continue;
            lines.push_back(line);
        }
        inFile.close();
    }

    // Build the operator line: operator <name> <description> : <tokens...>
    std::string opLine = "operator " + name + " " + description + " :";
    for (const auto& t : tokens) {
        opLine += " " + t;
    }
    lines.push_back(opLine);

    // Write back atomically via temp file
    std::string tempPath = configPath + ".tmp";
    std::ofstream outFile(tempPath);
    if (outFile.is_open()) {
        for (const auto& l : lines) {
            outFile << l << "\n";
        }
        outFile.close();
        std::rename(tempPath.c_str(), configPath.c_str());
    }
}

void RPNCalculator::deleteUserOperator(const std::string& name) {
    const char* home = getenv("HOME");
    if (!home) return;
    std::string configPath = std::string(home) + "/.rpn";

    std::vector<std::string> lines;
    std::ifstream inFile(configPath);
    if (inFile.is_open()) {
        std::string line;
        while (std::getline(inFile, line)) {
            std::istringstream iss(line);
            std::string cmd, existingName;
            iss >> cmd >> existingName;
            if (cmd == "operator" && existingName == name) continue;
            lines.push_back(line);
        }
        inFile.close();
    }

    std::string tempPath = configPath + ".tmp";
    std::ofstream outFile(tempPath);
    if (outFile.is_open()) {
        for (const auto& l : lines) {
            outFile << l << "\n";
        }
        outFile.close();
        std::rename(tempPath.c_str(), configPath.c_str());
    }
}

// ============================================================================
// OUTPUT OPERATIONS
// ============================================================================
std::string RPNCalculator::formatNumber(double value) const {
    std::ostringstream oss;
    oss << std::setprecision(scale_) << value;
    std::string str = oss.str();

    if (!localeFormatting_) {
        return str;
    }

    // Find the decimal point and exponent positions
    size_t decimalPos = str.find('.');
    size_t expPos = str.find('e');
    if (expPos == std::string::npos) expPos = str.find('E');

    // Determine the end of the integer part
    size_t intEnd = (decimalPos != std::string::npos) ? decimalPos :
                    (expPos != std::string::npos) ? expPos : str.length();

    // Find where digits start (after optional sign)
    size_t intStart = 0;
    if (!str.empty() && (str[0] == '-' || str[0] == '+')) {
        intStart = 1;
    }

    // Build the result with thousands separators in the integer part
    std::string result;

    // Add sign if present
    if (intStart > 0) {
        result += str[0];
    }

    // Add integer part with thousands separators
    for (size_t i = intStart; i < intEnd; ++i) {
        result += str[i];
        size_t remaining = intEnd - i - 1;
        if (remaining > 0 && remaining % 3 == 0) {
            result += thousandsSeparator_;
        }
    }

    // Add decimal part with locale decimal separator
    if (decimalPos != std::string::npos) {
        result += decimalSeparator_;
        size_t fracEnd = (expPos != std::string::npos) ? expPos : str.length();
        result += str.substr(decimalPos + 1, fracEnd - decimalPos - 1);
    }

    // Add exponent part unchanged
    if (expPos != std::string::npos) {
        result += str.substr(expPos);
    }

    return result;
}

void RPNCalculator::print(double value) const {
    std::cout << outputPrefix_ << formatNumber(value) << std::endl;
}

void RPNCalculator::printStatus(const std::string& message) const {
    std::cout << message << std::endl;
}

void RPNCalculator::printError(const std::string& message) const {
    std::cerr << message << std::endl;
}

// ============================================================================
// LOCALE DETECTION
// ============================================================================
void RPNCalculator::detectLocaleSeparators() {
    // Set locale from environment
    std::setlocale(LC_NUMERIC, "");

    // Use localeconv to get the locale's numeric formatting
    struct lconv* lc = std::localeconv();

    if (lc->decimal_point && lc->decimal_point[0]) {
        decimalSeparator_ = lc->decimal_point[0];
    }

    if (lc->thousands_sep && lc->thousands_sep[0]) {
        thousandsSeparator_ = lc->thousands_sep[0];
    } else {
        // Default thousands separator based on decimal separator
        thousandsSeparator_ = (decimalSeparator_ == ',') ? '.' : ',';
    }

    // Reset to C locale for stod to work correctly
    std::setlocale(LC_NUMERIC, "C");
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
    bool hasDigit = false;
    int digitsSinceThousands = 0;
    bool hasThousandsSep = false;

    for (size_t i = start; i < token.length(); i++) {
        char c = token[i];

        if (c == decimalSeparator_) {
            if (hasDecimal || hasExponent) return false;
            hasDecimal = true;
            digitsSinceThousands = 0;  // Reset for decimal portion
        } else if (c == thousandsSeparator_ && !hasDecimal && !hasExponent) {
            // Thousands separator only valid before decimal point and exponent
            // Must have 1-3 digits before first thousands separator
            // Must have exactly 3 digits between subsequent separators
            if (hasThousandsSep && digitsSinceThousands != 3) return false;
            if (!hasThousandsSep && (digitsSinceThousands < 1 || digitsSinceThousands > 3)) return false;
            hasThousandsSep = true;
            digitsSinceThousands = 0;
        } else if (c == 'e' || c == 'E') {
            if (hasExponent || !hasDigit) return false;
            if (i == start) return false;
            // If we had thousands separators, last group must be 3 digits (unless we hit decimal)
            if (hasThousandsSep && !hasDecimal && digitsSinceThousands != 3) return false;
            hasExponent = true;
            hasThousandsSep = false;  // Reset for exponent
            if (i + 1 < token.length() && (token[i + 1] == '+' || token[i + 1] == '-')) {
                i++;
                if (i + 1 >= token.length()) return false;
            }
        } else if (isdigit(c)) {
            digitsSinceThousands++;
            hasDigit = true;
        } else {
            return false;
        }
    }

    // Final validation: if thousands separators were used, last group must be 3 digits
    // (unless a decimal point was encountered)
    if (hasThousandsSep && !hasDecimal && !hasExponent && digitsSinceThousands != 3) {
        return false;
    }

    return hasDigit;
}

// ============================================================================
// NUMBER NORMALIZATION
// ============================================================================
std::string RPNCalculator::normalizeNumber(const std::string& token) const {
    std::string result;
    result.reserve(token.length());

    for (char c : token) {
        if (c == thousandsSeparator_) {
            // Skip thousands separators
            continue;
        } else if (c == decimalSeparator_) {
            // Convert locale decimal to standard decimal
            result += '.';
        } else {
            result += c;
        }
    }

    return result;
}

// ============================================================================
// OPERATOR EXTRACTION
// ============================================================================
std::string RPNCalculator::extractOperator(const std::string& token, size_t& opStart) const {
    OperatorRegistry& registry = OperatorRegistry::instance();
    const auto& ops = registry.getNamesSortedByLengthDesc();

    // First search registered operators (already sorted by length desc)
    for (const auto& op : ops) {
        if (token.length() >= op.length()) {
            size_t pos = token.length() - op.length();
            if (token.compare(pos, op.length(), op) == 0) {
                opStart = pos;
                return op;
            }
        }
    }

    // Then check special commands not in registry
    static const char* specials[] = {"sto", "rcl", "[", "]", "@", nullptr};
    for (int i = 0; specials[i] != nullptr; ++i) {
        const std::string op = specials[i];
        if (token.length() >= op.length()) {
            size_t pos = token.length() - op.length();
            if (token.compare(pos, op.length(), op) == 0) {
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

    // 1) Meta commands (assignment, operator start/stop, playback)
    if (handleMeta(token)) return;

    // 2) If recording, capture token
    if (isRecording()) {
        if (!definingOp_.empty()) {
            // Operator definition: capture only, don't execute
            definingBuffer_.push_back(token);
            return;
        } else {
            // Temporary operator recording: capture and execute (preserve existing behavior)
            recordingBuffer_.push_back(token);
        }
    }

    // 3) Special built-ins not in OperatorRegistry (sto/rcl/scale/fmt)
    if (handleSpecial(token)) return;

    // 4) Inline numeric + operator (e.g., "5+", "45tan")
    if (handleInlineNumericOp(token)) return;

    // 5) ENTER key - HP-style stack lift and duplicate X
    if (token == "enter" || token == "ENTER") {
        if (!stack_.empty()) {
            double x = stack_.top();
            stack_.push(x);  // Duplicate X
            print(x);
        }
        stackLiftEnabled_ = true;  // Enable lift for next number
        return;
    }
    
    // 6) Plain number
    if (isNumber(token)) {
        try {
            double num = std::stod(normalizeNumber(token));
            // In this token-based system, always lift for separate number tokens
            // (HP behavior is more nuanced for interactive digit entry)
            stack_.push(num);
            print(num);
            stackLiftEnabled_ = true;  // Keep lift enabled for next operation
        } catch (const std::out_of_range&) {
            printError("Error: Number out of range '" + token + "'");
        }
        return;
    }

    // 6) Operator, temporary operator, or variable
    OperatorRegistry& registry = OperatorRegistry::instance();
    if (const Operator* op = registry.getOperator(token)) {
        op->execute(*this);
        return;
    }
    // Check for temporary operator (no @ needed anymore)
    if (hasNamedMacro(token)) {
        executeMacro(token);
        return;
    }
    // Check named variables first (takes precedence over stack references in operator context)
    if (hasVariable(token)) {
        double value = recallVariable(token);
        stack_.push(value);
        print(value);
        return;
    }
    // Check for x, y, z, t special stack references (when autobind enabled)
    if (autobindXYZ_ && (token == "x" || token == "y" || token == "z" || token == "t")) {
        size_t size = stackSize();
        if (token == "x" && size >= 1) {
            double value = peekStack();
            pushStack(value);
            print(value);
            return;
        } else if (token == "y" && size >= 2) {
            double x = popStack();
            double y = peekStack();
            pushStack(x);
            pushStack(y);
            print(y);
            return;
        } else if (token == "z" && size >= 3) {
            double x = popStack();
            double y = popStack();
            double z = peekStack();
            pushStack(y);
            pushStack(x);
            pushStack(z);
            print(z);
            return;
        } else if (token == "t" && size >= 4) {
            double x = popStack();
            double y = popStack();
            double z = popStack();
            double t = peekStack();
            pushStack(z);
            pushStack(y);
            pushStack(x);
            pushStack(t);
            print(t);
            return;
        } else {
            printError("Error: Stack position '" + token + "' not available");
            return;
        }
    }

    // 7) Unknown
    printError("Error: Invalid input '" + token + "'");
}

void RPNCalculator::processStatement(const std::string& statement) {
    // Step 1: Extract trailing quoted description after the last '}'.
    // e.g. double{d +} "double the value" -> desc extracted, stmt trimmed to double{d +}
    std::string stmt = statement;
    size_t lastClose = stmt.rfind('}');
    if (lastClose != std::string::npos) {
        size_t q = stmt.find_first_not_of(" \t", lastClose + 1);
        if (q != std::string::npos && stmt[q] == '"') {
            size_t qEnd = stmt.rfind('"');
            if (qEnd > q) {
                pendingOpDescription_ = stmt.substr(q + 1, qEnd - q - 1);
                stmt = stmt.substr(0, lastClose + 1);
            }
        }
    }

    // Step 2: Build token list.  When a '{' is present we scan at the statement
    // level so that bodies with spaces (e.g. "name{d +}") are handled correctly.
    std::vector<std::string> tokens;
    size_t openBrace = stmt.find('{');

    if (openBrace != std::string::npos) {
        // Tokenize the part before '{'
        std::string nameToken;
        std::istringstream preSS(stmt.substr(0, openBrace));
        std::string tok;
        while (preSS >> tok) {
            if (!nameToken.empty()) tokens.push_back(nameToken);
            nameToken = tok;
        }
        // nameToken is the operator name; emit "name{"
        tokens.push_back(nameToken + "{");

        size_t closeBrace = stmt.find('}', openBrace);
        if (closeBrace != std::string::npos) {
            // Tokenize the body
            std::string body = stmt.substr(openBrace + 1, closeBrace - openBrace - 1);
            std::istringstream bodySS(body);
            while (bodySS >> tok) tokens.push_back(tok);
            tokens.push_back("}");

            // Any tokens after '}' (unusual, but handle gracefully)
            std::istringstream postSS(stmt.substr(closeBrace + 1));
            while (postSS >> tok) tokens.push_back(tok);
        } else {
            // No closing brace — '{' starts an interactive definition
            std::istringstream restSS(stmt.substr(openBrace + 1));
            while (restSS >> tok) tokens.push_back(tok);
        }
    } else {
        // No braces — plain whitespace tokenization
        std::istringstream iss(stmt);
        std::string tok;
        while (iss >> tok) tokens.push_back(tok);
    }

    for (const auto& t : tokens) {
        processToken(t);
    }
}

void RPNCalculator::processLine(const std::string& line) {
    // Handle empty line (Enter pressed) - just show current X (HP-style)
    if (line.empty() || line.find_first_not_of(" \t") == std::string::npos) {
        if (stack_.empty()) {
            print(0);
        } else {
            print(stack_.top());
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
    // Try local .rpn first, then fall back to ~/.rpn
    std::string configPath = ".rpn";
    std::ifstream configFile(configPath);
    
    if (!configFile.is_open()) {
        const char* home = getenv("HOME");
        if (!home) return;
        configPath = std::string(home) + "/.rpn";
        configFile.open(configPath);
        if (!configFile.is_open()) return;
    }
    
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
        } else if (cmd == "fmt") {
            std::string value;
            if (iss >> value) {
                if (value == "off" || value == "0" || value == "false") {
                    localeFormatting_ = false;
                } else if (value == "on" || value == "1" || value == "true") {
                    localeFormatting_ = true;
                }
            }
        } else if (cmd == "autobind") {
            std::string value;
            if (iss >> value) {
                if (value == "off" || value == "0" || value == "false") {
                    autobindXYZ_ = false;
                } else if (value == "on" || value == "1" || value == "true") {
                    autobindXYZ_ = true;
                }
            }
        } else if (cmd == "var") {
            // var <name> <value>
            std::string name;
            double val;
            if (iss >> name >> val) {
                // Silently ignore if it would shadow an operator
                storeVariable(name, val);
            }
        } else if (cmd == "operator") {
            // operator <name> <description words...> : <tokens...>
            std::string name;
            if (iss >> name) {
                // Read the rest of the line to split on ':'
                std::string rest;
                std::getline(iss, rest);
                size_t colonPos = rest.find(':');
                std::string description = "User-defined";
                std::vector<std::string> tokens;
                if (colonPos != std::string::npos) {
                    // Description is before ':', tokens after
                    std::string descPart = rest.substr(0, colonPos);
                    // Trim whitespace from description
                    size_t start = descPart.find_first_not_of(" \t");
                    size_t end = descPart.find_last_not_of(" \t");
                    if (start != std::string::npos) {
                        description = descPart.substr(start, end - start + 1);
                    }
                    std::string tokenPart = rest.substr(colonPos + 1);
                    std::istringstream tiss(tokenPart);
                    std::string tok;
                    while (tiss >> tok) {
                        tokens.push_back(tok);
                    }
                } else {
                    // No colon — everything is tokens, use default description
                    std::istringstream tiss(rest);
                    std::string tok;
                    while (tiss >> tok) {
                        tokens.push_back(tok);
                    }
                }
                if (!tokens.empty()) {
                    registerUserOperator(name, description, tokens);
                }
            }
        } else if (cmd == "prefix") {
            // prefix "<string>" - Set output prefix (quoted string)
            // Read the rest of the line and extract quoted string
            std::string rest;
            std::getline(iss, rest);
            // Trim leading whitespace
            size_t start = rest.find_first_not_of(" \t");
            if (start != std::string::npos) {
                rest = rest.substr(start);
                // Check for quotes
                if (!rest.empty() && rest[0] == '"') {
                    // Find closing quote
                    size_t endQuote = rest.find('"', 1);
                    if (endQuote != std::string::npos) {
                        // Extract string between quotes, handling escape sequences
                        std::string quoted = rest.substr(1, endQuote - 1);
                        /* Process escape sequences: \t \n \\ \" */
                        std::string processed;
                        for (size_t i = 0; i < quoted.length(); ++i) {
                            if (quoted[i] == '\\' && i + 1 < quoted.length()) {
                                char next = quoted[i + 1];
                                if (next == 't') {
                                    processed += '\t';
                                    ++i;
                                } else if (next == 'n') {
                                    processed += '\n';
                                    ++i;
                                } else if (next == '\\') {
                                    processed += '\\';
                                    ++i;
                                } else if (next == '"') {
                                    processed += '"';
                                    ++i;
                                } else {
                                    processed += quoted[i];
                                }
                            } else {
                                processed += quoted[i];
                            }
                        }
                        outputPrefix_ = processed;
                    }
                } else {
                    // No quotes - use rest of line as-is for backward compatibility
                    outputPrefix_ = rest;
                }
            } else {
                outputPrefix_ = "";  // Empty prefix
            }
        } else if (cmd == "macro") {
            // macro <name|slot> <tokens...> (deprecated keyword, use name[ ] syntax instead)
            // Try to parse as int first for backwards compatibility
            std::string nameOrSlot;
            if (iss >> nameOrSlot) {
                std::vector<std::string> tokens;
                std::string token;
                while (iss >> token) {
                    tokens.push_back(token);
                }
                if (!tokens.empty()) {
                    // Check if it's a numeric slot (deprecated)
                    bool isNumeric = true;
                    for (char c : nameOrSlot) {
                        if (!isdigit(c) && c != '-') {
                            isNumeric = false;
                            break;
                        }
                    }
                    if (isNumeric) {
                        int slot = std::stoi(nameOrSlot);
                        macros_[slot] = tokens;
                    } else {
                        namedMacros_[nameOrSlot] = tokens;
                    }
                }
            }
        }
    }
    configFile.close();
}

// ============================================================================
// READLINE COMPLETION
// ============================================================================

// Initialize the completion list with all operators and commands (encapsulated in OperatorRegistry)
static void initCompletions() {
    OperatorRegistry& registry = OperatorRegistry::instance();
    registry.setBuiltinCompletions({"sto", "rcl", "scale", "fmt", "quit", "exit"});
}

// Readline completion generator - returns matches one at a time
static char* completionGenerator(const char* text, int state) {
    static size_t listIndex;
    static size_t textLen;
    static const std::vector<std::string>* compList;
    
    if (state == 0) {
        listIndex = 0;
        textLen = strlen(text);
        compList = &OperatorRegistry::instance().completions();
    }
    
    // Find the next matching completion
    while (listIndex < compList->size()) {
        const std::string& name = (*compList)[listIndex++];
        if (name.compare(0, textLen, text) == 0) {
            return strdup(name.c_str());
        }
    }
    
    return nullptr;
}

// Custom completion function for readline
static char** rpnCompletion(const char* text, int start, int end) {
    (void)start;  // Unused
    (void)end;    // Unused
    
    // Don't complete filenames, only our operators
    rl_attempted_completion_over = 1;
    
    return rl_completion_matches(text, completionGenerator);
}

// ============================================================================
// MAIN RUN LOOP
// ============================================================================
void RPNCalculator::run() {
    loadConfig();
    
    // Initialize readline completion
    initCompletions();
    rl_attempted_completion_function = rpnCompletion;
    
    // Disable default filename completion
    rl_bind_key('\t', rl_complete);
    
    std::cout << "RPN Calculator (type 'help' or '?' for commands, 'q' to quit)" << std::endl;
    
    while (true) {
        // Build prompt with recording indicator
        std::string prompt;
        if (isRecording()) {
            if (!definingOp_.empty()) {
                prompt = "def:" + definingOp_ + "> ";
            } else if (!recordingName_.empty()) {
                prompt = "rec:" + recordingName_ + "> ";
            } else {
                prompt = "rec:" + std::to_string(recordingSlot_) + "> ";
            }
        } else {
            prompt = std::to_string(stack_.size()) + "> ";
        }
        
        char* input = readline(prompt.c_str());
        
        // EOF (Ctrl-D)
        if (!input) {
            std::cout << std::endl;
            break;
        }
        
        std::string line(input);
        
        // Add non-empty lines to history
        if (!line.empty()) {
            add_history(input);
        }
        
        free(input);
        
        if (line == "q" || line == "quit" || line == "exit") {
            // Discard any in-progress recording
            if (isRecording()) {
                std::cout << "Recording discarded" << std::endl;
                recordingSlot_ = -1;
                recordingName_.clear();
                recordingBuffer_.clear();
                definingOp_.clear();
                definingBuffer_.clear();
            }
            break;
        }
        
        processLine(line);
    }
}

// ============================================================================
// NON-INTERACTIVE EVALUATION
// ============================================================================
void RPNCalculator::evaluate(const std::string& expr) {
    loadConfig();
    processLine(expr);
    // Print final result (top of stack) if not already printed
    // The result is typically already printed by the operators
}

// ============================================================================
// PROCESS TOKEN HELPERS
// ============================================================================

bool RPNCalculator::handleMeta(const std::string& token) {
    OperatorRegistry& registry = OperatorRegistry::instance();

    // Variable assignment: name=
    if (token.size() > 1 && token.back() == '=') {
        std::string varName = token.substr(0, token.size() - 1);
        if (stack_.empty()) {
            printError("Error: Need value on stack for assignment");
            return true;
        }
        double value = stack_.top();
        if (!storeVariable(varName, value)) {
            printError("Error: Cannot use '" + varName + "' as variable name (shadows operator)");
            return true;
        }
        std::cout << outputPrefix_ << varName << " = " << formatNumber(value) << std::endl;
        return true;
    }

    // Temporary operator start: name[
    if (token.size() > 1 && token.back() == '[') {
        if (isRecording()) {
            std::string current = recordingName_.empty() ? std::to_string(recordingSlot_) : recordingName_;
            printError("Error: Already recording temporary operator '" + current + "'");
            return true;
        }
        std::string macroName = token.substr(0, token.size() - 1);
        if (registry.hasOperator(macroName)) {
            printError("Error: Cannot use '" + macroName + "' as temporary operator name (shadows operator)");
            return true;
        }
        recordingName_ = macroName;
        recordingBuffer_.clear();
        std::cout << "Defining temporary operator '" << recordingName_ << "'..." << std::endl;
        return true;
    }

    // User-defined operator start: name{
    if (token.size() > 1 && token.back() == '{') {
        if (isRecording()) {
            std::string current = !definingOp_.empty() ? definingOp_ :
                                  !recordingName_.empty() ? recordingName_ :
                                  std::to_string(recordingSlot_);
            printError("Error: Already recording '" + current + "'");
            return true;
        }
        std::string opName = token.substr(0, token.size() - 1);
        if (registry.hasOperator(opName)) {
            const Operator* existing = registry.getOperator(opName);
            if (existing->category != OperatorCategory::USER) {
                printError("Error: Cannot use '" + opName + "' as operator name (shadows built-in)");
                return true;
            }
        }
        definingOp_ = opName;
        definingBuffer_.clear();
        std::cout << "Defining operator '" << definingOp_ << "'..." << std::endl;
        return true;
    }

    // User-defined operator end: }
    if (token == "}") {
        if (definingOp_.empty()) {
            printError("Error: Not defining an operator");
            return true;
        }
        if (definingBuffer_.empty()) {
            std::string name = definingOp_;
            definingOp_.clear();
            pendingOpDescription_.clear();
            if (registry.hasOperator(name) && registry.getOperator(name)->category == OperatorCategory::USER) {
                registry.removeOperator(name);
                deleteUserOperator(name);
                std::cout << "Deleted operator '" << name << "'" << std::endl;
            } else {
                printError("Error: Operator body is empty");
            }
            return true;
        }
        std::string name = definingOp_;
        std::vector<std::string> toks = definingBuffer_;
        definingOp_.clear();
        definingBuffer_.clear();
        std::string desc = pendingOpDescription_.empty() ? "User-defined" : pendingOpDescription_;
        pendingOpDescription_.clear();
        if (registerUserOperator(name, desc, toks)) {
            saveUserOperator(name, desc, toks);
            std::cout << "Defined operator '" << name << "' (" << toks.size() << " commands, saved to ~/.rpn)" << std::endl;
        } else {
            printError("Error: Cannot define operator '" + name + "' (shadows built-in)");
        }
        return true;
    }

    // Temporary operator execution: name@ (backward compatibility - @ no longer required)
    if (token.size() > 1 && token.back() == '@') {
        std::string macroName = token.substr(0, token.size() - 1);
        executeMacro(macroName);
        return true;
    }

    // Start recording with optional numeric slot on stack: [
    if (token == "[") {
        if (isRecording()) {
            std::string current = recordingName_.empty() ? std::to_string(recordingSlot_) : recordingName_;
            printError("Error: Already recording temporary operator '" + current + "'");
            return true;
        }
        int slot = 0;
        if (!stack_.empty()) {
            double slotDouble = stack_.top();
            if (slotDouble != std::floor(slotDouble)) {
                printError("Error: Temporary operator slot must be an integer");
                return true;
            }
            stack_.pop();
            slot = static_cast<int>(slotDouble);
        }
        recordingSlot_ = slot;
        recordingBuffer_.clear();
        std::cout << "Recording temporary operator " << recordingSlot_ << " (deprecated, use name[ syntax)..." << std::endl;
        return true;
    }

    // Stop recording: ]
    if (token == "]") {
        if (!isRecording()) {
            printError("Error: Not recording");
            return true;
        }
        if (!recordingName_.empty()) {
            namedMacros_[recordingName_] = recordingBuffer_;
            std::cout << "Defined temporary operator '" << recordingName_ << "' (" << recordingBuffer_.size() << " commands)" << std::endl;
            recordingName_.clear();
        } else {
            macros_[recordingSlot_] = recordingBuffer_;
            std::cout << "Defined temporary operator " << recordingSlot_ << " (" << recordingBuffer_.size() << " commands)" << std::endl;
            recordingSlot_ = -1;
        }
        recordingBuffer_.clear();
        return true;
    }

    // Execute numeric-slot temporary operator: @
    if (token == "@") {
        if (isPlayingMacro_) {
            printError("Error: Nested temporary operator execution not supported");
            return true;
        }
        int slot = 0;
        if (!stack_.empty()) {
            double slotDouble = stack_.top();
            if (slotDouble != std::floor(slotDouble)) {
                printError("Error: Temporary operator slot must be an integer");
                return true;
            }
            stack_.pop();
            slot = static_cast<int>(slotDouble);
        }
        auto it = macros_.find(slot);
        if (it == macros_.end()) {
            printError("Error: No temporary operator in slot " + std::to_string(slot));
            return true;
        }
        isPlayingMacro_ = true;
        for (const auto& t : it->second) {
            std::cout << "  @" << slot << ": " << t << std::endl;
            processToken(t);
        }
        isPlayingMacro_ = false;
        return true;
    }

    return false; // not handled
}

bool RPNCalculator::handleSpecial(const std::string& token) {
    // sto
    if (token == "sto") {
        if (stack_.size() < 2) {
            printError("Error: Need location and value on stack");
            return true;
        }
        double locDouble = stack_.top();
        stack_.pop();
        if (locDouble != std::floor(locDouble)) {
            stack_.push(locDouble);
            printError("Error: Memory location must be an integer");
            return true;
        }
        int location = static_cast<int>(locDouble);
        double value = stack_.top();
        memory_[location] = value;
        std::cout << "(deprecated: use 'name=' instead)" << std::endl;
        return true;
    }

    // rcl
    if (token == "rcl") {
        if (stack_.empty()) {
            printError("Error: Need location on stack");
            return true;
        }
        double locDouble = stack_.top();
        if (locDouble != std::floor(locDouble)) {
            printError("Error: Memory location must be an integer");
            return true;
        }
        stack_.pop();
        int location = static_cast<int>(locDouble);
        double value = recallMemory(location);
        stack_.push(value);
        print(value);
        std::cout << "(deprecated: use variable names instead)" << std::endl;
        return true;
    }

    // scale
    if (token == "scale") {
        if (stack_.empty()) {
            std::cout << "Current scale: " << scale_ << std::endl;
        } else {
            double scaleVal = stack_.top();
            if (scaleVal != std::floor(scaleVal)) {
                printError("Error: Scale must be an integer");
                return true;
            }
            int newScale = static_cast<int>(scaleVal);
            if (newScale < 0 || newScale > 15) {
                printError("Error: Scale must be between 0 and 15");
                return true;
            }
            stack_.pop();
            scale_ = newScale;
            std::cout << "Scale set to " << scale_ << std::endl;
        }
        return true;
    }

    // fmt
    if (token == "fmt") {
        localeFormatting_ = !localeFormatting_;
        std::cout << "Locale formatting " << (localeFormatting_ ? "on" : "off") << std::endl;
        return true;
    }

    // autobind
    if (token == "autobind") {
        autobindXYZ_ = !autobindXYZ_;
        std::cout << "Auto-binding x,y,z,t " << (autobindXYZ_ ? "on" : "off") << std::endl;
        return true;
    }

    return false;
}

bool RPNCalculator::handleInlineNumericOp(const std::string& token) {
    if (token.size() <= 1) return false;

    size_t opStart;
    std::string op = extractOperator(token, opStart);
    if (!op.empty() && opStart > 0) {
        std::string numPart = token.substr(0, opStart);
        if (isNumber(numPart)) {
            double num;
            try {
                num = std::stod(normalizeNumber(numPart));
            } catch (const std::out_of_range&) {
                printError("Error: Number out of range '" + numPart + "'");
                return true;
            }
            stack_.push(num);
            print(num);

            OperatorRegistry& registry = OperatorRegistry::instance();
            const Operator* opObj = registry.getOperator(op);
            if (opObj) {
                opObj->execute(*this);
            } else if (op == "sto" || op == "rcl") {
                // Call directly to avoid double-recording during macro capture
                handleSpecial(op);
            } else if (op == "[" || op == "]" || op == "@") {
                handleMeta(op);
            }
            return true;
        }
    }
    return false;
}
