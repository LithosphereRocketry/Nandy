#include "nandy_instr_defs.h"
#include "nandy_parse_tools.h"
#include <stdio.h>

static void exe__isp(cpu_state_t* cpu) {
    
    cpu->pc ++;
}
const instruction_t i__isp = {
    .mnemonic = "nop",
    .opcode_mask = REGID_MASK,
    .opcode = 0,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe__isp
};
