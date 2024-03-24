#include "nandy_instructions.h"
#include "nandy_instr_defs.h"
#include "stdio.h"

static const instruction_t* default_instrs[] = {
    &i_nop, 
    &i_rd,
    &i_wr,
    &i__addi
};

const ilist_t NANDY_ILIST = {
    .size = sizeof(default_instrs) / sizeof(instruction_t*),
    .list = default_instrs
};