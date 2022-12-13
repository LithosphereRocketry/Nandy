#ifndef NULL_COMMAND_H
#define NULL_COMMAND_H

#include "Command.h"

class NullCommand: public Command {
    public:
        Command* clone() { return new NullCommand(); }
        std::vector<std::string> getLabels() { return std::vector<std::string>(); }
        int execute(CPUState* state) { return 0; }
        int executeLabel(CPUState* state, std::string label) { throw MissingLabelException(label); }
        int assemble() { return 0; }
    protected:
        void trace(int depth) {}
};

#endif