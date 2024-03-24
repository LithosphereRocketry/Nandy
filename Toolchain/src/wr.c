#include "nandy_instr_defs.h"
#include "stdio.h"

static const char* asm_wr(const char* text, asm_state_t* state) {
    regid_t reg;
    const char* after = parseRegRequired(text, &reg);
    if(reg == REG_ACC) {
        printf("Cannot write accumulator to itself\n");
        return NULL;
    }
    state->cpu.rom[state->rom_loc] = WR_MASK | reg;
    state->rom_loc ++;
    return after;
}
static void exe_wr(cpu_state_t* cpu) {
    word_t ibyte = peek(cpu, cpu->pc);
    switch(ibyte & i_wr.opcode_mask) {
        case REG_SP: cpu->sp = cpu->acc; break;
        case REG_IO: cpu->ioout = cpu->acc; break; // todo: data ping
        case REG_DX: cpu->dx = cpu->acc; break;
        case REG_DY: cpu->dy = cpu->acc; break;
    }
    cpu->pc ++;
}
static void dis_wr(const cpu_state_t* cpu, addr_t addr, char* buf, size_t len) {
    word_t ibyte = peek(cpu, addr);
    snprintf(buf, len, "wr %s", regnames[ibyte & 0b11]);
}

const instruction_t i_wr = {
    .mnemonic = "wr",
    .opcode_mask = REGID_MASK,
    .opcode = RD_MASK,
    .assemble = asm_wr,
    .disassemble = dis_wr,
    .execute = exe_wr
};