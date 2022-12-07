#include "ListCommand.h"

bool ListCommand::assemble() {
    for(int i = 0; i < cmds.size(); i++) {
        if(!cmds[i]->assemble()) {
            return false;
        }
    }
    return true;
}

bool ListCommand::execute() {
    for(int i = 0; i < cmds.size(); i++) {
        if(!cmds[i]->execute()) {
            return false;
        }
    }
    return true;
}