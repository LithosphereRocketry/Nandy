#ifndef COMMAND_H
#define COMMAND_H

class Command {
    public:
        virtual bool execute() = 0;
        virtual bool assemble() = 0;
};

#endif