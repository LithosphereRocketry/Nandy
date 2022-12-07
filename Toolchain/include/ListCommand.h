#ifndef LIST_COMMAND_H
#define LIST_COMMAND_H

#include <vector>

#include "Command.h"

class ListCommand: public Command {
    public:
        bool execute();
        bool assemble();
    protected:
        std::vector<Command*> cmds;
};

#endif