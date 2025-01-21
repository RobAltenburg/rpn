//
//  readLineWithVariant.cpp
//  rpn
//
//  Created by Robert Altenburg on 11/8/24.
//

#include "readLineWithVariant.hpp"

void setNonBlockingInput() {
    struct termios ttystate;
    tcgetattr(STDIN_FILENO, &ttystate);
    ttystate.c_lflag &= ~ICANON;
    ttystate.c_cc[VMIN] = 1;
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

void resetBlockingInput() {
    struct termios ttystate;
    tcgetattr(STDIN_FILENO, &ttystate);
    ttystate.c_lflag |= ICANON;
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

bool kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return true;
    }

    return false;
}

int readLineWithVarient(VariantType &number, std::string &text) {
    setNonBlockingInput();
    std::string line;
    char ch;
    std::set<char> immediateChars = {'+', '-', '*', '/', '^', '%', '\n'}; // List of immediate operators
    
    //std::getline(std::cin, line);
    
    while (true) {
        if (kbhit()) {
            ch = getchar();
            line += ch;
            if (immediateChars.find(ch) != immediateChars.end()) {
                if (ch != '\n') {
                    std::cout << "\n";
                }
                break;
            }

        }
        // You can add a small sleep here to prevent high CPU usage
        usleep(10000); // Sleep for 10 milliseconds
    }
    
    // Check if the last character is a newline
    if (!line.empty() && line.back() == '\n') {
        line.erase(line.size() - 1); // Remove the last character
    }
    
    std::istringstream iss(line);
    double tempNumber;
    if (iss >> tempNumber) {
        number = tempNumber;
        std::getline(iss >> std::ws, text); // extract the remainder of the line
    } else {
        number = std::monostate{};      // the number is null
        text = line;                    // the whole line is text
    }
    if (std::cin.eof()) {
        return EOF;
    } else {
        return 0;
    }

}
