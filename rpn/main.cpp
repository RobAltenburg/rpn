//
//  main.cpp
//  rpn2
//
//  Created by Robert Altenburg on 1/29/25.
//

#include "main.hpp"
#include "functions.hpp"
#include "DoubleVector.hpp"

using FunctionType = void(*)(DoubleVector&, State&);
std::set<char> immediateChars = {'+', '-', '*', '/', '^', '%'}; // List of immediate operators
    
std::string formatDouble(double value, State& state) {
    std::ostringstream oss;
    oss.precision(state.precision);
    oss << std::fixed << value;
    std::string result = oss.str();
    
    // Remove trailing zeros
    result.erase(result.find_last_not_of('0') + 1, std::string::npos);
    // Remove the decimal point if it's the last character
    if (result.back() == '.') {
        result.pop_back();
    }
    
    return result;
}

double convertToDouble(const std::string& str) {
    if (str.find("0x") == 0 || str.find("0X") == 0) {
        return static_cast<double>(std::stoll(str, nullptr, 16));
    } else if (str.find("0b") == 0 || str.find("0B") == 0) {
        return static_cast<double>(std::stoll(str.substr(2), nullptr, 2));
    } else if (str.find("0o") == 0 || str.find("0O") == 0) {
        return static_cast<double>(std::stoll(str.substr(2), nullptr, 8));
    } else {
        return std::stod(str);
    }
}

