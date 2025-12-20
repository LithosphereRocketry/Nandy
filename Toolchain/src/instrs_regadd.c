#include "nandy_instr_defs.h"
#include "nandy_parse_tools.h"
#include "nandy_alufuncs.h"
#include "shuntingyard.h"
#include <stdio.h>

static void exe_add(cpu_state_t* cpu) {
    cpu->acc = alu_add(cpu->acc, getALUReg(cpu), false, NULL);
    cpu->pc ++; cpu->elapsed ++;
}
const instruction_t i_add = {
    .mnemonic = "add",
    .opcode_mask = XY_MASK,
    .opcode = ALU_SEL_MASK | CARRY_SEL_MASK | ALU_ADD,
    .assemble = asm_alu_reg,
    .disassemble = dis_alu_reg,
    .execute = exe_add
};

static void exe_adc(cpu_state_t* cpu) {
    cpu->acc = alu_add(cpu->acc, getALUReg(cpu), cpu->carry, &cpu->carry);
    cpu->pc ++; cpu->elapsed ++;
}
const instruction_t i_adc = {
    .mnemonic = "adc",
    .opcode_mask = XY_MASK,
    .opcode = ALU_SEL_MASK | CARRY_SEL_MASK | ALU_ADD,
    .assemble = asm_alu_reg,
    .disassemble = dis_alu_reg,
    .execute = exe_adc
};

static void exe_sub(cpu_state_t* cpu) {
    cpu->acc = alu_sub(cpu->acc, getALUReg(cpu), true, NULL);
    cpu->pc ++; cpu->elapsed ++;
}
const instruction_t i_sub = {
    .mnemonic = "sub",
    .opcode_mask = XY_MASK,
    .opcode = ALU_SEL_MASK | CARRY_SEL_MASK | ALU_SUB,
    .assemble = asm_alu_reg,
    .disassemble = dis_alu_reg,
    .execute = exe_sub
};

static void exe_sbc(cpu_state_t* cpu) {
    cpu->acc = alu_sub(cpu->acc, getALUReg(cpu), cpu->carry, &cpu->carry);
    cpu->pc ++; cpu->elapsed ++;
}
const instruction_t i_sbc = {
    .mnemonic = "sbc",
    .opcode_mask = XY_MASK,
    .opcode = ALU_SEL_MASK | CARRY_SEL_MASK | ALU_SUB,
    .assemble = asm_alu_reg,
    .disassemble = dis_alu_reg,
    .execute = exe_sbc
};