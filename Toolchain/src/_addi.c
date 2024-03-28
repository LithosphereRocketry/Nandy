#include "nandy_instr_defs.h"
#include <stdio.h>


static const char* asm__addi(const char* text, asm_state_t* state) {
    state->rom[state->rom_loc] = 0;
    state->rom_loc ++;
    return text;
}
static void exe__addi(cpu_state_t* cpu) {
    cpu->acc += peek(cpu, cpu->pc + 1);
    cpu->pc += 2;
}
static void dis__addi(const cpu_state_t* cpu, addr_t addr, char* buf, size_t len) {
    snprintf(buf, len, "nop");
}

const instruction_t i__addi = {
    .mnemonic = "_addi",
    .opcode_mask = 0,
    .opcode = MULTICYCLE_MASK | ALU_SEL_MASK | ALU_ADD,
    .assemble = asm__addi,
    .disassemble = dis__addi,
    .execute = exe__addi
};