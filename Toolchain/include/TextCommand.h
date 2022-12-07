#ifndef TEXT_COMMAND_H
#define TEXT_COMMAND_H

#include <string>

#include "Command.h"

class TextCommand: public Command {
    public:
        TextCommand(std::string text);
        bool execute();
        bool assemble();
    private:
        Command* cmd;
};

#endif