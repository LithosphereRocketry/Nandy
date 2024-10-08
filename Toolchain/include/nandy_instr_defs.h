#ifndef NANDY_INSTR_DEFS_H
#define NANDY_INSTR_DEFS_H

#include "nandy_tools.h"

// macros
extern const instruction_t i_macro_echo, i_macro_define, i_macro_assert;
extern const instruction_t i_macro_loc, i_macro_memloc, i_macro_static;
extern const instruction_t i_macro_string; // align, memalign, include
// temp macros
extern const instruction_t i_rda, i_call, i_goto, i_move; 
// register
extern const instruction_t i_nop, i_rd, i_wr, i_sw;
// jump
extern const instruction_t i_ja, i_jar, i_jri, i_j, i_jcz;
// flags
extern const instruction_t i_brk, i_bell, i_csset, i_csclr, i_dint, i_eint, i_iclr, i_iset;
// regbit
extern const instruction_t i_or, i_and, i_xor;
// regadd
extern const instruction_t i__add, i__addc, i__sub, i__subc;
extern const instruction_t i_add, i_addc, i_sub, i_subc;
// regshift
extern const instruction_t i__sl, i__slc, i__sla, i__slr, i__sr, i__src, i__srr, i__sra;
extern const instruction_t i_sl, i_slc, i_sla, i_slr, i_sr, i_src, i_srr, i_sra;
// regcomp
extern const instruction_t i_ctog, i_cset, i_cclr, i_zero, i_nzero, i_par, i_npar;
// immbit
extern const instruction_t i_rdi, i_ori, i_andi, i_xori;
// immadd
extern const instruction_t i__addi, i__addci, i__subi, i__subci;
extern const instruction_t i_addi, i_addci, i_subi, i_subci;
// memory
extern const instruction_t i__isp, i_isp, i_lda, i_lds, i_stra, i_strs;

#endif