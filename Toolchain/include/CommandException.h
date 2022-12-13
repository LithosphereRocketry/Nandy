#ifndef COMMAND_EXCEPTION_H
#define COMMAND_EXCEPTION_H

#include <exception>
#include <string>

class InvalidInstructionException: public std::exception {
    public:
        InvalidInstructionException(std::string inst) {
            instruction = inst;
        }
        const char* what() const throw() {
            return instruction.c_str();
        }
    private:
        std::string instruction;
};

class MissingLabelException: public std::exception {
    public:
        MissingLabelException(std::string l) {
            label = l;
        }
        const char* what() const throw() {
            return label.c_str();
        }
    private:
        std::string label;
};

#endif