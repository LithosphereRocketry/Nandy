#include "nandy_instr_defs.h"
#include "nandy_parse_tools.h"
#include "nandy_alufuncs.h"
#include <stdio.h>

static void exe__add(cpu_state_t* cpu) {
    cpu->acc = alu_add(cpu->acc, getALUReg(cpu), false, NULL);
    cpu->pc ++;
}
const instruction_t i__add = {
    .mnemonic = "_add",
    .opcode_mask = XY_MASK,
    .opcode = ALU_SEL_MASK | ALU_ADD,
    .assemble = asm_alu_reg,
    .disassemble = dis_alu_reg,
    .execute = exe__add
};

static void exe__addc(cpu_state_t* cpu) {
    cpu->acc = alu_add(cpu->acc, getALUReg(cpu), cpu->carry, NULL);
    cpu->pc ++;
}
const instruction_t i__addc = {
    .mnemonic = "_addc",
    .opcode_mask = XY_MASK,
    .opcode = ALU_SEL_MASK | ALU_ADDC,
    .assemble = asm_alu_reg,
    .disassemble = dis_alu_reg,
    .execute = exe__addc
};

static void exe__sub(cpu_state_t* cpu) {
    cpu->acc = alu_sub(cpu->acc, getALUReg(cpu), true, NULL);
    cpu->pc ++;
}
const instruction_t i__sub = {
    .mnemonic = "_sub",
    .opcode_mask = XY_MASK,
    .opcode = ALU_SEL_MASK | ALU_SUB,
    .assemble = asm_alu_reg,
    .disassemble = dis_alu_reg,
    .execute = exe__sub
};

static void exe__subc(cpu_state_t* cpu) {
    cpu->acc = alu_sub(cpu->acc, getALUReg(cpu), cpu->carry, NULL);
    cpu->pc ++;
}
const instruction_t i__subc = {
    .mnemonic = "_subc",
    .opcode_mask = XY_MASK,
    .opcode = ALU_SEL_MASK | ALU_SUBC,
    .assemble = asm_alu_reg,
    .disassemble = dis_alu_reg,
    .execute = exe__subc
};