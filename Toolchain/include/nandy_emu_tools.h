#ifndef NANDY_EMU_TOOLS_H
#define NANDY_EMU_TOOLS_H

#include "nandy_tools.h"

typedef bool (*io_step_t)(cpu_state_t*, bool);

typedef struct {
    word_t base;
    word_t bound;
    io_step_t operation;
} iorange_t;

const instruction_t* ilookup(word_t word);
bool emu_step(cpu_state_t* state, FILE* outstream);

#endif