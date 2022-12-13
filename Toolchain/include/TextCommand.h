#ifndef TEXT_COMMAND_H
#define TEXT_COMMAND_H

#include <string>
#include <unordered_map>

#include "Command.h"

class TextCommand: public Command {
    public:
        TextCommand* clone();
        TextCommand(std::string text);
        std::vector<std::string> getLabels();
        int execute(CPUState* state);
        int executeLabel(CPUState* state, std::string label);
        int assemble();
    private:
        Command* cmd;
        std::string line;
        std::string label;
        void trace(int depth);
        static const std::unordered_map<std::string, Command*> commandMap;
        static const std::string commentSym;
        static const std::string labelSym;
};

#endif