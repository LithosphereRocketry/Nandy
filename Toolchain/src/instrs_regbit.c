#include "nandy_instr_defs.h"
#include "nandy_parse_tools.h"
#include <stdio.h>

static void exe_or(cpu_state_t* cpu) {
    cpu->acc |= getALUReg(cpu);
    cpu->pc ++; cpu->elapsed ++;
}
const instruction_t i_or = {
    .mnemonic = "or",
    .opcode_mask = XY_MASK,
    .opcode = ALU_SEL_MASK | ALU_OR,
    .assemble = asm_alu_reg,
    .disassemble = dis_alu_reg,
    .execute = exe_or
};

static void exe_and(cpu_state_t* cpu) {
    cpu->acc &= getALUReg(cpu);
    cpu->pc ++; cpu->elapsed ++;
}
const instruction_t i_and = {
    .mnemonic = "and",
    .opcode_mask = XY_MASK,
    .opcode = ALU_SEL_MASK | ALU_AND,
    .assemble = asm_alu_reg,
    .disassemble = dis_alu_reg,
    .execute = exe_and
};

static void exe_xor(cpu_state_t* cpu) {
    cpu->acc ^= getALUReg(cpu);
    cpu->pc ++; cpu->elapsed ++;
}
const instruction_t i_xor = {
    .mnemonic = "xor",
    .opcode_mask = XY_MASK,
    .opcode = ALU_SEL_MASK | ALU_XOR,
    .assemble = asm_alu_reg,
    .disassemble = dis_alu_reg,
    .execute = exe_xor
};