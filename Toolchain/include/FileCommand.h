#ifndef FILE_COMMAND_H
#define FILE_COMMAND_H

#include <string>
#include <fstream>

#include "ListCommand.h"
#include "TextCommand.h"

class FileCommand: public ListCommand {
    public:
        FileCommand(std::string path);
        FileCommand* clone();
};

#endif