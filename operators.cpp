#include "operators.h"
#include "rpn.h"
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstdio>

// Singleton instance
OperatorRegistry& OperatorRegistry::instance() {
    static OperatorRegistry registry;
    return registry;
}

OperatorRegistry::OperatorRegistry() {
    initializeOperators();
}

void OperatorRegistry::registerOperator(const Operator& op) {
    operators_[op.name] = op;
}

bool OperatorRegistry::hasOperator(const std::string& name) const {
    return operators_.find(name) != operators_.end();
}

const Operator* OperatorRegistry::getOperator(const std::string& name) const {
    auto it = operators_.find(name);
    return it != operators_.end() ? &it->second : nullptr;
}

std::vector<std::string> OperatorRegistry::getAllNames() const {
    std::vector<std::string> names;
    for (const auto& pair : operators_) {
        names.push_back(pair.first);
    }
    return names;
}

void OperatorRegistry::initializeOperators() {
    registerArithmetic();
    registerTrigonometric();
    registerLogarithmic();
    registerStackOperations();
    registerMiscellaneous();
}

// ============================================================================
// ARITHMETIC OPERATORS
// ============================================================================
void OperatorRegistry::registerArithmetic() {
    // Addition
    registerOperator({"+", OperatorType::BINARY, [](RPNCalculator& calc) {
        double x = calc.popStack();
        double y = calc.popStack();
        double result = y + x;
        calc.pushStack(result);
        calc.print(result);
    }, "Addition"});
    
    // Subtraction
    registerOperator({"-", OperatorType::BINARY, [](RPNCalculator& calc) {
        double x = calc.popStack();
        double y = calc.popStack();
        double result = y - x;
        calc.pushStack(result);
        calc.print(result);
    }, "Subtraction"});
    
    // Multiplication
    registerOperator({"*", OperatorType::BINARY, [](RPNCalculator& calc) {
        double x = calc.popStack();
        double y = calc.popStack();
        double result = y * x;
        calc.pushStack(result);
        calc.print(result);
    }, "Multiplication"});
    
    // Division
    registerOperator({"/", OperatorType::BINARY, [](RPNCalculator& calc) {
        double x = calc.popStack();
        double y = calc.popStack();
        if (x == 0) {
            calc.printError("Error: Division by zero");
            calc.pushStack(y);
            calc.pushStack(x);
            return;
        }
        double result = y / x;
        calc.pushStack(result);
        calc.print(result);
    }, "Division"});
    
    // Modulo
    registerOperator({"%", OperatorType::BINARY, [](RPNCalculator& calc) {
        double x = calc.popStack();
        double y = calc.popStack();
        if (x == 0) {
            calc.printError("Error: Modulo by zero");
            calc.pushStack(y);
            calc.pushStack(x);
            return;
        }
        double result = std::fmod(y, x);
        calc.pushStack(result);
        calc.print(result);
    }, "Modulo"});
    
    // Power
    registerOperator({"^", OperatorType::BINARY, [](RPNCalculator& calc) {
        double x = calc.popStack();
        double y = calc.popStack();
        double result = std::pow(y, x);
        if (std::isnan(result)) {
            calc.printError("Error: Result is not a number");
            calc.pushStack(y);
            calc.pushStack(x);
            return;
        }
        if (std::isinf(result)) {
            calc.printError("Error: Result is infinity");
            calc.pushStack(y);
            calc.pushStack(x);
            return;
        }
        calc.pushStack(result);
        calc.print(result);
    }, "Power"});
}

