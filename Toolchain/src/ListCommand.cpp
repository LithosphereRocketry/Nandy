#include "ListCommand.h"

std::vector<std::string> ListCommand::getLabels() {
    return std::vector<std::string>();
}

int ListCommand::execute() {
    int count = 0;
    for(int i = 0; i < cmds.size(); i++) {
        count += cmds[i]->execute();
    }
    return count;
}

int ListCommand::executeLabel(std::string label) {

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