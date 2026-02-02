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
#include <locale>
#include <clocale>

// Constructor
RPNCalculator::RPNCalculator()
    : angleMode_(AngleMode::RADIANS), scale_(15), recordingSlot_(-1), isPlayingMacro_(false),
      decimalSeparator_('.'), thousandsSeparator_(','), localeFormatting_(true) {
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
        std::cout << level++ << ": " << formatNumber(reverse.top()) << std::endl;
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

void RPNCalculator::setScale(int s) {
    if (s >= 0 && s <= 15) {
        scale_ = s;
    }
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
    std::cout << formatNumber(value) << std::endl;
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
            if (hasExponent) return false;
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
        } else {
            return false;
        }
    }

    // Final validation: if thousands separators were used, last group must be 3 digits
    // (unless a decimal point was encountered)
    if (hasThousandsSep && !hasDecimal && !hasExponent && digitsSinceThousands != 3) {
        return false;
    }

    return true;
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
    
    // Handle start recording: "[" with optional slot number on stack (default 0)
    if (token == "[") {
        if (isRecording()) {
            printError("Error: Already recording macro " + std::to_string(recordingSlot_));
            return;
        }
        int slot = 0;
        if (!stack_.empty()) {
            double slotDouble = stack_.top();
            if (slotDouble != std::floor(slotDouble)) {
                printError("Error: Macro slot must be an integer");
                return;
            }
            stack_.pop();
            slot = static_cast<int>(slotDouble);
        }
        recordingSlot_ = slot;
        recordingBuffer_.clear();
        std::cout << "Recording macro " << recordingSlot_ << "..." << std::endl;
        return;
    }
    
    // Handle stop recording: "]"
    if (token == "]") {
        if (!isRecording()) {
            printError("Error: Not recording");
            return;
        }
        macros_[recordingSlot_] = recordingBuffer_;
        std::cout << "Recorded macro " << recordingSlot_ 
                  << " (" << recordingBuffer_.size() << " commands)" << std::endl;
        recordingSlot_ = -1;
        recordingBuffer_.clear();
        return;
    }
    
    // Handle macro playback: "@" with optional slot number on stack (default 0)
    if (token == "@") {
        if (isPlayingMacro_) {
            printError("Error: Nested macro playback not supported");
            return;
        }
        int slot = 0;
        if (!stack_.empty()) {
            double slotDouble = stack_.top();
            if (slotDouble != std::floor(slotDouble)) {
                printError("Error: Macro slot must be an integer");
                return;
            }
            stack_.pop();
            slot = static_cast<int>(slotDouble);
        }
        auto it = macros_.find(slot);
        if (it == macros_.end()) {
            printError("Error: No macro in slot " + std::to_string(slot));
            return;
        }
        // Play back the macro
        isPlayingMacro_ = true;
        for (const auto& t : it->second) {
            processToken(t);
        }
        isPlayingMacro_ = false;
        return;
    }
    
    // If recording, store token (but don't store [ or ])
    if (isRecording()) {
        recordingBuffer_.push_back(token);
    }
    
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
        double locDouble = stack_.top();
        if (locDouble != std::floor(locDouble)) {
            printError("Error: Memory location must be an integer");
            return;
        }
        stack_.pop();
        int location = static_cast<int>(locDouble);
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

    // Handle fmt command - toggle locale formatting
    if (token == "fmt") {
        localeFormatting_ = !localeFormatting_;
        std::cout << "Locale formatting " << (localeFormatting_ ? "on" : "off") << std::endl;
        return;
    }
    
    // Check if token ends with an operator (e.g., "1+", "5*", "45tan")
    if (token.length() > 1) {
        size_t opStart;
        std::string op = extractOperator(token, opStart);
        
        if (!op.empty() && opStart > 0) {
            std::string numPart = token.substr(0, opStart);
            if (isNumber(numPart)) {
                double num = std::stod(normalizeNumber(numPart));
                stack_.push(num);
                std::cout << formatNumber(num) << std::endl;
                
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
        double num = std::stod(normalizeNumber(token));
        stack_.push(num);
        std::cout << formatNumber(num) << std::endl;
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
            std::cout << formatNumber(top) << std::endl;
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
        } else if (cmd == "fmt") {
            std::string value;
            if (iss >> value) {
                if (value == "off" || value == "0" || value == "false") {
                    localeFormatting_ = false;
                } else if (value == "on" || value == "1" || value == "true") {
                    localeFormatting_ = true;
                }
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
    
    std::cout << "RPN Calculator (type 'help' or '?' for commands, 'q' to quit)" << std::endl;
    
    std::string line;
    while (true) {
        // Show recording indicator in prompt
        if (isRecording()) {
            std::cout << "rec:" << recordingSlot_ << "> ";
        } else {
            std::cout << "> ";
        }
        
        if (!std::getline(std::cin, line)) break;
        
        if (line == "q" || line == "quit" || line == "exit") {
            // Discard any in-progress recording
            if (isRecording()) {
                std::cout << "Recording discarded" << std::endl;
                recordingSlot_ = -1;
                recordingBuffer_.clear();
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
