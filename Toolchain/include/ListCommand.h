#ifndef LIST_COMMAND_H
#define LIST_COMMAND_H

#include <vector>
#include <string>

#include "Command.h"

class ListCommand: public Command {
    public:
        std::vector<std::string> getLabels();
        int execute();
        int executeLabel(std::string label);
        int assemble();
    protected:
        std::vector<Command*> cmds;
        void trace(int depth);
};

#endif