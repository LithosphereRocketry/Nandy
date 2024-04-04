#include "nandy_instr_defs.h"
#include "nandy_parse_tools.h"
#include <stdio.h>

static const char* asm_or(const char* text, asm_state_t* state) {
    return asm_alu_reg(text, state, ALU_OR);
}
static void exe_or(cpu_state_t* cpu) {
    cpu->acc += peek(cpu, cpu->pc + 1);
    cpu->pc += 2;
}
static void dis_or(const cpu_state_t* cpu, addr_t addr, char* buf, size_t len) {
    snprintf(buf, len, "or %s", regnames[peek(cpu, cpu->pc) & XY_MASK ? REG_DY : REG_DX]);
}

const instruction_t i_or = {
    .mnemonic = "or",
    .opcode_mask = XY_MASK,
    .opcode = ALU_SEL_MASK | ALU_OR,
    .assemble = asm_or,
    .disassemble = dis_or,
    .execute = exe_or
};

static const char* asm_and(const char* text, asm_state_t* state) {
    return asm_alu_reg(text, state, ALU_AND);
}
static void exe_and(cpu_state_t* cpu) {
    cpu->acc += peek(cpu, cpu->pc + 1);
    cpu->pc += 2;
}
static void dis_and(const cpu_state_t* cpu, addr_t addr, char* buf, size_t len) {
    snprintf(buf, len, "and %s", regnames[peek(cpu, cpu->pc) & XY_MASK ? REG_DY : REG_DX]);
}

const instruction_t i_and = {
    .mnemonic = "and",
    .opcode_mask = XY_MASK,
    .opcode = ALU_SEL_MASK | ALU_AND,
    .assemble = asm_and,
    .disassemble = dis_and,
    .execute = exe_and
};

static const char* asm_xor(const char* text, asm_state_t* state) {
    return asm_alu_reg(text, state, ALU_XOR);
}
static void exe_xor(cpu_state_t* cpu) {
    cpu->acc += peek(cpu, cpu->pc + 1);
    cpu->pc += 2;
}
static void dis_xor(const cpu_state_t* cpu, addr_t addr, char* buf, size_t len) {
    snprintf(buf, len, "xor %s", regnames[peek(cpu, cpu->pc) & XY_MASK ? REG_DY : REG_DX]);
}

const instruction_t i_xor = {
    .mnemonic = "xor",
    .opcode_mask = XY_MASK,
    .opcode = ALU_SEL_MASK | ALU_XOR,
    .assemble = asm_xor,
    .disassemble = dis_xor,
    .execute = exe_xor
};

static const char* asm_inv(const char* text, asm_state_t* state) {
    return asm_alu_reg(text, state, ALU_NB);
}
static void exe_inv(cpu_state_t* cpu) {
    cpu->acc += peek(cpu, cpu->pc + 1);
    cpu->pc += 2;
}
static void dis_inv(const cpu_state_t* cpu, addr_t addr, char* buf, size_t len) {
    snprintf(buf, len, "inv %s", regnames[peek(cpu, cpu->pc) & XY_MASK ? REG_DY : REG_DX]);
}

const instruction_t i_inv = {
    .mnemonic = "inv",
    .opcode_mask = XY_MASK,
    .opcode = ALU_SEL_MASK | ALU_NB,
    .assemble = asm_inv,
    .disassemble = dis_inv,
    .execute = exe_inv
};

static const char* asm_nor(const char* text, asm_state_t* state) {
    return asm_alu_reg(text, state, ALU_NOR);
}
static void exe_nor(cpu_state_t* cpu) {
    cpu->acc += peek(cpu, cpu->pc + 1);
    cpu->pc += 2;
}
static void dis_nor(const cpu_state_t* cpu, addr_t addr, char* buf, size_t len) {
    snprintf(buf, len, "nor %s", regnames[peek(cpu, cpu->pc) & XY_MASK ? REG_DY : REG_DX]);
}

const instruction_t i_nor = {
    .mnemonic = "nor",
    .opcode_mask = XY_MASK,
    .opcode = ALU_SEL_MASK | ALU_NOR,
    .assemble = asm_nor,
    .disassemble = dis_nor,
    .execute = exe_nor
};

static const char* asm_nand(const char* text, asm_state_t* state) {
    return asm_alu_reg(text, state, ALU_NAND);
}
static void exe_nand(cpu_state_t* cpu) {
    cpu->acc += peek(cpu, cpu->pc + 1);
    cpu->pc += 2;
}
static void dis_nand(const cpu_state_t* cpu, addr_t addr, char* buf, size_t len) {
    snprintf(buf, len, "nand %s", regnames[peek(cpu, cpu->pc) & XY_MASK ? REG_DY : REG_DX]);
}

const instruction_t i_nand = {
    .mnemonic = "nand",
    .opcode_mask = XY_MASK,
    .opcode = ALU_SEL_MASK | ALU_NAND,
    .assemble = asm_nand,
    .disassemble = dis_nand,
    .execute = exe_nand
};

static const char* asm_xnor(const char* text, asm_state_t* state) {
    return asm_alu_reg(text, state, ALU_XNOR);
}
static void exe_xnor(cpu_state_t* cpu) {
    cpu->acc += peek(cpu, cpu->pc + 1);
    cpu->pc += 2;
}
static void dis_xnor(const cpu_state_t* cpu, addr_t addr, char* buf, size_t len) {
    snprintf(buf, len, "xnor %s", regnames[peek(cpu, cpu->pc) & XY_MASK ? REG_DY : REG_DX]);
}

const instruction_t i_xnor = {
    .mnemonic = "xnor",
    .opcode_mask = XY_MASK,
    .opcode = ALU_SEL_MASK | ALU_XNOR,
    .assemble = asm_xnor,
    .disassemble = dis_xnor,
    .execute = exe_xnor
};