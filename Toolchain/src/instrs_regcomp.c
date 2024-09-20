#include "nandy_instr_defs.h"
#include "nandy_parse_tools.h"
#include "nandy_alufuncs.h"

static void exe_cclr(cpu_state_t* cpu) {
    cpu->carry = false;
    cpu->pc ++; cpu->elapsed ++;
}
const instruction_t i_cclr = {
    .mnemonic = "cclr",
    .opcode_mask = 0,
    .opcode = ALU_SEL_MASK | CARRY_SEL_MASK | ALU_B,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe_cclr
};

static void exe_nzero(cpu_state_t* cpu) {
    cpu->carry = cpu->acc != 0;
    cpu->pc ++; cpu->elapsed ++;
}
const instruction_t i_nzero = {
    .mnemonic = "nzero",
    .opcode_mask = 0,
    .opcode = ALU_SEL_MASK | CARRY_SEL_MASK | ALU_AND,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe_nzero
};

static void exe_par(cpu_state_t* cpu) {
    cpu->carry = parity(cpu->acc);
    cpu->pc ++; cpu->elapsed ++;
}
const instruction_t i_par = {
    .mnemonic = "par",
    .opcode_mask = 0,
    .opcode = ALU_SEL_MASK | CARRY_SEL_MASK | ALU_XOR,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe_par
};

static void exe_cset(cpu_state_t* cpu) {
    cpu->carry = (cpu->acc & 0x80) == 0;
    cpu->pc ++; cpu->elapsed ++;
}
const instruction_t i_cset = {
    .mnemonic = "cset",
    .opcode_mask = 0,
    .opcode = ALU_SEL_MASK | XY_MASK | CARRY_SEL_MASK | ALU_B,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe_cset
};

static void exe_ctog(cpu_state_t* cpu) {
    cpu->carry = !cpu->carry;
    cpu->pc ++; cpu->elapsed ++;
}
const instruction_t i_ctog = {
    .mnemonic = "ctog",
    .opcode_mask = 0,
    .opcode = ALU_SEL_MASK | XY_MASK | CARRY_SEL_MASK | ALU_OR,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe_ctog
};

static void exe_zero(cpu_state_t* cpu) {
    cpu->carry = cpu->acc == 0;
    cpu->pc ++; cpu->elapsed ++;
}
const instruction_t i_zero = {
    .mnemonic = "zero",
    .opcode_mask = 0,
    .opcode = ALU_SEL_MASK | XY_MASK | CARRY_SEL_MASK | ALU_AND,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe_zero
};

static void exe_npar(cpu_state_t* cpu) {
    cpu->carry = !parity(cpu->acc);
    cpu->pc ++; cpu->elapsed ++;
}
const instruction_t i_npar = {
    .mnemonic = "npar",
    .opcode_mask = 0,
    .opcode = ALU_SEL_MASK | XY_MASK | CARRY_SEL_MASK | ALU_XOR,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe_npar
};