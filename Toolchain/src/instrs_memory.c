#include "nandy_instr_defs.h"
#include "nandy_parse_tools.h"
#include "nandy_alufuncs.h"

static void exe_ld(cpu_state_t* cpu) {
    addr_t addr = getMemAddr(cpu);
    cpu->acc = peek(cpu, addr);
    cpu->pc ++; cpu->elapsed += 2;
}
const instruction_t i_ld = {
    .mnemonic = "ld",
    .opcode_mask = IMM4_MASK | MMODE_MASK,
    .opcode = MEM_MASK | LOAD_MASK,
    .assemble = asm_mem,
    .disassemble = dis_mem,
    .execute = exe_ld
};

static void exe_st(cpu_state_t* cpu) {
    addr_t addr = getMemAddr(cpu);
    poke(cpu, addr, cpu->acc);
    cpu->pc ++; cpu->elapsed += 2;
}
const instruction_t i_st = {
    .mnemonic = "st",
    .opcode_mask = IMM4_MASK | MMODE_MASK,
    .opcode = MEM_MASK,
    .assemble = asm_mem,
    .disassemble = dis_mem,
    .execute = exe_st
};