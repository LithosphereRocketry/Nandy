#include "FileCommand.h"

#include <iostream>

FileCommand::FileCommand(std::string path) {
    std::ifstream fs(path);
    for(std::string line; std::getline(fs, line); ) {
        cmds.push_back(new TextCommand(line));
    } 
}