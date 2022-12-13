#include "ListCommand.h"

ListCommand* ListCommand::clone() {
    return new ListCommand(*this);
}

std::vector<std::string> ListCommand::getLabels() {
    return std::vector<std::string>();
}

int ListCommand::execute(CPUState* state) {
    int count = 0;
    for(int i = 0; i < cmds.size(); i++) {
        count += cmds[i]->execute(state);
    }
    return count;
}

int ListCommand::executeLabel(CPUState* state, std::string label) {

}

int ListCommand::assemble() {
    int count = 0;
    for(int i = 0; i < cmds.size(); i++) {
        count += cmds[i]->assemble();
    }
    return count;
}

void ListCommand::trace(int depth) {

}