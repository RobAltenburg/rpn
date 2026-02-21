#ifndef OPERATORS_H
#define OPERATORS_H

#include <string>
#include <functional>
#include <map>
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
    
private:
    OperatorRegistry();
    std::map<std::string, Operator> operators_;
    
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
