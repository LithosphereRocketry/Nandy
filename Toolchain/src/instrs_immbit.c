#include "nandy_instr_defs.h"
#include "nandy_parse_tools.h"

static void exe_rdi(cpu_state_t* cpu) {
    cpu->acc = peek(cpu, cpu->pc + 1);
    cpu->pc += 2; cpu->elapsed += 2;
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
    cpu->pc += 2; cpu->elapsed += 2;
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
    cpu->pc += 2; cpu->elapsed += 2;
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
    cpu->pc += 2; cpu->elapsed += 2;
}
const instruction_t i_xori = {
    .mnemonic = "xori",
    .opcode_mask = 0,
    .opcode = MULTICYCLE_MASK | ALU_SEL_MASK | ALU_XOR,
    .assemble = asm_alu_imm,
    .disassemble = dis_alu_imm,
    .execute = exe_xori
};