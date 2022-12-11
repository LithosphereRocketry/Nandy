#ifndef COMMAND_H
#define COMMAND_H

#include <vector>
#include <string>

class Command {
    public:
        virtual std::vector<std::string> getLabels() = 0;
        virtual int execute() = 0;
        virtual int executeLabel(std::string label) = 0;
        virtual int assemble() = 0;
        void trace();
    protected:
        virtual void trace(int depth) = 0;
};

#endif