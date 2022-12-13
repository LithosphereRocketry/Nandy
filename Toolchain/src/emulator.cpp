#include <iostream>

#include <FileCommand.h>

int main(int argc, char** argv) {
    if(argc < 2) {
        std::cout << "No path specified\n";
        return 0;
    }
    CPUState state;
    state.pc = 0;
    try {
        FileCommand program = FileCommand(std::string(argv[1]));
        program.executeLabel(&state, "main");
    } catch(InvalidInstructionException e) {
        std::cout << "Invalid instruction \"" << e.what() << "\"" << std::endl;
    }
}