#ifndef COMMAND_H
#define COMMAND_H

#include <vector>
#include <string>

#include "CommandException.h"

typedef int8_t word_t;

typedef struct {
    word_t pc;
    word_t acc;
    word_t data;
    word_t sp;
    word_t carry;
} CPUState;

class Command {
    public:
        virtual Command* clone() = 0;
        virtual std::vector<std::string> getLabels() = 0;
        virtual int execute(CPUState* state) = 0;
        virtual int executeLabel(CPUState* state, std::string label) = 0;
        virtual int assemble() = 0;
        void trace();
    protected:
        virtual void trace(int depth) = 0;
};

#endif