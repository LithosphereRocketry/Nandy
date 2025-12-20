#ifndef NANDY_INSTR_DEFS_H
#define NANDY_INSTR_DEFS_H

#include "nandy_tools.h"

// macros
extern const instruction_t i_macro_echo, i_macro_define, i_macro_assert;
extern const instruction_t i_macro_loc, i_macro_memloc, i_macro_static;
extern const instruction_t i_macro_string; // align, memalign, include
// temp macros
extern const instruction_t i_isc, i_isp, i_rda, i_call, i_goto, i_move, i_pusha;
// register
extern const instruction_t i_rd, i_wr, i_sw;
// jump
extern const instruction_t i_jp, i_jpr, i_jri, i_j, i_jc;
// flags
extern const instruction_t i_brk, i_int, i_dint, i_eint;
// regbit
extern const instruction_t i_ipoll, i_or, i_and, i_xor;
// regadd
extern const instruction_t i_add, i_adc, i_sub, i_sbc;
// regshift
extern const instruction_t i_sl, i_slc, i_sr, i_src;
// regcomp
extern const instruction_t i_ctog, i_cset, i_cclr, i_zero, i_nzero, i_sgn, i_nsgn;
// immbit
extern const instruction_t i_rdi, i_ori, i_andi, i_xori;
// immadd
extern const instruction_t i_addi, i_adci, i_subi, i_sbci;
// memory
extern const instruction_t i_ld, i_st;

#endif