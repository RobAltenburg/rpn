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

#include "operators.h"
#include "rpn.h"
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstdio>
#include <algorithm>
#include <random>

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
    names_len_desc_dirty_ = true;
    completions_dirty_ = true;
}

void OperatorRegistry::removeOperator(const std::string& name) {
    operators_.erase(name);
    names_len_desc_dirty_ = true;
    completions_dirty_ = true;
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
    names.reserve(operators_.size());
    for (const auto& pair : operators_) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<std::string> OperatorRegistry::getNamesByCategory(OperatorCategory category) const {
    std::vector<std::string> names;
    for (const auto& pair : operators_) {
        if (pair.second.category == category) {
            names.push_back(pair.first);
        }
    }
    return names;
}

std::string OperatorRegistry::categoryName(OperatorCategory category) {
    switch (category) {
        case OperatorCategory::ARITHMETIC: return "Arithmetic";
        case OperatorCategory::TRIGONOMETRIC: return "Trigonometric";
        case OperatorCategory::HYPERBOLIC: return "Hyperbolic";
        case OperatorCategory::LOGARITHMIC: return "Logarithmic";
        case OperatorCategory::STACK: return "Stack";
        case OperatorCategory::CONVERSION: return "Unit Conversion";
        case OperatorCategory::MISCELLANEOUS: return "Miscellaneous";
        case OperatorCategory::USER: return "User-defined";
    }
    return "Unknown";
}

const std::vector<OperatorCategory>& OperatorRegistry::allCategories() {
    static std::vector<OperatorCategory> categories = {
        OperatorCategory::ARITHMETIC,
        OperatorCategory::TRIGONOMETRIC,
        OperatorCategory::HYPERBOLIC,
        OperatorCategory::LOGARITHMIC,
        OperatorCategory::STACK,
        OperatorCategory::CONVERSION,
        OperatorCategory::MISCELLANEOUS,
        OperatorCategory::USER
    };
    return categories;
}

const std::vector<std::string>& OperatorRegistry::getNamesSortedByLengthDesc() {
    if (names_len_desc_dirty_) {
        names_len_desc_cache_.clear();
        names_len_desc_cache_.reserve(operators_.size());
        for (const auto& kv : operators_) names_len_desc_cache_.push_back(kv.first);
        std::sort(names_len_desc_cache_.begin(), names_len_desc_cache_.end(),
                  [](const std::string& a, const std::string& b) {
                      if (a.size() != b.size()) return a.size() > b.size();
                      return a < b; // stable ordering for equal lengths
                  });
        names_len_desc_dirty_ = false;
    }
    return names_len_desc_cache_;
}

void OperatorRegistry::setBuiltinCompletions(const std::vector<std::string>& builtins) {
    builtins_ = builtins;
    completions_dirty_ = true;
}

const std::vector<std::string>& OperatorRegistry::completions() {
    if (completions_dirty_) {
        completions_cache_ = getAllNames();
        // add builtins not in registry
        completions_cache_.insert(completions_cache_.end(), builtins_.begin(), builtins_.end());
        std::sort(completions_cache_.begin(), completions_cache_.end());
        completions_cache_.erase(std::unique(completions_cache_.begin(), completions_cache_.end()), completions_cache_.end());
        completions_dirty_ = false;
    }
    return completions_cache_;
}

void OperatorRegistry::initializeOperators() {
    registerArithmetic();
    registerTrigonometric();
    registerHyperbolic();
    registerLogarithmic();
    registerStackOperations();
    registerUnitConversions();
    registerMiscellaneous();
}

void OperatorRegistry::registerUnaryOp(const std::string& name, OperatorCategory cat,
                                        UnaryFn fn, const std::string& desc) {
    registerOperator({name, OperatorType::UNARY, cat, [fn](RPNCalculator& calc) {
        double x = calc.popStack();
        calc.lastX_ = x;  // Save LASTX
        double result = fn(calc, x);
        calc.pushStack(result);
        calc.print(result);
        calc.stackLiftEnabled_ = true;  // Enable stack lift after operation
    }, desc});
}

void OperatorRegistry::registerBinaryOp(const std::string& name, OperatorCategory cat,
                                         BinaryFn fn, const std::string& desc) {
    registerOperator({name, OperatorType::BINARY, cat, [fn](RPNCalculator& calc) {
        double x = calc.popStack();
        double y = calc.popStack();
        calc.lastX_ = x;  // Save LASTX (typically save the last operand)
        double result = fn(calc, y, x);
        calc.pushStack(result);
        calc.print(result);
        calc.stackLiftEnabled_ = true;  // Enable stack lift after operation
    }, desc});
}

void OperatorRegistry::registerGuardedUnaryOp(const std::string& name, OperatorCategory cat,
                                               UnaryFn fn, const std::string& desc) {
    registerOperator({name, OperatorType::UNARY, cat, [fn](RPNCalculator& calc) {
        double x = calc.popStack();
        calc.lastX_ = x;  // Save LASTX
        double result = fn(calc, x);
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
        calc.stackLiftEnabled_ = true;  // Enable stack lift after operation
    }, desc});
}

void OperatorRegistry::registerGuardedBinaryOp(const std::string& name, OperatorCategory cat,
                                                BinaryFn fn, const std::string& desc) {
    registerOperator({name, OperatorType::BINARY, cat, [fn](RPNCalculator& calc) {
        double x = calc.popStack();
        double y = calc.popStack();
        calc.lastX_ = x;  // Save LASTX
        double result = fn(calc, y, x);
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
        calc.stackLiftEnabled_ = true;  // Enable stack lift after operation
    }, desc});
}

// ============================================================================
// ARITHMETIC OPERATORS
// ============================================================================
void OperatorRegistry::registerArithmetic() {
    registerBinaryOp("+", OperatorCategory::ARITHMETIC,
        [](RPNCalculator&, double y, double x) { return y + x; }, "Addition");
    registerBinaryOp("-", OperatorCategory::ARITHMETIC,
        [](RPNCalculator&, double y, double x) { return y - x; }, "Subtraction");
    registerBinaryOp("*", OperatorCategory::ARITHMETIC,
        [](RPNCalculator&, double y, double x) { return y * x; }, "Multiplication");
    
    // Division — custom validation for zero
    registerOperator({"/", OperatorType::BINARY, OperatorCategory::ARITHMETIC, [](RPNCalculator& calc) {
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
    
    // Modulo — custom validation for zero
    registerOperator({"%", OperatorType::BINARY, OperatorCategory::ARITHMETIC, [](RPNCalculator& calc) {
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
    
    registerGuardedBinaryOp("^", OperatorCategory::ARITHMETIC,
        [](RPNCalculator&, double y, double x) { return std::pow(y, x); }, "Power");
    
    // Percent change: ((x - y) / y) * 100
    registerOperator({"%ch", OperatorType::BINARY, OperatorCategory::ARITHMETIC, [](RPNCalculator& calc) {
        double x = calc.popStack();
        double y = calc.popStack();
        if (y == 0) {
            calc.printError("Error: Percent change from zero");
            calc.pushStack(y);
            calc.pushStack(x);
            return;
        }
        calc.lastX_ = x;
        double result = ((x - y) / y) * 100.0;
        calc.pushStack(result);
        calc.print(result);
    }, "Percent change ((x-y)/y * 100)"});
}

// ============================================================================
// TRIGONOMETRIC OPERATORS
// ============================================================================
void OperatorRegistry::registerTrigonometric() {
    registerUnaryOp("sin", OperatorCategory::TRIGONOMETRIC,
        [](RPNCalculator& c, double x) { return std::sin(c.toRadians(x)); }, "Sine");
    registerUnaryOp("cos", OperatorCategory::TRIGONOMETRIC,
        [](RPNCalculator& c, double x) { return std::cos(c.toRadians(x)); }, "Cosine");
    
    // Tangent — custom validation for cos near zero
    registerOperator({"tan", OperatorType::UNARY, OperatorCategory::TRIGONOMETRIC, [](RPNCalculator& calc) {
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
    
    // Arcsine — custom range validation
    registerOperator({"asin", OperatorType::UNARY, OperatorCategory::TRIGONOMETRIC, [](RPNCalculator& calc) {
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
    
    // Arccosine — custom range validation
    registerOperator({"acos", OperatorType::UNARY, OperatorCategory::TRIGONOMETRIC, [](RPNCalculator& calc) {
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
    
    registerUnaryOp("atan", OperatorCategory::TRIGONOMETRIC,
        [](RPNCalculator& c, double x) { return c.fromRadians(std::atan(x)); }, "Arctangent");
    registerBinaryOp("atan2", OperatorCategory::TRIGONOMETRIC,
        [](RPNCalculator& c, double y, double x) { return c.fromRadians(std::atan2(y, x)); }, "Arctangent2");
}

// ============================================================================
// HYPERBOLIC OPERATORS
// ============================================================================
void OperatorRegistry::registerHyperbolic() {
    registerUnaryOp("sinh", OperatorCategory::HYPERBOLIC,
        [](RPNCalculator&, double x) { return std::sinh(x); }, "Hyperbolic sine");
    registerUnaryOp("cosh", OperatorCategory::HYPERBOLIC,
        [](RPNCalculator&, double x) { return std::cosh(x); }, "Hyperbolic cosine");
    registerUnaryOp("tanh", OperatorCategory::HYPERBOLIC,
        [](RPNCalculator&, double x) { return std::tanh(x); }, "Hyperbolic tangent");
    registerUnaryOp("asinh", OperatorCategory::HYPERBOLIC,
        [](RPNCalculator&, double x) { return std::asinh(x); }, "Inverse hyperbolic sine");
    
    // acosh — custom range validation
    registerOperator({"acosh", OperatorType::UNARY, OperatorCategory::HYPERBOLIC, [](RPNCalculator& calc) {
        double x = calc.popStack();
        if (x < 1) {
            calc.printError("Error: acosh argument must be >= 1");
            calc.pushStack(x);
            return;
        }
        double result = std::acosh(x);
        calc.pushStack(result);
        calc.print(result);
    }, "Inverse hyperbolic cosine"});
    
    // atanh — custom range validation
    registerOperator({"atanh", OperatorType::UNARY, OperatorCategory::HYPERBOLIC, [](RPNCalculator& calc) {
        double x = calc.popStack();
        if (x <= -1 || x >= 1) {
            calc.printError("Error: atanh argument must be in (-1, 1)");
            calc.pushStack(x);
            return;
        }
        double result = std::atanh(x);
        calc.pushStack(result);
        calc.print(result);
    }, "Inverse hyperbolic tangent"});
}

// ============================================================================
// LOGARITHMIC/EXPONENTIAL OPERATORS
// ============================================================================
void OperatorRegistry::registerLogarithmic() {
    // ln, log, log2 — custom validation for non-positive input
    registerOperator({"ln", OperatorType::UNARY, OperatorCategory::LOGARITHMIC, [](RPNCalculator& calc) {
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
    
    registerOperator({"log", OperatorType::UNARY, OperatorCategory::LOGARITHMIC, [](RPNCalculator& calc) {
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
    
    registerGuardedUnaryOp("exp", OperatorCategory::LOGARITHMIC,
        [](RPNCalculator&, double x) { return std::exp(x); }, "Exponential (e^x)");
    
    registerOperator({"log2", OperatorType::UNARY, OperatorCategory::LOGARITHMIC, [](RPNCalculator& calc) {
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
    
    // logb — custom validation for non-positive and base=1
    registerOperator({"logb", OperatorType::BINARY, OperatorCategory::LOGARITHMIC, [](RPNCalculator& calc) {
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
    registerOperator({"p", OperatorType::NULLARY, OperatorCategory::STACK, [](RPNCalculator& calc) {
        calc.printStack();
    }, "Print stack"});
    
    // Clear stack
    registerOperator({"c", OperatorType::NULLARY, OperatorCategory::STACK, [](RPNCalculator& calc) {
        calc.clearStack();
        calc.printStatus("Stack cleared");
    }, "Clear stack"});
    
    // Duplicate top
    registerOperator({"d", OperatorType::NULLARY, OperatorCategory::STACK, [](RPNCalculator& calc) {
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
    registerOperator({"r", OperatorType::NULLARY, OperatorCategory::STACK, swapFunc, "Reverse top 2"});
    registerOperator({"swap", OperatorType::NULLARY, OperatorCategory::STACK, swapFunc, "Swap top 2 (alias for r)"});
    
    // Pop
    registerOperator({"pop", OperatorType::NULLARY, OperatorCategory::STACK, [](RPNCalculator& calc) {
        if (!calc.isStackEmpty()) {
            calc.popStack();
        }
    }, "Pop top value"});
    
    registerOperator({"rdn", OperatorType::NULLARY, OperatorCategory::STACK, [](RPNCalculator& calc) {
        if (calc.stackSize() < 2) return;
        std::vector<double> values;
        while (!calc.isStackEmpty()) {
            values.push_back(calc.popStack());
        }
        double top = values[0];
        for (size_t i = 0; i < values.size() - 1; ++i) {
            values[i] = values[i + 1];
        }
        values[values.size() - 1] = top;
        for (auto it = values.rbegin(); it != values.rend(); ++it) {
            calc.pushStack(*it);
        }
        calc.print(calc.peekStack());
    }, "Roll down stack"});
    
    registerOperator({"rup", OperatorType::NULLARY, OperatorCategory::STACK, [](RPNCalculator& calc) {
        if (calc.stackSize() < 2) return;
        std::vector<double> values;
        while (!calc.isStackEmpty()) {
            values.push_back(calc.popStack());
        }
        double bottom = values[values.size() - 1];
        for (size_t i = values.size() - 1; i > 0; --i) {
            values[i] = values[i - 1];
        }
        values[0] = bottom;
        for (auto it = values.rbegin(); it != values.rend(); ++it) {
            calc.pushStack(*it);
        }
        calc.print(calc.peekStack());
    }, "Roll up stack"});

    // Copy to clipboard (cross-platform)
    registerOperator({"copy", OperatorType::NULLARY, OperatorCategory::STACK, [](RPNCalculator& calc) {
        double value = calc.peekStack();
        std::ostringstream oss;
        oss << std::setprecision(calc.getScale()) << value;
        std::string str = oss.str();

        // Try platform-specific clipboard commands.
        // SAFETY: These command strings are hardcoded constants. Do not derive
        // them from user input — popen() would be a command-injection risk.
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
                    calc.printStatus("Copied: " + str);
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
    registerOperator({"sum", OperatorType::NULLARY, OperatorCategory::STACK, [](RPNCalculator& calc) {
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
    registerOperator({"prod", OperatorType::NULLARY, OperatorCategory::STACK, [](RPNCalculator& calc) {
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
// UNIT CONVERSIONS
// ============================================================================
void OperatorRegistry::registerUnitConversions() {
    // Temperature
    registerUnaryOp("c>f", OperatorCategory::CONVERSION,
        [](RPNCalculator&, double x) { return x * 9.0 / 5.0 + 32.0; }, "Celsius to Fahrenheit (F = C * 9/5 + 32)");
    registerUnaryOp("f>c", OperatorCategory::CONVERSION,
        [](RPNCalculator&, double x) { return (x - 32.0) * 5.0 / 9.0; }, "Fahrenheit to Celsius (C = (F - 32) * 5/9)");
    // Distance
    registerUnaryOp("km>mi", OperatorCategory::CONVERSION,
        [](RPNCalculator&, double x) { return x / 1.609344; }, "Kilometers to miles (1 mi = 1.609344 km)");
    registerUnaryOp("mi>km", OperatorCategory::CONVERSION,
        [](RPNCalculator&, double x) { return x * 1.609344; }, "Miles to kilometers (1 mi = 1.609344 km)");
    registerUnaryOp("m>ft", OperatorCategory::CONVERSION,
        [](RPNCalculator&, double x) { return x / 0.3048; }, "Meters to feet (1 ft = 0.3048 m)");
    registerUnaryOp("ft>m", OperatorCategory::CONVERSION,
        [](RPNCalculator&, double x) { return x * 0.3048; }, "Feet to meters (1 ft = 0.3048 m)");
    registerUnaryOp("cm>in", OperatorCategory::CONVERSION,
        [](RPNCalculator&, double x) { return x / 2.54; }, "Centimeters to inches (1 in = 2.54 cm)");
    registerUnaryOp("in>cm", OperatorCategory::CONVERSION,
        [](RPNCalculator&, double x) { return x * 2.54; }, "Inches to centimeters (1 in = 2.54 cm)");
    // Weight/mass
    registerUnaryOp("kg>lb", OperatorCategory::CONVERSION,
        [](RPNCalculator&, double x) { return x * 2.20462262; }, "Kilograms to pounds (1 kg = 2.20462262 lb)");
    registerUnaryOp("lb>kg", OperatorCategory::CONVERSION,
        [](RPNCalculator&, double x) { return x / 2.20462262; }, "Pounds to kilograms (1 kg = 2.20462262 lb)");
    registerUnaryOp("g>oz", OperatorCategory::CONVERSION,
        [](RPNCalculator&, double x) { return x / 28.3495231; }, "Grams to ounces (1 oz = 28.3495231 g)");
    registerUnaryOp("oz>g", OperatorCategory::CONVERSION,
        [](RPNCalculator&, double x) { return x * 28.3495231; }, "Ounces to grams (1 oz = 28.3495231 g)");
    // Volume
    registerUnaryOp("l>gal", OperatorCategory::CONVERSION,
        [](RPNCalculator&, double x) { return x / 3.78541178; }, "Liters to US gallons (1 gal = 3.78541178 L)");
    registerUnaryOp("gal>l", OperatorCategory::CONVERSION,
        [](RPNCalculator&, double x) { return x * 3.78541178; }, "US gallons to liters (1 gal = 3.78541178 L)");
    // Energy
    registerUnaryOp("btu>kwh", OperatorCategory::CONVERSION,
        [](RPNCalculator&, double x) { return x / 3412.14163; }, "BTU to kilowatt-hours (1 kWh = 3412.14163 BTU)");
    registerUnaryOp("kwh>btu", OperatorCategory::CONVERSION,
        [](RPNCalculator&, double x) { return x * 3412.14163; }, "Kilowatt-hours to BTU (1 kWh = 3412.14163 BTU)");
}

// ============================================================================
// MISCELLANEOUS OPERATORS
// ============================================================================
void OperatorRegistry::registerMiscellaneous() {
    // TODO: Implement additional HP calculator features:
    // - Percentage operations: %T (percent of total)
    // - Statistical functions: Σ+ Σ- mean stddev linear regression
    // - Display format: FIX SCI ENG for fixed/scientific/engineering notation
    // - Quiet mode: option to not print after every operation
    
    // Square root
    registerOperator({"sqrt", OperatorType::UNARY, OperatorCategory::MISCELLANEOUS, [](RPNCalculator& calc) {
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
    
    registerUnaryOp("abs", OperatorCategory::MISCELLANEOUS,
        [](RPNCalculator&, double x) { return std::abs(x); }, "Absolute value");
    registerUnaryOp("neg", OperatorCategory::MISCELLANEOUS,
        [](RPNCalculator&, double x) { return -x; }, "Negation");
    registerUnaryOp("chs", OperatorCategory::MISCELLANEOUS,
        [](RPNCalculator&, double x) { return -x; }, "Change sign (alias for neg)");
    
    // Square (x^2)
    registerUnaryOp("sq", OperatorCategory::MISCELLANEOUS,
        [](RPNCalculator&, double x) { return x * x; }, "Square (x^2)");
    
    // LASTX - recall last X value before operation
    registerOperator({"lastx", OperatorType::NULLARY, OperatorCategory::MISCELLANEOUS, [](RPNCalculator& calc) {
        calc.pushStack(calc.lastX_);
        calc.print(calc.lastX_);
    }, "Recall last X (last displayed value before an operation)"});
    
    // Inverse (1/x)
    registerOperator({"inv", OperatorType::UNARY, OperatorCategory::MISCELLANEOUS, [](RPNCalculator& calc) {
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
    
    registerGuardedUnaryOp("gamma", OperatorCategory::MISCELLANEOUS,
        [](RPNCalculator&, double x) { return std::tgamma(x); }, "Gamma function");
    registerGuardedUnaryOp("!", OperatorCategory::MISCELLANEOUS,
        [](RPNCalculator&, double x) { return std::tgamma(x + 1); }, "Factorial");
    registerUnaryOp("floor", OperatorCategory::MISCELLANEOUS,
        [](RPNCalculator&, double x) { return std::floor(x); }, "Floor (round down)");
    registerUnaryOp("ceil", OperatorCategory::MISCELLANEOUS,
        [](RPNCalculator&, double x) { return std::ceil(x); }, "Ceiling (round up)");
    registerUnaryOp("round", OperatorCategory::MISCELLANEOUS,
        [](RPNCalculator&, double x) { return std::round(x); }, "Round to nearest integer");
    registerUnaryOp("trunc", OperatorCategory::MISCELLANEOUS,
        [](RPNCalculator&, double x) { return std::trunc(x); }, "Truncate (round toward zero)");
    
    // Constants
    registerOperator({"pi", OperatorType::NULLARY, OperatorCategory::MISCELLANEOUS, [](RPNCalculator& calc) {
        calc.pushStack(M_PI);
        calc.print(M_PI);
    }, "Push pi (3.14159...)"});
    
    registerOperator({"e", OperatorType::NULLARY, OperatorCategory::MISCELLANEOUS, [](RPNCalculator& calc) {
        calc.pushStack(M_E);
        calc.print(M_E);
    }, "Push e (2.71828...)"});
    
    registerOperator({"phi", OperatorType::NULLARY, OperatorCategory::MISCELLANEOUS, [](RPNCalculator& calc) {
        double phi = (1.0 + std::sqrt(5.0)) / 2.0;  // Golden ratio
        calc.pushStack(phi);
        calc.print(phi);
    }, "Push phi golden ratio (1.61803...)"});
    
    // Angle mode commands
    registerOperator({"deg", OperatorType::NULLARY, OperatorCategory::MISCELLANEOUS, [](RPNCalculator& calc) {
        calc.setAngleMode("degrees");
        calc.printStatus("Angle mode: degrees");
    }, "Set degrees mode"});
    
    registerOperator({"rad", OperatorType::NULLARY, OperatorCategory::MISCELLANEOUS, [](RPNCalculator& calc) {
        calc.setAngleMode("radians");
        calc.printStatus("Angle mode: radians");
    }, "Set radians mode"});
    
    registerOperator({"grd", OperatorType::NULLARY, OperatorCategory::MISCELLANEOUS, [](RPNCalculator& calc) {
        calc.setAngleMode("gradians");
        calc.printStatus("Angle mode: gradians");
    }, "Set gradians mode"});
    
    // Help command - shows operators grouped by category
    registerOperator({"help", OperatorType::NULLARY, OperatorCategory::MISCELLANEOUS, [](RPNCalculator&) {
        OperatorRegistry& reg = OperatorRegistry::instance();
        
        // Show operators grouped by category
        for (OperatorCategory cat : reg.allCategories()) {
            std::vector<std::string> names = reg.getNamesByCategory(cat);
            if (names.empty()) continue;
            
            std::sort(names.begin(), names.end());
            std::cout << "\n" << reg.categoryName(cat) << ":" << std::endl;
            for (const auto& name : names) {
                const Operator* op = reg.getOperator(name);
                if (op) {
                    std::cout << "  " << name << " - " << op->description << std::endl;
                }
            }
        }
        std::cout << "\nVariables:" << std::endl;
        std::cout << "  name= - Store top of stack to variable 'name'" << std::endl;
        std::cout << "  name  - Recall variable 'name' (must not shadow operator)" << std::endl;
        std::cout << "  x,y,z,t - Auto-bound to top 4 stack positions (when autobind enabled)" << std::endl;
        std::cout << "\nUser-defined operators:" << std::endl;
        std::cout << "  name{ - Define operator (saved to ~/.rpn)" << std::endl;
        std::cout << "  name[ - Define temporary operator (session only)" << std::endl;
        std::cout << "  }     - End definition" << std::endl;
        std::cout << "  ]     - End definition" << std::endl;
        std::cout << "  name  - Execute operator (temporary or saved)" << std::endl;
        std::cout << "  name@ - Execute operator (backward compatibility)" << std::endl;
        std::cout << "\nSpecial commands: show, fix, fmt, autobind, q/quit/exit" << std::endl;
        std::cout << "  show/config - Display current configuration settings" << std::endl;
        std::cout << "  fix - Set decimal places (0-15, requires value on stack)" << std::endl;
        std::cout << "  fmt - Toggle locale number formatting" << std::endl;
        std::cout << "  autobind - Toggle x,y,z,t auto-binding (on by default)" << std::endl;
        std::cout << "\nTiered help: help_<category>" << std::endl;
        std::cout << "  help_arith, help_trig, help_hyper, help_log, help_stack, help_conv, help_misc, help_user" << std::endl;
    }, "Show this help"});
    
    registerOperator({"?", OperatorType::NULLARY, OperatorCategory::MISCELLANEOUS, [](RPNCalculator& calc) {
        // Alias for help
        const Operator* helpOp = OperatorRegistry::instance().getOperator("help");
        if (helpOp) helpOp->execute(calc);
    }, "Show help (alias for help)"});
    
    // Helper for category-specific help
    auto categoryHelp = [](OperatorCategory cat) {
        OperatorRegistry& reg = OperatorRegistry::instance();
        std::vector<std::string> names = reg.getNamesByCategory(cat);
        std::sort(names.begin(), names.end());
        
        std::cout << reg.categoryName(cat) << " operators:" << std::endl;
        for (const auto& name : names) {
            const Operator* op = reg.getOperator(name);
            if (op) {
                std::cout << "  " << name << " - " << op->description << std::endl;
            }
        }
    };
    
    // Tiered help commands
    registerOperator({"help_arith", OperatorType::NULLARY, OperatorCategory::MISCELLANEOUS, [categoryHelp](RPNCalculator&) {
        categoryHelp(OperatorCategory::ARITHMETIC);
    }, "Help for arithmetic operators"});
    
    registerOperator({"help_trig", OperatorType::NULLARY, OperatorCategory::MISCELLANEOUS, [categoryHelp](RPNCalculator&) {
        categoryHelp(OperatorCategory::TRIGONOMETRIC);
    }, "Help for trigonometric operators"});
    
    registerOperator({"help_hyper", OperatorType::NULLARY, OperatorCategory::MISCELLANEOUS, [categoryHelp](RPNCalculator&) {
        categoryHelp(OperatorCategory::HYPERBOLIC);
    }, "Help for hyperbolic operators"});
    
    registerOperator({"help_log", OperatorType::NULLARY, OperatorCategory::MISCELLANEOUS, [categoryHelp](RPNCalculator&) {
        categoryHelp(OperatorCategory::LOGARITHMIC);
    }, "Help for logarithmic operators"});
    
    registerOperator({"help_stack", OperatorType::NULLARY, OperatorCategory::MISCELLANEOUS, [categoryHelp](RPNCalculator&) {
        categoryHelp(OperatorCategory::STACK);
    }, "Help for stack operators"});
    
    registerOperator({"help_conv", OperatorType::NULLARY, OperatorCategory::MISCELLANEOUS, [categoryHelp](RPNCalculator&) {
        categoryHelp(OperatorCategory::CONVERSION);
    }, "Help for unit conversion operators"});
    
    registerOperator({"help_misc", OperatorType::NULLARY, OperatorCategory::MISCELLANEOUS, [categoryHelp](RPNCalculator&) {
        categoryHelp(OperatorCategory::MISCELLANEOUS);
    }, "Help for miscellaneous operators"});

    registerOperator({"help_user", OperatorType::NULLARY, OperatorCategory::MISCELLANEOUS, [categoryHelp](RPNCalculator&) {
        categoryHelp(OperatorCategory::USER);
    }, "Help for user-defined operators"});
    
    // Random number generator (0 to 1 with precision matching scale)
    registerOperator({"rand", OperatorType::NULLARY, OperatorCategory::MISCELLANEOUS, [](RPNCalculator& calc) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        
        int scale = calc.getScale();
        
        if (scale == 0) {
            // For scale 0, return 0 or 1
            std::uniform_int_distribution<> dis(0, 1);
            double result = static_cast<double>(dis(gen));
            calc.pushStack(result);
            calc.print(result);
        } else {
            // Generate integer in range [0, 10^scale] then divide by 10^scale
            long long max_val = 1;
            for (int i = 0; i < scale; ++i) {
                max_val *= 10;
            }
            std::uniform_int_distribution<long long> dis(0, max_val);
            double result = static_cast<double>(dis(gen)) / static_cast<double>(max_val);
            calc.pushStack(result);
            calc.print(result);
        }
    }, "Random number [0,1] with precision matching scale setting"});
}
