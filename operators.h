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

#ifndef OPERATORS_H
#define OPERATORS_H

#include <string>
#include <functional>
#include <unordered_map>
#include <stack>
#include <vector>

// Forward declaration
class RPNCalculator;

// Operator types
enum class OperatorType {
    NULLARY,    // No arguments (e.g., p, c)
    UNARY,      // One argument
    BINARY      // Two arguments
};

// Operator categories for help organization
enum class OperatorCategory {
    ARITHMETIC,
    TRIGONOMETRIC,
    HYPERBOLIC,
    LOGARITHMIC,
    STACK,
    CONVERSION,
    MISCELLANEOUS,
    USER
};

// Operator definition
struct Operator {
    std::string name;
    OperatorType type;
    OperatorCategory category;
    std::function<void(RPNCalculator&)> execute;
    std::string description;
};

// Operator registry - makes it easy to add new operators
class OperatorRegistry {
public:
    static OperatorRegistry& instance();
    
    void registerOperator(const Operator& op);
    void removeOperator(const std::string& name);
    bool hasOperator(const std::string& name) const;
    const Operator* getOperator(const std::string& name) const;
    
    // Get all operator names for help/extraction
    std::vector<std::string> getAllNames() const;
    std::vector<std::string> getNamesByCategory(OperatorCategory category) const;
    static std::string categoryName(OperatorCategory category);
    static const std::vector<OperatorCategory>& allCategories();

    // Cached names sorted by length (desc) for operator extraction
    const std::vector<std::string>& getNamesSortedByLengthDesc();

    // Readline completions management (encapsulates former global g_completions)
    void setBuiltinCompletions(const std::vector<std::string>& builtins);
    const std::vector<std::string>& completions();
    
private:
    OperatorRegistry();
    std::unordered_map<std::string, Operator> operators_;

    // Caches
    bool names_len_desc_dirty_ = true;
    std::vector<std::string> names_len_desc_cache_;

    // Completions
    bool completions_dirty_ = true;
    std::vector<std::string> builtins_;
    std::vector<std::string> completions_cache_;
    
    // Initialize all operators
    void initializeOperators();
    
    // Helper to register operators by category
    void registerArithmetic();
    void registerTrigonometric();
    void registerHyperbolic();
    void registerLogarithmic();
    void registerStackOperations();
    void registerUnitConversions();
    void registerMiscellaneous();
};

#endif // OPERATORS_H
