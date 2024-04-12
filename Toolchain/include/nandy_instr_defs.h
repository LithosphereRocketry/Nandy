#ifndef NANDY_INSTR_DEFS_H
#define NANDY_INSTR_DEFS_H

#include "nandy_tools.h"

// macros
extern const instruction_t i_macro_echo, i_macro_define;
// register
extern const instruction_t i_nop, i_rd, i_wr, i_sw;
// jump
extern const instruction_t i_ja, i_jar, i_jri;
// flags
extern const instruction_t i_brk, i_bell, i_dint, i_eint, i_iclr, i_iset;
// regbit
extern const instruction_t i_or, i_and, i_xor, i_inv, i_nor, i_nand, i_xnor;
// regadd
extern const instruction_t i__add, i__addc, i__sub, i__subc;
// immadd
extern const instruction_t i__addi, i__addci, i__subi, i__subci;
// memory
extern const instruction_t i__isp, i_isp;

#endif