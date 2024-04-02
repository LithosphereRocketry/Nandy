#include "nandy_instr_defs.h"
#include "nandy_parse_tools.h"
#include <stdio.h>

static const char* asm_rd(const char* text, asm_state_t* state) {
    regid_t reg;
    const char* after = parseRegRequired(text, &reg);
    if(reg == REG_ACC) {
        printf("Cannot read accumulator from itself\n");
        return NULL;
    }
    state->rom[state->rom_loc] = RD_MASK | reg;
    state->rom_loc ++;
    return after;
}
static void exe_rd(cpu_state_t* cpu) {
    word_t ibyte = peek(cpu, cpu->pc);
    switch(ibyte & i_rd.opcode_mask) {
        case REG_SP: cpu->acc = cpu->sp; break;
        case REG_IO: cpu->acc = cpu->ioin; break; // todo: data ping
        case REG_DX: cpu->acc = cpu->dx; break;
        case REG_DY: cpu->acc = cpu->dy; break;
    }
    cpu->pc ++;
}
static void dis_rd(const cpu_state_t* cpu, addr_t addr, char* buf, size_t len) {
    word_t ibyte = peek(cpu, addr);
    snprintf(buf, len, "rd %s", regnames[ibyte & 0b11]);
}

const instruction_t i_rd = {
    .mnemonic = "rd",
    .opcode_mask = REGID_MASK,
    .opcode = RD_MASK,
    .assemble = asm_rd,
    .disassemble = dis_rd,
    .execute = exe_rd
};