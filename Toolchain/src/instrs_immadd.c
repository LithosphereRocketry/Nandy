#include "nandy_instr_defs.h"
#include "nandy_parse_tools.h"
#include "nandy_alufuncs.h"
#include <stdio.h>

static void exe__addi(cpu_state_t* cpu) {
    cpu->acc = alu_add(cpu->acc, peek(cpu, cpu->pc + 1), false, NULL);
    cpu->pc += 2; cpu->elapsed += 2;
}
const instruction_t i__addi = {
    .mnemonic = "_addi",
    .opcode_mask = 0,
    .opcode = MULTICYCLE_MASK | ALU_SEL_MASK | ALU_ADD,
    .assemble = asm_alu_imm,
    .disassemble = dis_alu_imm,
    .execute = exe__addi
};

static void exe__addci(cpu_state_t* cpu) {
    cpu->acc = alu_add(cpu->acc, peek(cpu, cpu->pc + 1), cpu->carry, NULL);
    cpu->pc += 2; cpu->elapsed += 2;
}
const instruction_t i__addci = {
    .mnemonic = "_addci",
    .opcode_mask = 0,
    .opcode = MULTICYCLE_MASK | ALU_SEL_MASK | ALU_ADDC,
    .assemble = asm_alu_imm,
    .disassemble = dis_alu_imm,
    .execute = exe__addci
};

static void exe__subi(cpu_state_t* cpu) {
    cpu->acc = alu_sub(cpu->acc, peek(cpu, cpu->pc + 1), true, NULL);
    cpu->pc += 2; cpu->elapsed += 2;
}
const instruction_t i__subi = {
    .mnemonic = "_subi",
    .opcode_mask = 0,
    .opcode = MULTICYCLE_MASK | ALU_SEL_MASK | ALU_SUB,
    .assemble = asm_alu_imm,
    .disassemble = dis_alu_imm,
    .execute = exe__subi
};

static void exe__subci(cpu_state_t* cpu) {
    cpu->acc = alu_sub(cpu->acc, peek(cpu, cpu->pc + 1), cpu->carry, NULL);
    cpu->pc += 2; cpu->elapsed += 2;
}
const instruction_t i__subci = {
    .mnemonic = "_subci",
    .opcode_mask = 0,
    .opcode = MULTICYCLE_MASK | ALU_SEL_MASK | ALU_SUBC,
    .assemble = asm_alu_imm,
    .disassemble = dis_alu_imm,
    .execute = exe__subci
};

static void exe_addi(cpu_state_t* cpu) {
    cpu->acc = alu_add(cpu->acc, peek(cpu, cpu->pc + 1), false, &cpu->carry);
    cpu->pc += 2; cpu->elapsed += 2;
}
const instruction_t i_addi = {
    .mnemonic = "addi",
    .opcode_mask = 0,
    .opcode = MULTICYCLE_MASK | ALU_SEL_MASK | CARRY_SEL_MASK | ALU_ADD,
    .assemble = asm_alu_imm,
    .disassemble = dis_alu_imm,
    .execute = exe_addi
};

static void exe_addci(cpu_state_t* cpu) {
    cpu->acc = alu_add(cpu->acc, peek(cpu, cpu->pc + 1), cpu->carry, &cpu->carry);
    cpu->pc += 2; cpu->elapsed += 2;
}
const instruction_t i_addci = {
    .mnemonic = "addci",
    .opcode_mask = 0,
    .opcode = MULTICYCLE_MASK | ALU_SEL_MASK | CARRY_SEL_MASK | ALU_ADDC,
    .assemble = asm_alu_imm,
    .disassemble = dis_alu_imm,
    .execute = exe_addci
};

static void exe_subi(cpu_state_t* cpu) {
    cpu->acc = alu_sub(cpu->acc, peek(cpu, cpu->pc + 1), true, &cpu->carry);
    cpu->pc += 2; cpu->elapsed += 2;
}
const instruction_t i_subi = {
    .mnemonic = "subi",
    .opcode_mask = 0,
    .opcode = MULTICYCLE_MASK | ALU_SEL_MASK | CARRY_SEL_MASK | ALU_SUB,
    .assemble = asm_alu_imm,
    .disassemble = dis_alu_imm,
    .execute = exe_subi
};

static void exe_subci(cpu_state_t* cpu) {
    cpu->acc = alu_sub(cpu->acc, peek(cpu, cpu->pc + 1), cpu->carry, &cpu->carry);
    cpu->pc += 2; cpu->elapsed += 2;
}
const instruction_t i_subci = {
    .mnemonic = "subci",
    .opcode_mask = 0,
    .opcode = MULTICYCLE_MASK | ALU_SEL_MASK | CARRY_SEL_MASK | ALU_SUBC,
    .assemble = asm_alu_imm,
    .disassemble = dis_alu_imm,
    .execute = exe_subci
};