#include "nandy_instr_defs.h"
#include "nandy_parse_tools.h"
#include "stdio.h"

static const char* asm_nop(const char* text, asm_state_t* state) {
    state->rom[state->rom_loc] = 0;
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

static const char* asm_wr(const char* text, asm_state_t* state) {
    regid_t reg;
    const char* after = parseRegRequired(text, &reg);
    if(reg == REG_ACC) {
        printf("Cannot write accumulator to itself\n");
        return NULL;
    }
    state->rom[state->rom_loc] = WR_MASK | reg;
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

static const char* asm_sw(const char* text, asm_state_t* state) {
    regid_t reg;
    const char* after = parseRegRequired(text, &reg);
    if(reg == REG_ACC) {
        printf("Cannot swap accumulator with itself\n");
        return NULL;
    }
    state->rom[state->rom_loc] = RD_MASK | WR_MASK | reg;
    state->rom_loc ++;
    return after;
}
static void exe_sw(cpu_state_t* cpu) {
    word_t ibyte = peek(cpu, cpu->pc);
    word_t tmp = cpu->acc;
    switch(ibyte & i_rd.opcode_mask) {
        case REG_SP: cpu->acc = cpu->sp; cpu->sp = tmp; break;
        case REG_IO: cpu->acc = cpu->ioin; cpu->ioout = tmp; break; // todo: data ping
        case REG_DX: cpu->acc = cpu->dx; cpu->dx = tmp; break;
        case REG_DY: cpu->acc = cpu->dy; cpu->dy = tmp; break;
    }
    cpu->pc ++;
}
static void dis_sw(const cpu_state_t* cpu, addr_t addr, char* buf, size_t len) {
    word_t ibyte = peek(cpu, addr);
    snprintf(buf, len, "wr %s", regnames[ibyte & 0b11]);
}

const instruction_t i_sw = {
    .mnemonic = "sw",
    .opcode_mask = REGID_MASK,
    .opcode = RD_MASK | WR_MASK,
    .assemble = asm_sw,
    .disassemble = dis_sw,
    .execute = exe_sw
};