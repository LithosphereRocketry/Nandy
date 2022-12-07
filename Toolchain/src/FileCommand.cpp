#include "FileCommand.h"

#include <iostream>

FileCommand::FileCommand(std::string path) {
    std::ifstream fs(path);
    for(std::string line; std::getline(fs, line); ) {
        std::cout << line << std::endl;
    } 
}