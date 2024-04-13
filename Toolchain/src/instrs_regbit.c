#include "nandy_instr_defs.h"
#include "nandy_parse_tools.h"
#include <stdio.h>

static void exe_or(cpu_state_t* cpu) {
    cpu->acc |= getALUReg(cpu);
    cpu->pc ++;
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
    cpu->pc ++;
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
    cpu->pc ++;
}
const instruction_t i_xor = {
    .mnemonic = "xor",
    .opcode_mask = XY_MASK,
    .opcode = ALU_SEL_MASK | ALU_XOR,
    .assemble = asm_alu_reg,
    .disassemble = dis_alu_reg,
    .execute = exe_xor
};

static void exe_inv(cpu_state_t* cpu) {
    cpu->acc = ~getALUReg(cpu);
    cpu->pc ++;
}
const instruction_t i_inv = {
    .mnemonic = "inv",
    .opcode_mask = XY_MASK,
    .opcode = ALU_SEL_MASK | ALU_NB,
    .assemble = asm_alu_reg,
    .disassemble = dis_alu_reg,
    .execute = exe_inv
};

static void exe_nor(cpu_state_t* cpu) {
    cpu->acc = ~(cpu->acc | getALUReg(cpu));
    cpu->pc ++;
}
const instruction_t i_nor = {
    .mnemonic = "nor",
    .opcode_mask = XY_MASK,
    .opcode = ALU_SEL_MASK | ALU_NOR,
    .assemble = asm_alu_reg,
    .disassemble = dis_alu_reg,
    .execute = exe_nor
};

static void exe_nand(cpu_state_t* cpu) {
    cpu->acc = ~(cpu->acc & getALUReg(cpu));
    cpu->pc ++;
}
const instruction_t i_nand = {
    .mnemonic = "nand",
    .opcode_mask = XY_MASK,
    .opcode = ALU_SEL_MASK | ALU_NAND,
    .assemble = asm_alu_reg,
    .disassemble = dis_alu_reg,
    .execute = exe_nand
};

static void exe_xnor(cpu_state_t* cpu) {
    cpu->acc = ~(cpu->acc ^ getALUReg(cpu));
    cpu->pc ++;
}
const instruction_t i_xnor = {
    .mnemonic = "xnor",
    .opcode_mask = XY_MASK,
    .opcode = ALU_SEL_MASK | ALU_XNOR,
    .assemble = asm_alu_reg,
    .disassemble = dis_alu_reg,
    .execute = exe_xnor
};