// ============================================================================
// TRIGONOMETRIC OPERATORS
// ============================================================================
void OperatorRegistry::registerTrigonometric() {
    // Sine
    registerOperator({"sin", OperatorType::UNARY, [](RPNCalculator& calc) {
        double x = calc.popStack();
        double result = std::sin(calc.toRadians(x));
        calc.pushStack(result);
        calc.print(result);
    }, "Sine"});
    
    // Cosine
    registerOperator({"cos", OperatorType::UNARY, [](RPNCalculator& calc) {
        double x = calc.popStack();
        double result = std::cos(calc.toRadians(x));
        calc.pushStack(result);
        calc.print(result);
    }, "Cosine"});
    
    // Tangent
    registerOperator({"tan", OperatorType::UNARY, [](RPNCalculator& calc) {
        double x = calc.popStack();
        double radians = calc.toRadians(x);
        double cosVal = std::cos(radians);
        if (std::abs(cosVal) < 1e-10) {
            calc.printError("Error: Tangent undefined at this angle");
            calc.pushStack(x);
            return;
        }
        double result = std::tan(radians);
        calc.pushStack(result);
        calc.print(result);
    }, "Tangent"});
    
    // Arcsine
    registerOperator({"asin", OperatorType::UNARY, [](RPNCalculator& calc) {
        double x = calc.popStack();
        if (x < -1 || x > 1) {
            calc.printError("Error: asin argument must be in [-1, 1]");
            calc.pushStack(x);
            return;
        }
        double result = calc.fromRadians(std::asin(x));
        calc.pushStack(result);
        calc.print(result);
    }, "Arcsine"});
    
    // Arccosine
    registerOperator({"acos", OperatorType::UNARY, [](RPNCalculator& calc) {
        double x = calc.popStack();
        if (x < -1 || x > 1) {
            calc.printError("Error: acos argument must be in [-1, 1]");
            calc.pushStack(x);
            return;
        }
        double result = calc.fromRadians(std::acos(x));
        calc.pushStack(result);
        calc.print(result);
    }, "Arccosine"});
    
    // Arctangent
    registerOperator({"atan", OperatorType::UNARY, [](RPNCalculator& calc) {
        double x = calc.popStack();
        double result = calc.fromRadians(std::atan(x));
        calc.pushStack(result);
        calc.print(result);
    }, "Arctangent"});
    
    // Arctangent2
    registerOperator({"atan2", OperatorType::BINARY, [](RPNCalculator& calc) {
        double x = calc.popStack();
        double y = calc.popStack();
        double result = calc.fromRadians(std::atan2(y, x));
        calc.pushStack(result);
        calc.print(result);
    }, "Arctangent2"});
}

// ============================================================================
// LOGARITHMIC/EXPONENTIAL OPERATORS
// ============================================================================
void OperatorRegistry::registerLogarithmic() {
    // Natural logarithm
    registerOperator({"ln", OperatorType::UNARY, [](RPNCalculator& calc) {
        double x = calc.popStack();
        if (x <= 0) {
            calc.printError("Error: Logarithm of non-positive number");
            calc.pushStack(x);
            return;
        }
        double result = std::log(x);
        calc.pushStack(result);
        calc.print(result);
    }, "Natural logarithm"});
    
    // Base-10 logarithm
    registerOperator({"log", OperatorType::UNARY, [](RPNCalculator& calc) {
        double x = calc.popStack();
        if (x <= 0) {
            calc.printError("Error: Logarithm of non-positive number");
            calc.pushStack(x);
            return;
        }
        double result = std::log10(x);
        calc.pushStack(result);
        calc.print(result);
    }, "Base-10 logarithm"});
    
    // Exponential
    registerOperator({"exp", OperatorType::UNARY, [](RPNCalculator& calc) {
        double x = calc.popStack();
        double result = std::exp(x);
        if (std::isinf(result)) {
            calc.printError("Error: Result is infinity");
            calc.pushStack(x);
            return;
        }
        calc.pushStack(result);
        calc.print(result);
    }, "Exponential (e^x)"});
    
    // Base-2 logarithm
    registerOperator({"log2", OperatorType::UNARY, [](RPNCalculator& calc) {
        double x = calc.popStack();
        if (x <= 0) {
            calc.printError("Error: Logarithm of non-positive number");
            calc.pushStack(x);
            return;
        }
        double result = std::log2(x);
        calc.pushStack(result);
        calc.print(result);
    }, "Base-2 logarithm"});
    
    // Arbitrary base logarithm: y logb = log_y(x) where x is below y on stack
    registerOperator({"logb", OperatorType::BINARY, [](RPNCalculator& calc) {
        double base = calc.popStack();
        double x = calc.popStack();
        if (x <= 0 || base <= 0) {
            calc.printError("Error: Logarithm of non-positive number");
            calc.pushStack(x);
            calc.pushStack(base);
            return;
        }
        if (base == 1) {
            calc.printError("Error: Logarithm base cannot be 1");
            calc.pushStack(x);
            calc.pushStack(base);
            return;
        }
        double result = std::log(x) / std::log(base);
        calc.pushStack(result);
        calc.print(result);
    }, "Logarithm with arbitrary base (x base logb)"});
}

