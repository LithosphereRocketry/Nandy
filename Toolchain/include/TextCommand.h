#ifndef TEXT_COMMAND_H
#define TEXT_COMMAND_H

#include <string>

#include "Command.h"

class TextCommand: public Command {
    public:
        TextCommand(std::string text);
        std::vector<std::string> getLabels();
        int execute();
        int executeLabel(std::string label);
        int assemble();
    private:
        Command* cmd;
        void trace(int depth);
};

#endif