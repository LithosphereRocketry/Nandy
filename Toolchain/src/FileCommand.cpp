#include "FileCommand.h"

#include <iostream>

FileCommand* FileCommand::clone() {
    return new FileCommand(*this);
}

FileCommand::FileCommand(std::string path) {
    std::ifstream fs(path);
    for(std::string line; std::getline(fs, line); ) {
        cmds.push_back(new TextCommand(line));
    } 
}