#include "programs.h"

const program_t PROGRAM_INITIALIZER = {NULL, NULL};

void append(program_t* program, instruction_t* instruction) {
    if(program->end) {
        insertInstruction(program->end, instruction);
    } else {
        program->start = instruction;
    }
    program->end = instruction;
}

void print(program_t* program) {
    instruction_t* instr = program->start;
    while(instr) {
        printInstruction(instr);
        instr = instr->next;
    }
}

void delete(program_t* program) {
    instruction_t* instr = program->start;
    while(instr) {
        instruction_t* nextinst = instr->next;
        deleteInstruction(instr);
        instr = nextinst;
    }
}