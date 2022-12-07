#include <iostream>

#include <FileCommand.h>

int main(int argc, char** argv) {
    if(argc < 2) {
        std::cout << "No path specified\n";
        return 0;
    }
    FileCommand program = FileCommand(std::string(argv[1]));
    program.execute();
}