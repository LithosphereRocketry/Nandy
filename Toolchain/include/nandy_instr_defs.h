#ifndef NANDY_INSTR_DEFS_H
#define NANDY_INSTR_DEFS_H

#include "nandy_tools.h"

// register
extern const instruction_t i_nop, i_rd, i_wr, i_sw;
// jump
extern const instruction_t i_ja, i_jar, i_jri;
// flags
extern const instruction_t i_brk, i_bell, i_dint, i_eint, i_iclr, i_iset;
// regbit
extern const instruction_t i_or, i_and, i_xor, i_inv, i_nor, i_nand, i_xnor;
// immadd
extern const instruction_t i__addi;

#endif