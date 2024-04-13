#include "nandy_instr_defs.h"
#include "nandy_parse_tools.h"

static void exe_rdi(cpu_state_t* cpu) {
    cpu->acc = peek(cpu, cpu->pc + 1);
    cpu->pc += 2;
}
const instruction_t i_rdi = {
    .mnemonic = "rdi",
    .opcode_mask = 0,
    .opcode = MULTICYCLE_MASK | ALU_SEL_MASK | ALU_B,
    .assemble = asm_alu_imm,
    .disassemble = dis_alu_imm,
    .execute = exe_rdi
};

static void exe_ori(cpu_state_t* cpu) {
    cpu->acc |= peek(cpu, cpu->pc + 1);
    cpu->pc += 2;
}
const instruction_t i_ori = {
    .mnemonic = "ori",
    .opcode_mask = 0,
    .opcode = MULTICYCLE_MASK | ALU_SEL_MASK | ALU_OR,
    .assemble = asm_alu_imm,
    .disassemble = dis_alu_imm,
    .execute = exe_ori
};

static void exe_andi(cpu_state_t* cpu) {
    cpu->acc &= peek(cpu, cpu->pc + 1);
    cpu->pc += 2;
}
const instruction_t i_andi = {
    .mnemonic = "andi",
    .opcode_mask = 0,
    .opcode = MULTICYCLE_MASK | ALU_SEL_MASK | ALU_AND,
    .assemble = asm_alu_imm,
    .disassemble = dis_alu_imm,
    .execute = exe_andi
};

static void exe_xori(cpu_state_t* cpu) {
    cpu->acc ^= peek(cpu, cpu->pc + 1);
    cpu->pc += 2;
}
const instruction_t i_xori = {
    .mnemonic = "xori",
    .opcode_mask = 0,
    .opcode = MULTICYCLE_MASK | ALU_SEL_MASK | ALU_XOR,
    .assemble = asm_alu_imm,
    .disassemble = dis_alu_imm,
    .execute = exe_xori
};

static void exe_invi(cpu_state_t* cpu) {
    cpu->acc = ~peek(cpu, cpu->pc + 1);
    cpu->pc += 2;
}
const instruction_t i_invi = {
    .mnemonic = "invi",
    .opcode_mask = 0,
    .opcode = MULTICYCLE_MASK | ALU_SEL_MASK | ALU_NB,
    .assemble = asm_alu_imm,
    .disassemble = dis_alu_imm,
    .execute = exe_invi
};

static void exe_nori(cpu_state_t* cpu) {
    cpu->acc = ~(cpu->acc | getALUReg(cpu));
    cpu->pc ++;
}
const instruction_t i_nori = {
    .mnemonic = "nori",
    .opcode_mask = 0,
    .opcode = MULTICYCLE_MASK | ALU_SEL_MASK | ALU_NOR,
    .assemble = asm_alu_imm,
    .disassemble = dis_alu_imm,
    .execute = exe_nori
};

static void exe_nandi(cpu_state_t* cpu) {
    cpu->acc = ~(cpu->acc & getALUReg(cpu));
    cpu->pc ++;
}
const instruction_t i_nandi = {
    .mnemonic = "nandi",
    .opcode_mask = 0,
    .opcode = MULTICYCLE_MASK | ALU_SEL_MASK | ALU_NAND,
    .assemble = asm_alu_imm,
    .disassemble = dis_alu_imm,
    .execute = exe_nandi
};

static void exe_xnori(cpu_state_t* cpu) {
    cpu->acc = ~(cpu->acc ^ getALUReg(cpu));
    cpu->pc ++;
}
const instruction_t i_xnori = {
    .mnemonic = "xnori",
    .opcode_mask = 0,
    .opcode = MULTICYCLE_MASK | ALU_SEL_MASK | ALU_XNOR,
    .assemble = asm_alu_imm,
    .disassemble = dis_alu_imm,
    .execute = exe_xnori
};