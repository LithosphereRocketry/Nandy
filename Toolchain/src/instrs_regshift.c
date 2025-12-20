#include "nandy_instr_defs.h"
#include "nandy_parse_tools.h"
#include "nandy_alufuncs.h"

static void exe_sl(cpu_state_t* cpu) {
    cpu->acc = alu_sl(cpu->acc, 0, false, NULL);
    cpu->pc ++; cpu->elapsed ++;
}
const instruction_t i_sl = {
    .mnemonic = "sl",
    .opcode_mask = XY_MASK,
    .opcode = ALU_SEL_MASK | ALU_SL,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe_sl
};

static void exe_slc(cpu_state_t* cpu) {
    cpu->acc = alu_sl(cpu->acc, 0, cpu->carry, &cpu->carry);
    cpu->pc ++; cpu->elapsed ++;
}
const instruction_t i_slc = {
    .mnemonic = "slc",
    .opcode_mask = XY_MASK,
    .opcode = ALU_SEL_MASK | CARRY_SEL_MASK | ALU_SL,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe_slc
};

static void exe_sr(cpu_state_t* cpu) {
    cpu->acc = alu_sr(cpu->acc, 0, false, NULL);
    cpu->pc ++; cpu->elapsed ++;
}
const instruction_t i_sr = {
    .mnemonic = "sr",
    .opcode_mask = XY_MASK,
    .opcode = ALU_SEL_MASK | ALU_SR,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe_sr
};

static void exe_src(cpu_state_t* cpu) {
    cpu->acc = alu_sr(cpu->acc, 0, cpu->carry, &cpu->carry);
    cpu->pc ++; cpu->elapsed ++;
}
const instruction_t i_src = {
    .mnemonic = "src",
    .opcode_mask = XY_MASK,
    .opcode = ALU_SEL_MASK | CARRY_SEL_MASK | ALU_SR,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe_src
};
