#include "nandy_instr_defs.h"
#include "nandy_parse_tools.h"
#include "nandy_alufuncs.h"
#include <stdio.h>

static void exe_addi(cpu_state_t* cpu) {
    cpu->acc = alu_add(cpu->acc, peek(cpu, cpu->pc + 1), false, NULL);
    cpu->pc += 2; cpu->elapsed += 2;
}
const instruction_t i_addi = {
    .mnemonic = "addi",
    .opcode_mask = XY_MASK,
    .opcode = MULTIBYTE_MASK | ALU_SEL_MASK | ALU_ADD,
    .assemble = asm_alu_imm,
    .disassemble = dis_alu_imm,
    .execute = exe_addi
};

static void exe_adci(cpu_state_t* cpu) {
    cpu->acc = alu_add(cpu->acc, peek(cpu, cpu->pc + 1), cpu->carry, &cpu->carry);
    cpu->pc += 2; cpu->elapsed += 2;
}
const instruction_t i_adci = {
    .mnemonic = "adci",
    .opcode_mask = XY_MASK,
    .opcode = MULTIBYTE_MASK | ALU_SEL_MASK | CARRY_SEL_MASK | ALU_ADD,
    .assemble = asm_alu_imm,
    .disassemble = dis_alu_imm,
    .execute = exe_adci
};

static void exe_subi(cpu_state_t* cpu) {
    cpu->acc = alu_sub(cpu->acc, peek(cpu, cpu->pc + 1), true, NULL);
    cpu->pc += 2; cpu->elapsed += 2;
}
const instruction_t i_subi = {
    .mnemonic = "subi",
    .opcode_mask = XY_MASK,
    .opcode = MULTIBYTE_MASK | ALU_SEL_MASK | ALU_SUB,
    .assemble = asm_alu_imm,
    .disassemble = dis_alu_imm,
    .execute = exe_subi
};

static void exe_sbci(cpu_state_t* cpu) {
    cpu->acc = alu_sub(cpu->acc, peek(cpu, cpu->pc + 1), cpu->carry, &cpu->carry);
    cpu->pc += 2; cpu->elapsed += 2;
}
const instruction_t i_sbci = {
    .mnemonic = "sbci",
    .opcode_mask = XY_MASK,
    .opcode = MULTIBYTE_MASK | ALU_SEL_MASK | CARRY_SEL_MASK | ALU_SUB,
    .assemble = asm_alu_imm,
    .disassemble = dis_alu_imm,
    .execute = exe_sbci
};