#ifndef PROGRAMS_H
#define PROGRAMS_H

#include "instructions.h"

typedef struct program {
    instruction_t* start;
    instruction_t* end;
} program_t;

const extern program_t PROGRAM_INITIALIZER;

void append(program_t* program, instruction_t* instruction);
void print(program_t* program);
void delete(program_t* program);

#endif