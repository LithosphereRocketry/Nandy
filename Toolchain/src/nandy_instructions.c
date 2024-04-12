#include "nandy_instructions.h"
#include "nandy_instr_defs.h"
#include "stdio.h"

static const instruction_t* default_instrs[] = {
    &i_nop, 
    &i_rd,
    &i_wr,
    &i_sw,
    &i_ja,
    &i_jar,
    &i_jri,
    &i_brk,
    &i_bell,
    &i_dint,
    &i_eint,
    &i_iclr,
    &i_iset,
    &i__isp,
    &i_isp,
    &i_or,
    &i_and,
    &i_xor,
    &i_inv,
    &i_nor,
    &i_nand,
    &i_xnor,
    &i__addi,
    &i__addci,
    &i__subi,
    &i__subci
};

const ilist_t NANDY_ILIST = {
    .size = sizeof(default_instrs) / sizeof(instruction_t*),
    .list = default_instrs
};