int main(int argc, const char * argv[]) {
    tb_event event;
    DoubleVector stack;
    State state; // Create an instance of State
    std::string line;
    std::string entry;
    std::regex pattern(R"(([-+]?(0[xX][\da-fA-F]+|0[oO]?[0-7]+|0[bB][01]+|\d*\.?\d+([eE][-+]?\d+)?))(.*))");
 
    std::smatch matches;
    double number;
    int write_point;
    char last_character;
    bool flag_arrow_down = false;
    
    // Initialize the Termbox library
    int init_result = tb_init();
    if (init_result != 0) {
        fprintf(stderr, "Init Termbox Error: %s\n", tb_strerror(init_result));
        return 1;
    }

    std::map<std::string, FunctionType> functionMap;
    functionMap["+"] = funcAdd;
    functionMap["-"] = funcSubtract;
    functionMap["*"] = funcMultiply;
    functionMap["/"] = funcDivide;
    functionMap["%"] = funcModulo;
    functionMap["r"] = funcReciprocal;
    functionMap["^"] = funcPower;
    functionMap["pow"] = funcPower;
    functionMap["root"] = funcRoot;
    functionMap["sin"] = funcSin;
    functionMap["cos"] = funcCos;
    functionMap["tan"] = funcTan;
    functionMap["asin"] = funcArcSin;
    functionMap["acos"] = funcArcCos;
    functionMap["atan"] = funcArcTan;
    functionMap["atan2"] = funcArcTan2;
    functionMap["sinh"] = funcSinh;
    functionMap["cosh"] = funcCosh;
    functionMap["tanh"] = funcTanh;
    functionMap["asinh"] = funcArcSinh;
    functionMap["acosh"] = funcArcCosh;
    functionMap["atanh"] = funcArcTanh;
    functionMap["log"] = funcLog;
    functionMap["log10"] = funcLog10;
    functionMap["invlog"] = funcInverseLog;
    functionMap["e^x"] = funcInverseLog;
    functionMap["invlog10"] = funcInverseLog10;
    functionMap["ten^x"] = funcInverseLog10;
    functionMap["chs"] = funcChangeSign;
    functionMap["pop"] = funcPop;
    functionMap["~"] = funcSwap;
    functionMap["sto"] = funcStore;
    functionMap["rcl"] = funcRecall;
    functionMap["mc"] = funcMemoryClear;
    functionMap["pi"] = funcPi;
    functionMap["e"] = funcE;
    functionMap["hypot"] =  funcHypotenuse;
    functionMap["deg"] =  funcDegrees;
    functionMap["rad"] =  funcRadians;
    functionMap["grd"] =  funcGradians;
    functionMap["sum"] =  funcSum;
    
    functionMap["save"] =  funcSave; // puts the stack size in mem[0] and copies the rest of the stack to memory.
    functionMap["restore"] =  funcRestore; // pushes the saved values back to the stack
    
    while (true) {
        write_point = 5;
        line = "";
        last_character = ' ';
        flag_arrow_down = false;
        stack.removeLeadingZeros(); // removes unnecesary zeros in the stack
        tb_clear();
        tb_printf(0, 0, TB_RED, 0, "last: %s", state.last_command.c_str());
        tb_printf(0, tb_height() - 1, TB_GREEN, 0, "rpn>");
        tb_printf(0, tb_height() - 2, TB_BLUE, 0, "x:  %s", formatDouble(stack.x(),state).c_str());
        tb_printf(0, tb_height() - 3, TB_BLUE, 0, "y:  %s", formatDouble(stack.y(),state).c_str());
        tb_printf(0, tb_height() - 4, TB_BLUE, 0, "z:  %s", formatDouble(stack.z(),state).c_str());
        
        // show the extended stack if it is needed
        if (stack.size() > 3) {
            for (int i = 4; i <= tb_height(); i++) {
                if (stack.size() >= i) {
                    tb_printf(0, tb_height() - (i + 1), TB_BLUE, 0, "%d:  %s", i, formatDouble(stack.at(i-1),state).c_str());
                }
            }
        }
        
        // show the memory slots if used
        int mem_position = tb_height() - 2;
        for (int i = 0; i < MEMORY_SIZE; i++) {
            if (state.memory[i] != 0) {
                //tb_printf(20, mem_position, TB_MAGENTA, 0, "MEM[%d]:  %g", i, state.memory[i]);
                tb_printf(20, mem_position, TB_MAGENTA, 0, "MEM[%d]:  %s", i, formatDouble(state.memory[i],state).c_str());
                mem_position--;
            }
            
        }
        
        // show the degree setting
        if (state.drg == RADIANS) {
            tb_printf(tb_width() - 3, 0, TB_MAGENTA, 0, "rad");
        } else if (state.drg == GRADIANS) {
            tb_printf(tb_width() - 3, 0, TB_MAGENTA, 0, "grd");
        }
        
        tb_present();
        while (true) {
            // Event loop
            tb_poll_event(&event);
            if (event.key == TB_KEY_ENTER) {                        // If ENTER, line is complete
                break;
            } else if (((event.key == 127) ||                       // Process backspace
                        (event.key == TB_KEY_BACKSPACE) ||
                        (event.key == TB_KEY_BACKSPACE2) ||
                        (event.key == TB_KEY_DELETE)) && write_point > 5) {
                    write_point--;
                    tb_printf(write_point, tb_height() - 1, TB_RED, 0, " ");
                    tb_present();
                    if (!line.empty()) {
                        line.pop_back();
                        if (!line.empty()) {
                            last_character = line.back();
                        } else {
                            last_character = ' ';
                        }
                    }
            } else if (event.key == TB_KEY_ARROW_DOWN) {
                stack.pop();
                flag_arrow_down = true;
                break;
                
            } else if (event.ch != 0) {                         // Process the characters
                
                    line += event.ch;
                    tb_printf(write_point, tb_height() - 1, TB_BLUE, 0, "%c", event.ch);
                    tb_present();
                    write_point++;
                
                    if ((immediateChars.find(event.ch) != immediateChars.end())         //
                            && (last_character != 'e') && (last_character != 'E')) break;
           
                    last_character = event.ch; // needed to check that + or - isn't part of scientific notation
            }
        } // completed loop for reading one line

//------------------------------------------------------------Entry Processing
        if (line == "q") break;
        if (line == ".") line = state.last_command;
        
        try {
            if (std::regex_match(line, matches, pattern)) {
                number = convertToDouble(matches[1].str());
                entry = matches[4].str();
                
                
                if (!matches[1].str().empty()) { // There is just a number
                    stack.push(number);          // Push it on the stack
                }
                
                if (!entry.empty()) { // There is a non-numeric part
                    if (functionMap.find(entry) != functionMap.end()) {  // just a function
                        functionMap[entry](stack, state); // Call the function
                        state.last_command = entry;  // Save it to state.last_command
                    }}
                
            } else if (functionMap.find(line) != functionMap.end())  {  // just a function
                functionMap[line](stack, state); // Call the function
                state.last_command = line;  // Save it to state.last_command
                
            } else if (!flag_arrow_down && line.empty()){ // just a return
                stack.push(stack.x());  // push x again
                
            }
        }
        
        catch (int error_number) {
            tb_printf(0, tb_height() - 1, TB_RED, 0, "ERROR %d", error_number);
                tb_present();
                usleep(2000000); // 2 second delay
        }
        
        
    } // completed processing loop
    
    tb_shutdown();
    system("reset");
    return 0;
}
