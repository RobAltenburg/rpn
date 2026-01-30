#ifndef OPERATORS_H
#define OPERATORS_H

#include <string>
#include <functional>
#include <map>
#include <stack>

// Forward declaration
class RPNCalculator;

// Operator types
enum class OperatorType {
    NULLARY,    // No arguments (e.g., p, c)
    UNARY,      // One argument
    BINARY      // Two arguments
};

// Operator definition
struct Operator {
    std::string name;
    OperatorType type;
    std::function<void(RPNCalculator&)> execute;
    std::string description;
};

// Operator registry - makes it easy to add new operators
class OperatorRegistry {
public:
    static OperatorRegistry& instance();
    
    void registerOperator(const Operator& op);
    bool hasOperator(const std::string& name) const;
    const Operator* getOperator(const std::string& name) const;
    
    // Get all operator names for help/extraction
    std::vector<std::string> getAllNames() const;
    
private:
    OperatorRegistry();
    std::map<std::string, Operator> operators_;
    
    // Initialize all operators
    void initializeOperators();
    
    // Helper to register operators by category
    void registerArithmetic();
    void registerTrigonometric();
    void registerLogarithmic();
    void registerStackOperations();
    void registerMiscellaneous();
};

#endif // OPERATORS_H
