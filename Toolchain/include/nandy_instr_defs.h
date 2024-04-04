#ifndef NANDY_INSTR_DEFS_H
#define NANDY_INSTR_DEFS_H

#include "nandy_tools.h"

// register
extern const instruction_t i_nop, i_rd, i_wr, i_sw;
// jump
extern const instruction_t i_ja, i_jar, i_jri;
// immadd
extern const instruction_t i__addi;

#endif