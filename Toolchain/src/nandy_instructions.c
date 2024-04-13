#include "nandy_instructions.h"
#include "nandy_instr_defs.h"
#include "stdio.h"

static const instruction_t* default_instrs[] = {
    &i_macro_echo,
    &i_macro_define,

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
    &i__add,
    &i__addc,
    &i__sub,
    &i__subc,
    &i__sl,
    &i__slc,
    &i__sla,
    &i__slr,
    &i__sr,
    &i__src,
    &i__srr,
    &i__sra,
    &i_add,
    &i_addc,
    &i_sub,
    &i_subc,
    &i_sl,
    &i_slc,
    &i_sla,
    &i_slr,
    &i_sr,
    &i_src,
    &i_srr,
    &i_sra,
    &i__addi,
    &i__addci,
    &i__subi,
    &i__subci
};

const ilist_t NANDY_ILIST = {
    .size = sizeof(default_instrs) / sizeof(instruction_t*),
    .list = default_instrs
};