// ============================================================================
// STACK OPERATIONS
// ============================================================================
void OperatorRegistry::registerStackOperations() {
    // Print stack
    registerOperator({"p", OperatorType::NULLARY, [](RPNCalculator& calc) {
        calc.printStack();
    }, "Print stack"});
    
    // Clear stack
    registerOperator({"c", OperatorType::NULLARY, [](RPNCalculator& calc) {
        calc.clearStack();
        std::cout << "Stack cleared" << std::endl;
    }, "Clear stack"});
    
    // Duplicate top
    registerOperator({"d", OperatorType::NULLARY, [](RPNCalculator& calc) {
        if (calc.isStackEmpty()) {
            calc.printError("Error: Stack empty");
            return;
        }
        double val = calc.peekStack();
        calc.pushStack(val);
    }, "Duplicate top"});
    
    // Reverse top 2 / swap
    auto swapFunc = [](RPNCalculator& calc) {
        if (calc.stackSize() < 2) {
            calc.printError("Error: Need at least 2 elements");
            return;
        }
        double x = calc.popStack();
        double y = calc.popStack();
        calc.pushStack(x);
        calc.pushStack(y);
    };
    registerOperator({"r", OperatorType::NULLARY, swapFunc, "Reverse top 2"});
    registerOperator({"swap", OperatorType::NULLARY, swapFunc, "Swap top 2 (alias for r)"});
    
    // Pop
    registerOperator({"pop", OperatorType::NULLARY, [](RPNCalculator& calc) {
        if (!calc.isStackEmpty()) {
            calc.popStack();
        }
    }, "Pop top value"});

    // Copy to clipboard (cross-platform)
    registerOperator({"copy", OperatorType::NULLARY, [](RPNCalculator& calc) {
        double value = calc.peekStack();
        std::ostringstream oss;
        oss << std::setprecision(calc.getScale()) << value;
        std::string str = oss.str();

        // Try platform-specific clipboard commands
        const char* commands[] = {
#ifdef __APPLE__
            "pbcopy",
#elif defined(__linux__)
            "xclip -selection clipboard",
            "xsel --clipboard --input",
#elif defined(_WIN32)
            "clip",
#endif
            nullptr
        };
        
        bool copied = false;
        for (int i = 0; commands[i] != nullptr; ++i) {
            FILE* pipe = popen(commands[i], "w");
            if (pipe) {
                fputs(str.c_str(), pipe);
                int status = pclose(pipe);
                if (status == 0) {
                    std::cout << "Copied: " << str << std::endl;
                    copied = true;
                    break;
                }
            }
        }
        
        if (!copied) {
            calc.printError("Error: Could not copy to clipboard");
        }
    }, "Copy top to clipboard"});
    
    // Sum all stack values
    registerOperator({"sum", OperatorType::NULLARY, [](RPNCalculator& calc) {
        if (calc.isStackEmpty()) {
            calc.pushStack(0);
            calc.print(0);
            return;
        }
        double total = 0;
        while (!calc.isStackEmpty()) {
            total += calc.popStack();
        }
        calc.pushStack(total);
        calc.print(total);
    }, "Sum all stack values"});
    
    // Product of all stack values
    registerOperator({"prod", OperatorType::NULLARY, [](RPNCalculator& calc) {
        if (calc.isStackEmpty()) {
            calc.pushStack(1);
            calc.print(1);
            return;
        }
        double total = 1;
        while (!calc.isStackEmpty()) {
            total *= calc.popStack();
        }
        calc.pushStack(total);
        calc.print(total);
    }, "Product of all stack values"});
}

