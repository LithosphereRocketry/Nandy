#ifndef NANDY_IOS_H
#define NANDY_IOS_H

#include "nandy_emu_tools.h"

extern FILE* tty_outstream;
extern FILE* tty_instream;
bool io_step_tty(cpu_state_t* cpu, bool active);

#endif