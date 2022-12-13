#ifndef ISP_COMMAND_H
#define ISP_COMMAND_H

#include "Command.h"

class ISPCommand: public Command {
    public:
        ISPCommand(int dir) { direction = dir; }
        ISPCommand* clone() { return new ISPCommand(*this); }
        std::vector<std::string> getLabels() { return std::vector<std::string>(); };
        int execute(CPUState* state);
        int executeLabel(CPUState* state, std::string label) { throw MissingLabelException(label); };
        int assemble();
    protected:
        void trace(int depth);
    private:
        int direction;

};

#endif