#ifndef RPN_H
#define RPN_H

#include <stack>
#include <string>
#include <map>
#include <vector>

class RPNCalculator {
public:
    RPNCalculator();
    
    // Main entry points
    void run();                              // Interactive mode
    void evaluate(const std::string& expr);  // Non-interactive: evaluate expression and print result
    
    // Stack operations - these need to be public for operators to access
    void pushStack(double value);
    double popStack();  // Returns 0 if empty
    double peekStack() const;  // Returns 0 if empty
    bool isStackEmpty() const;
    size_t stackSize() const;
    void clearStack();
    void printStack() const;
    
    // Memory operations (numeric slots - deprecated, use named variables)
    void storeMemory(int location, double value);
    double recallMemory(int location) const;
    
    // Named variables
    bool storeVariable(const std::string& name, double value);  // Returns false if name shadows operator
    bool hasVariable(const std::string& name) const;
    double recallVariable(const std::string& name) const;
    
    // Named macros
    bool hasNamedMacro(const std::string& name) const;
    const std::vector<std::string>* getNamedMacro(const std::string& name) const;
    
    // Settings
    void setAngleMode(const std::string& mode);
    std::string getAngleMode() const;
    void setScale(int s);
    int getScale() const;
    
    // Angle conversions
    double toRadians(double angle) const;
    double fromRadians(double angle) const;
    
    // Output
    void print(double value) const;
    void printError(const std::string& message) const;
    
private:
    enum class AngleMode { RADIANS, DEGREES, GRADIANS };
    
    std::stack<double> stack_;
    std::map<int, double> memory_;
    AngleMode angleMode_;
    int scale_;
    
    // Macro recording (can be loaded from .rpn config file)
    std::map<int, std::vector<std::string>> macros_;  // slot -> recorded tokens (deprecated)
    std::map<std::string, std::vector<std::string>> namedMacros_;  // name -> recorded tokens
    int recordingSlot_;           // -1 if not recording (numeric)
    std::string recordingName_;   // empty if not recording (named)
    std::vector<std::string> recordingBuffer_;
    bool isRecording() const { return recordingSlot_ >= 0 || !recordingName_.empty(); }
    bool isPlayingMacro_;         // Prevent nested macro playback
    
    // Named variables
    std::map<std::string, double> namedVariables_;
    
    // Helper methods
    void removeTrailingZeros();
    void loadConfig();
    void detectLocaleSeparators();
    bool isNumber(const std::string& token) const;
    std::string normalizeNumber(const std::string& token) const;
    std::string extractOperator(const std::string& token, size_t& opStart) const;

    // Locale settings
    char decimalSeparator_;
    char thousandsSeparator_;
    bool localeFormatting_;  // Format output with locale separators (on by default)

    // Formatting helper
    std::string formatNumber(double value) const;

    // Processing
    void processLine(const std::string& line);
    void processStatement(const std::string& statement);
    void processToken(const std::string& token);
    void executeOperator(const std::string& op);
};

#endif // RPN_H