// ============================================================================
// MISCELLANEOUS OPERATORS
// ============================================================================
void OperatorRegistry::registerMiscellaneous() {
    // Square root
    registerOperator({"sqrt", OperatorType::UNARY, [](RPNCalculator& calc) {
        double x = calc.popStack();
        if (x < 0) {
            calc.printError("Error: Square root of negative number");
            calc.pushStack(x);
            return;
        }
        double result = std::sqrt(x);
        calc.pushStack(result);
        calc.print(result);
    }, "Square root"});
    
    // Absolute value
    registerOperator({"abs", OperatorType::UNARY, [](RPNCalculator& calc) {
        double x = calc.popStack();
        double result = std::abs(x);
        calc.pushStack(result);
        calc.print(result);
    }, "Absolute value"});
    
    // Negation
    registerOperator({"neg", OperatorType::UNARY, [](RPNCalculator& calc) {
        double x = calc.popStack();
        double result = -x;
        calc.pushStack(result);
        calc.print(result);
    }, "Negation"});
    
    // Inverse (1/x)
    registerOperator({"inv", OperatorType::UNARY, [](RPNCalculator& calc) {
        double x = calc.popStack();
        if (x == 0) {
            calc.printError("Error: Division by zero");
            calc.pushStack(x);
            return;
        }
        double result = 1.0 / x;
        calc.pushStack(result);
        calc.print(result);
    }, "Inverse (1/x)"});
    
    // Gamma function
    registerOperator({"gamma", OperatorType::UNARY, [](RPNCalculator& calc) {
        double x = calc.popStack();
        double result = std::tgamma(x);
        if (std::isnan(result)) {
            calc.printError("Error: Result is not a number");
            calc.pushStack(x);
            return;
        }
        if (std::isinf(result)) {
            calc.printError("Error: Result is infinity");
            calc.pushStack(x);
            return;
        }
        calc.pushStack(result);
        calc.print(result);
    }, "Gamma function"});
    
    // Factorial (using gamma)
    registerOperator({"!", OperatorType::UNARY, [](RPNCalculator& calc) {
        double x = calc.popStack();
        double result = std::tgamma(x + 1);
        if (std::isnan(result)) {
            calc.printError("Error: Result is not a number");
            calc.pushStack(x);
            return;
        }
        if (std::isinf(result)) {
            calc.printError("Error: Result is infinity");
            calc.pushStack(x);
            return;
        }
        calc.pushStack(result);
        calc.print(result);
    }, "Factorial"});
    
    // Floor
    registerOperator({"floor", OperatorType::UNARY, [](RPNCalculator& calc) {
        double x = calc.popStack();
        double result = std::floor(x);
        calc.pushStack(result);
        calc.print(result);
    }, "Floor (round down)"});
    
    // Ceil
    registerOperator({"ceil", OperatorType::UNARY, [](RPNCalculator& calc) {
        double x = calc.popStack();
        double result = std::ceil(x);
        calc.pushStack(result);
        calc.print(result);
    }, "Ceiling (round up)"});
    
    // Round
    registerOperator({"round", OperatorType::UNARY, [](RPNCalculator& calc) {
        double x = calc.popStack();
        double result = std::round(x);
        calc.pushStack(result);
        calc.print(result);
    }, "Round to nearest integer"});
    
    // Trunc
    registerOperator({"trunc", OperatorType::UNARY, [](RPNCalculator& calc) {
        double x = calc.popStack();
        double result = std::trunc(x);
        calc.pushStack(result);
        calc.print(result);
    }, "Truncate (round toward zero)"});
    
    // Constants
    registerOperator({"pi", OperatorType::NULLARY, [](RPNCalculator& calc) {
        calc.pushStack(M_PI);
        calc.print(M_PI);
    }, "Push π (3.14159...)"});
    
    registerOperator({"e", OperatorType::NULLARY, [](RPNCalculator& calc) {
        calc.pushStack(M_E);
        calc.print(M_E);
    }, "Push e (2.71828...)"});
    
    registerOperator({"phi", OperatorType::NULLARY, [](RPNCalculator& calc) {
        double phi = (1.0 + std::sqrt(5.0)) / 2.0;  // Golden ratio
        calc.pushStack(phi);
        calc.print(phi);
    }, "Push φ golden ratio (1.61803...)"});
    
    // Angle mode commands
    registerOperator({"deg", OperatorType::NULLARY, [](RPNCalculator& calc) {
        calc.setAngleMode("degrees");
        std::cout << "Angle mode: degrees" << std::endl;
    }, "Set degrees mode"});
    
    registerOperator({"rad", OperatorType::NULLARY, [](RPNCalculator& calc) {
        calc.setAngleMode("radians");
        std::cout << "Angle mode: radians" << std::endl;
    }, "Set radians mode"});
    
    registerOperator({"grd", OperatorType::NULLARY, [](RPNCalculator& calc) {
        calc.setAngleMode("gradians");
        std::cout << "Angle mode: gradians" << std::endl;
    }, "Set gradians mode"});
    
    // Help command
    registerOperator({"help", OperatorType::NULLARY, [](RPNCalculator&) {
        OperatorRegistry& reg = OperatorRegistry::instance();
        std::vector<std::string> names = reg.getAllNames();
        std::sort(names.begin(), names.end());
        
        std::cout << "Available operators:" << std::endl;
        for (const auto& name : names) {
            const Operator* op = reg.getOperator(name);
            if (op) {
                std::cout << "  " << name << " - " << op->description << std::endl;
            }
        }
        std::cout << "\nSpecial commands: sto, rcl, scale, fmt, q/quit/exit" << std::endl;
        std::cout << "\nMacros:" << std::endl;
        std::cout << "  x[  - Start recording macro to slot x (default 0)" << std::endl;
        std::cout << "  ]   - Stop recording" << std::endl;
        std::cout << "  x@  - Play macro from slot x" << std::endl;
        std::cout << "\nConfig: loads .rpn from current dir or ~/.rpn" << std::endl;
    }, "Show this help"});
    
    registerOperator({"?", OperatorType::NULLARY, [](RPNCalculator& calc) {
        // Alias for help
        const Operator* helpOp = OperatorRegistry::instance().getOperator("help");
        if (helpOp) helpOp->execute(calc);
    }, "Show help (alias for help)"});
}
