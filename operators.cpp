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

void OperatorRegistry::removeOperator(const std::string& name) {
    operators_.erase(name);
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

void OperatorRegistry::initializeOperators() {
    registerArithmetic();
    registerTrigonometric();
    registerHyperbolic();
    registerLogarithmic();
    registerStackOperations();
    registerUnitConversions();
    registerMiscellaneous();
}

// ============================================================================
// ARITHMETIC OPERATORS
// ============================================================================
void OperatorRegistry::registerArithmetic() {
    // Addition
    registerOperator({"+", OperatorType::BINARY, OperatorCategory::ARITHMETIC, [](RPNCalculator& calc) {
        double x = calc.popStack();
        double y = calc.popStack();
        double result = y + x;
        calc.pushStack(result);
        calc.print(result);
    }, "Addition"});
    
    // Subtraction
    registerOperator({"-", OperatorType::BINARY, OperatorCategory::ARITHMETIC, [](RPNCalculator& calc) {
        double x = calc.popStack();
        double y = calc.popStack();
        double result = y - x;
        calc.pushStack(result);
        calc.print(result);
    }, "Subtraction"});
    
    // Multiplication
    registerOperator({"*", OperatorType::BINARY, OperatorCategory::ARITHMETIC, [](RPNCalculator& calc) {
        double x = calc.popStack();
        double y = calc.popStack();
        double result = y * x;
        calc.pushStack(result);
        calc.print(result);
    }, "Multiplication"});
    
    // Division
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
    
    // Modulo
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
    
    // Power
    registerOperator({"^", OperatorType::BINARY, OperatorCategory::ARITHMETIC, [](RPNCalculator& calc) {
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
    registerOperator({"sin", OperatorType::UNARY, OperatorCategory::TRIGONOMETRIC, [](RPNCalculator& calc) {
        double x = calc.popStack();
        double result = std::sin(calc.toRadians(x));
        calc.pushStack(result);
        calc.print(result);
    }, "Sine"});
    
    // Cosine
    registerOperator({"cos", OperatorType::UNARY, OperatorCategory::TRIGONOMETRIC, [](RPNCalculator& calc) {
        double x = calc.popStack();
        double result = std::cos(calc.toRadians(x));
        calc.pushStack(result);
        calc.print(result);
    }, "Cosine"});
    
    // Tangent
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
    
    // Arcsine
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
    
    // Arccosine
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
    
    // Arctangent
    registerOperator({"atan", OperatorType::UNARY, OperatorCategory::TRIGONOMETRIC, [](RPNCalculator& calc) {
        double x = calc.popStack();
        double result = calc.fromRadians(std::atan(x));
        calc.pushStack(result);
        calc.print(result);
    }, "Arctangent"});
    
    // Arctangent2
    registerOperator({"atan2", OperatorType::BINARY, OperatorCategory::TRIGONOMETRIC, [](RPNCalculator& calc) {
        double x = calc.popStack();
        double y = calc.popStack();
        double result = calc.fromRadians(std::atan2(y, x));
        calc.pushStack(result);
        calc.print(result);
    }, "Arctangent2"});
}

// ============================================================================
// HYPERBOLIC OPERATORS
// ============================================================================
void OperatorRegistry::registerHyperbolic() {
    // Hyperbolic sine
    registerOperator({"sinh", OperatorType::UNARY, OperatorCategory::HYPERBOLIC, [](RPNCalculator& calc) {
        double x = calc.popStack();
        double result = std::sinh(x);
        calc.pushStack(result);
        calc.print(result);
    }, "Hyperbolic sine"});
    
    // Hyperbolic cosine
    registerOperator({"cosh", OperatorType::UNARY, OperatorCategory::HYPERBOLIC, [](RPNCalculator& calc) {
        double x = calc.popStack();
        double result = std::cosh(x);
        calc.pushStack(result);
        calc.print(result);
    }, "Hyperbolic cosine"});
    
    // Hyperbolic tangent
    registerOperator({"tanh", OperatorType::UNARY, OperatorCategory::HYPERBOLIC, [](RPNCalculator& calc) {
        double x = calc.popStack();
        double result = std::tanh(x);
        calc.pushStack(result);
        calc.print(result);
    }, "Hyperbolic tangent"});
    
    // Inverse hyperbolic sine
    registerOperator({"asinh", OperatorType::UNARY, OperatorCategory::HYPERBOLIC, [](RPNCalculator& calc) {
        double x = calc.popStack();
        double result = std::asinh(x);
        calc.pushStack(result);
        calc.print(result);
    }, "Inverse hyperbolic sine"});
    
    // Inverse hyperbolic cosine
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
    
    // Inverse hyperbolic tangent
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
    // Natural logarithm
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
    
    // Base-10 logarithm
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
    
    // Exponential
    registerOperator({"exp", OperatorType::UNARY, OperatorCategory::LOGARITHMIC, [](RPNCalculator& calc) {
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
    
    // Arbitrary base logarithm: y logb = log_y(x) where x is below y on stack
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
        std::cout << "Stack cleared" << std::endl;
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

    // Copy to clipboard (cross-platform)
    registerOperator({"copy", OperatorType::NULLARY, OperatorCategory::STACK, [](RPNCalculator& calc) {
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
    // Temperature conversions
    registerOperator({"c>f", OperatorType::UNARY, OperatorCategory::CONVERSION, [](RPNCalculator& calc) {
        double c = calc.popStack();
        double f = c * 9.0 / 5.0 + 32.0;
        calc.pushStack(f);
        calc.print(f);
    }, "Celsius to Fahrenheit (F = C * 9/5 + 32)"});
    
    registerOperator({"f>c", OperatorType::UNARY, OperatorCategory::CONVERSION, [](RPNCalculator& calc) {
        double f = calc.popStack();
        double c = (f - 32.0) * 5.0 / 9.0;
        calc.pushStack(c);
        calc.print(c);
    }, "Fahrenheit to Celsius (C = (F - 32) * 5/9)"});
    
    // Distance conversions
    registerOperator({"km>mi", OperatorType::UNARY, OperatorCategory::CONVERSION, [](RPNCalculator& calc) {
        double km = calc.popStack();
        double mi = km / 1.609344;
        calc.pushStack(mi);
        calc.print(mi);
    }, "Kilometers to miles (1 mi = 1.609344 km)"});
    
    registerOperator({"mi>km", OperatorType::UNARY, OperatorCategory::CONVERSION, [](RPNCalculator& calc) {
        double mi = calc.popStack();
        double km = mi * 1.609344;
        calc.pushStack(km);
        calc.print(km);
    }, "Miles to kilometers (1 mi = 1.609344 km)"});
    
    registerOperator({"m>ft", OperatorType::UNARY, OperatorCategory::CONVERSION, [](RPNCalculator& calc) {
        double m = calc.popStack();
        double ft = m / 0.3048;
        calc.pushStack(ft);
        calc.print(ft);
    }, "Meters to feet (1 ft = 0.3048 m)"});
    
    registerOperator({"ft>m", OperatorType::UNARY, OperatorCategory::CONVERSION, [](RPNCalculator& calc) {
        double ft = calc.popStack();
        double m = ft * 0.3048;
        calc.pushStack(m);
        calc.print(m);
    }, "Feet to meters (1 ft = 0.3048 m)"});
    
    registerOperator({"cm>in", OperatorType::UNARY, OperatorCategory::CONVERSION, [](RPNCalculator& calc) {
        double cm = calc.popStack();
        double in = cm / 2.54;
        calc.pushStack(in);
        calc.print(in);
    }, "Centimeters to inches (1 in = 2.54 cm)"});
    
    registerOperator({"in>cm", OperatorType::UNARY, OperatorCategory::CONVERSION, [](RPNCalculator& calc) {
        double in = calc.popStack();
        double cm = in * 2.54;
        calc.pushStack(cm);
        calc.print(cm);
    }, "Inches to centimeters (1 in = 2.54 cm)"});
    
    // Weight/mass conversions
    registerOperator({"kg>lb", OperatorType::UNARY, OperatorCategory::CONVERSION, [](RPNCalculator& calc) {
        double kg = calc.popStack();
        double lb = kg * 2.20462262;
        calc.pushStack(lb);
        calc.print(lb);
    }, "Kilograms to pounds (1 kg = 2.20462262 lb)"});
    
    registerOperator({"lb>kg", OperatorType::UNARY, OperatorCategory::CONVERSION, [](RPNCalculator& calc) {
        double lb = calc.popStack();
        double kg = lb / 2.20462262;
        calc.pushStack(kg);
        calc.print(kg);
    }, "Pounds to kilograms (1 kg = 2.20462262 lb)"});
    
    registerOperator({"g>oz", OperatorType::UNARY, OperatorCategory::CONVERSION, [](RPNCalculator& calc) {
        double g = calc.popStack();
        double oz = g / 28.3495231;
        calc.pushStack(oz);
        calc.print(oz);
    }, "Grams to ounces (1 oz = 28.3495231 g)"});
    
    registerOperator({"oz>g", OperatorType::UNARY, OperatorCategory::CONVERSION, [](RPNCalculator& calc) {
        double oz = calc.popStack();
        double g = oz * 28.3495231;
        calc.pushStack(g);
        calc.print(g);
    }, "Ounces to grams (1 oz = 28.3495231 g)"});
    
    // Volume conversions
    registerOperator({"l>gal", OperatorType::UNARY, OperatorCategory::CONVERSION, [](RPNCalculator& calc) {
        double l = calc.popStack();
        double gal = l / 3.78541178;
        calc.pushStack(gal);
        calc.print(gal);
    }, "Liters to US gallons (1 gal = 3.78541178 L)"});
    
    registerOperator({"gal>l", OperatorType::UNARY, OperatorCategory::CONVERSION, [](RPNCalculator& calc) {
        double gal = calc.popStack();
        double l = gal * 3.78541178;
        calc.pushStack(l);
        calc.print(l);
    }, "US gallons to liters (1 gal = 3.78541178 L)"});
    
    // Energy conversions
    registerOperator({"btu>kwh", OperatorType::UNARY, OperatorCategory::CONVERSION, [](RPNCalculator& calc) {
        double btu = calc.popStack();
        double kwh = btu / 3412.14163;
        calc.pushStack(kwh);
        calc.print(kwh);
    }, "BTU to kilowatt-hours (1 kWh = 3412.14163 BTU)"});
    
    registerOperator({"kwh>btu", OperatorType::UNARY, OperatorCategory::CONVERSION, [](RPNCalculator& calc) {
        double kwh = calc.popStack();
        double btu = kwh * 3412.14163;
        calc.pushStack(btu);
        calc.print(btu);
    }, "Kilowatt-hours to BTU (1 kWh = 3412.14163 BTU)"});
}

// ============================================================================
// MISCELLANEOUS OPERATORS
// ============================================================================
void OperatorRegistry::registerMiscellaneous() {
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
    
    // Absolute value
    registerOperator({"abs", OperatorType::UNARY, OperatorCategory::MISCELLANEOUS, [](RPNCalculator& calc) {
        double x = calc.popStack();
        double result = std::abs(x);
        calc.pushStack(result);
        calc.print(result);
    }, "Absolute value"});
    
    // Negation
    registerOperator({"neg", OperatorType::UNARY, OperatorCategory::MISCELLANEOUS, [](RPNCalculator& calc) {
        double x = calc.popStack();
        double result = -x;
        calc.pushStack(result);
        calc.print(result);
    }, "Negation"});
    
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
    
    // Gamma function
    registerOperator({"gamma", OperatorType::UNARY, OperatorCategory::MISCELLANEOUS, [](RPNCalculator& calc) {
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
    registerOperator({"!", OperatorType::UNARY, OperatorCategory::MISCELLANEOUS, [](RPNCalculator& calc) {
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
    registerOperator({"floor", OperatorType::UNARY, OperatorCategory::MISCELLANEOUS, [](RPNCalculator& calc) {
        double x = calc.popStack();
        double result = std::floor(x);
        calc.pushStack(result);
        calc.print(result);
    }, "Floor (round down)"});
    
    // Ceil
    registerOperator({"ceil", OperatorType::UNARY, OperatorCategory::MISCELLANEOUS, [](RPNCalculator& calc) {
        double x = calc.popStack();
        double result = std::ceil(x);
        calc.pushStack(result);
        calc.print(result);
    }, "Ceiling (round up)"});
    
    // Round
    registerOperator({"round", OperatorType::UNARY, OperatorCategory::MISCELLANEOUS, [](RPNCalculator& calc) {
        double x = calc.popStack();
        double result = std::round(x);
        calc.pushStack(result);
        calc.print(result);
    }, "Round to nearest integer"});
    
    // Trunc
    registerOperator({"trunc", OperatorType::UNARY, OperatorCategory::MISCELLANEOUS, [](RPNCalculator& calc) {
        double x = calc.popStack();
        double result = std::trunc(x);
        calc.pushStack(result);
        calc.print(result);
    }, "Truncate (round toward zero)"});
    
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
        std::cout << "Angle mode: degrees" << std::endl;
    }, "Set degrees mode"});
    
    registerOperator({"rad", OperatorType::NULLARY, OperatorCategory::MISCELLANEOUS, [](RPNCalculator& calc) {
        calc.setAngleMode("radians");
        std::cout << "Angle mode: radians" << std::endl;
    }, "Set radians mode"});
    
    registerOperator({"grd", OperatorType::NULLARY, OperatorCategory::MISCELLANEOUS, [](RPNCalculator& calc) {
        calc.setAngleMode("gradians");
        std::cout << "Angle mode: gradians" << std::endl;
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
        std::cout << "\nMacros:" << std::endl;
        std::cout << "  name[ - Start recording macro 'name'" << std::endl;
        std::cout << "  ]     - Stop recording" << std::endl;
        std::cout << "  name@ - Play macro 'name'" << std::endl;
        std::cout << "\nUser-defined operators:" << std::endl;
        std::cout << "  name{ - Start defining operator 'name' (spacing flexible: 'name {' or 'name{body}')" << std::endl;
        std::cout << "  }     - Stop defining and save to ~/.rpn" << std::endl;
        std::cout << "\nSpecial commands: scale, fmt, q/quit/exit" << std::endl;
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
}
