#ifndef NANDY_EMU_TOOLS_H
#define NANDY_EMU_TOOLS_H

#include "nandy_tools.h"

instruction_t* ilookup(word_t word);
bool emu_step(cpu_state_t* state, FILE* outstream);

#endif