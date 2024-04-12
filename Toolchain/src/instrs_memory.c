#include "nandy_instr_defs.h"
#include "nandy_parse_tools.h"
#include "nandy_alufuncs.h"
#include <stdio.h>

static void exe__isp(cpu_state_t* cpu) {
    cpu->sp = alu_add(cpu->sp, signExtend(peek(cpu, cpu->pc), 4), false, NULL);
    cpu->pc ++;
}
const instruction_t i__isp = {
    .mnemonic = "_isp",
    .opcode_mask = IMM4_MASK,
    .opcode = ISP_MASK,
    .assemble = asm_imm4s,
    .disassemble = dis_imm4s,
    .execute = exe__isp
};

static void exe_isp(cpu_state_t* cpu) {
    cpu->sp = alu_add(cpu->sp, signExtend(peek(cpu, cpu->pc), 4), false, &cpu->carry);
    cpu->pc ++;
}
const instruction_t i_isp = {
    .mnemonic = "isp",
    .opcode_mask = IMM4_MASK,
    .opcode = ISP_MASK | CARRY_SEL_MASK,
    .assemble = asm_imm4s,
    .disassemble = dis_imm4s,
    .execute = exe_isp
};