#include "TextCommand.h"
#include "NullCommand.h"
#include <iostream>
#include <sstream>


TextCommand::TextCommand(std::string s) {
    // purge comments
    size_t cpos = s.find(commentSym);
    s = s.substr(0, cpos);
    
    // identify and purge labels
    size_t lpos = s.find(labelSym);
    if(lpos != std::string::npos) {
        label = s.substr(0, lpos);
        s = s.substr(lpos + labelSym.length());
    }

    // identify instruction keyword
    std::string inst;
    std::istringstream sstr(s);
    sstr >> inst;

    try {
        // make a copy of the corresponding instruction from the index
        // using .at here rather than [] because [] isn't const (thanks stdlib)
        Command* cmdref = commandMap.at(inst);
        if(!cmdref) {    
            throw InvalidInstructionException(inst);
        }
        cmd = cmdref->clone();
    } catch(std::out_of_range e) {
        throw InvalidInstructionException(inst);
    }
}

TextCommand* TextCommand::clone() {
    return new TextCommand(*this);
}

std::vector<std::string> TextCommand::getLabels() {
    return std::vector<std::string>();
}

int TextCommand::execute(CPUState* state) { return cmd->execute(state); }
int TextCommand::executeLabel(CPUState* state, std::string l) { return cmd->executeLabel(state, l); }

int TextCommand::assemble() {
    int count = 0;
    return count;
}

void TextCommand::trace(int depth) {

}