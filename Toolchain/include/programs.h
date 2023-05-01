#ifndef PROGRAMS_H
#define PROGRAMS_H

#include "instructions.h"

typedef struct program {
    instruction_t* start;
    instruction_t* end;
} program_t;



#endif