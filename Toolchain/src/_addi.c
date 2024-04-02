#include "nandy_instr_defs.h"
#include "nandy_parse_tools.h"
#include <stdio.h>

static const char* asm__addi(const char* text, asm_state_t* state) {
    state->rom[state->rom_loc] = i__addi.opcode;
    const char* endptr = addUnresolved(state, text, resolveImm8);
    state->rom_loc += 2;
    return endptr;
}
static void exe__addi(cpu_state_t* cpu) {
    cpu->acc += peek(cpu, cpu->pc + 1);
    cpu->pc += 2;
}
static void dis__addi(const cpu_state_t* cpu, addr_t addr, char* buf, size_t len) {
    snprintf(buf, len, "_addi %hhi", peek(cpu, addr+1));
}

const instruction_t i__addi = {
    .mnemonic = "_addi",
    .opcode_mask = 0,
    .opcode = MULTICYCLE_MASK | ALU_SEL_MASK | ALU_ADD,
    .assemble = asm__addi,
    .disassemble = dis__addi,
    .execute = exe__addi
};