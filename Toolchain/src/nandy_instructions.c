#include "nandy_instructions.h"
#include "nandy_instr_defs.h"
#include "stdio.h"

static const instruction_t* default_instrs[] = {
    &i_macro_echo,
    &i_macro_define,
    &i_macro_assert,
    &i_macro_loc,
    &i_macro_memloc,
    &i_macro_static,
    &i_macro_string,

    &i_isc,
    &i_isp,
    &i_rdp,
    &i_call,
    &i_goto,
    &i_move,
    &i_swap,
    &i_pusha,

    &i_rd,
    &i_wr,
    &i_sw,
    &i_jp,
    &i_jpr,
    &i_jri,
    &i_brk,
    &i_dint,
    &i_eint,
    &i_ipoll,
    &i_or,
    &i_and,
    &i_xor,
    &i_cclr,
    &i_nzero,
    &i_sgn,
    &i_cset,
    &i_ctog,
    &i_zero,
    &i_nsgn,
    &i_add,
    &i_adc,
    &i_sub,
    &i_sbc,
    &i_sl,
    &i_slc,
    &i_sr,
    &i_src,
    &i_ld,
    &i_st,
    &i_rdi,
    &i_ori,
    &i_andi,
    &i_xori,
    &i_addi,
    &i_adci,
    &i_subi,
    &i_sbci,
    &i_j,
    &i_jc
};

const ilist_t NANDY_ILIST = {
    .size = sizeof(default_instrs) / sizeof(instruction_t*),
    .list = default_instrs
};