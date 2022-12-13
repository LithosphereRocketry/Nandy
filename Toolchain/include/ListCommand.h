#ifndef LIST_COMMAND_H
#define LIST_COMMAND_H

#include <vector>
#include <string>
#include <unordered_map>

#include "Command.h"

class ListCommand: public Command {
    public:
        ListCommand* clone();
        std::vector<std::string> getLabels();
        int execute(CPUState* state);
        int executeLabel(CPUState* state, std::string label);
        int assemble();
    protected:
        std::vector<Command*> cmds;
        void trace(int depth);
};

#endif