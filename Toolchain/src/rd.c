#include "nandy_instr_defs.h"
#include "stdio.h"

static const char* asm_rd(const char* text, asm_state_t* state) {
    regid_t reg;
    const char* after = parseReg(text, &reg);
    if(!after) {
        after = parseFallback(text);
        if(after == text) {
            printf("No register name provided\n");
        } else {
            printf("Unrecognized register name \"%.*s\"\n", (int) (after-text), text);
        }
        return NULL;
    }
    if(reg == REG_ACC) {
        printf("Cannot move accumulator into itself\n");
        return NULL;
    }
    state->cpu.rom[state->rom_loc] = WR_MASK | reg;
    state->rom_loc ++;
    return after;
}
static void exe_rd(cpu_state_t* cpu) {
    word_t ibyte = peek(cpu, cpu->pc);
    switch(ibyte & 0x11) {
        case 0: cpu->acc = cpu->sp; break;
        case 1: cpu->acc = cpu->ioin; break; // todo: data ping
        case 2: cpu->acc = cpu->dx; break;
        case 3: cpu->acc = cpu->dy; break;
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