#include "nandy_instr_defs.h"
#include "stdio.h"

static const char* asm_nop(const char* text, asm_state_t* state) {
    state->cpu.rom[state->rom_loc] = 0;
    state->rom_loc ++;
    return text;
}
static void exe_nop(cpu_state_t* cpu) {
    cpu->pc ++;
}
static void dis_nop(const cpu_state_t* cpu, addr_t addr, char* buf, size_t len) {
    snprintf(buf, len, "nop");
}

const instruction_t i_nop = {
    .mnemonic = "nop",
    .opcode_mask = REGID_MASK,
    .opcode = 0,
    .assemble = asm_nop,
    .disassemble = dis_nop,
    .execute = exe_nop
};