#include "rpn.h"
#include <iostream>
#include <string>
#include <cstring>

void printUsage(const char* progname) {
    std::cerr << "Usage: " << progname << " [-e expression]" << std::endl;
    std::cerr << "  -e expression  Evaluate expression and exit" << std::endl;
    std::cerr << "  -h, --help     Show this help" << std::endl;
    std::cerr << "  (no args)      Start interactive mode" << std::endl;
}

int main(int argc, char* argv[]) {
    RPNCalculator calc;
    
    if (argc == 1) {
        // No arguments: interactive mode
        calc.run();
    } else if (argc == 2 && (std::strcmp(argv[1], "-h") == 0 || std::strcmp(argv[1], "--help") == 0)) {
        printUsage(argv[0]);
        return 0;
    } else if (argc == 3 && std::strcmp(argv[1], "-e") == 0) {
        // -e expression: evaluate and exit
        calc.evaluate(argv[2]);
    } else if (argc == 2 && std::strcmp(argv[1], "-e") != 0) {
        // Single argument without -e: treat as expression
        calc.evaluate(argv[1]);
    } else {
        printUsage(argv[0]);
        return 1;
    }
    
    return 0;
}
