#ifndef NANDY_IOS_H
#define NANDY_IOS_H

#include "nandy_emu_tools.h"

bool io_step_tty(cpu_state_t* cpu, bool active);
bool io_step_compactflash(cpu_state_t* cpu, bool active);

#endif