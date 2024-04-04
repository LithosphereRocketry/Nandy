#include "nandy_instr_defs.h"
#include "nandy_parse_tools.h"
#include "stdio.h"

static const char* asm_brk(const char* text, asm_state_t* state) {
    state->rom[state->rom_loc] = PROGFLOW_MASK | SIG_MASK | SIG_BRK;
    state->rom_loc ++;
    return text;
}
static void exe_brk(cpu_state_t* cpu) {
    // TODO: exit to debugger
    cpu->pc ++;
}
static void dis_brk(const cpu_state_t* cpu, addr_t addr, char* buf, size_t len) {
    snprintf(buf, len, "brk");
}

const instruction_t i_brk = {
    .mnemonic = "brk",
    .opcode_mask = 0,
    .opcode = PROGFLOW_MASK | SIG_MASK | SIG_BRK,
    .assemble = asm_brk,
    .disassemble = dis_brk,
    .execute = exe_brk
};

static const char* asm_bell(const char* text, asm_state_t* state) {
    state->rom[state->rom_loc] = PROGFLOW_MASK | SIG_MASK | SIG_BELL;
    state->rom_loc ++;
    return text;
}
static void exe_bell(cpu_state_t* cpu) {
    putchar('\a');
    cpu->pc ++;
}
static void dis_bell(const cpu_state_t* cpu, addr_t addr, char* buf, size_t len) {
    snprintf(buf, len, "bell");
}

const instruction_t i_bell = {
    .mnemonic = "bell",
    .opcode_mask = 0,
    .opcode = PROGFLOW_MASK | SIG_MASK | SIG_BELL,
    .assemble = asm_bell,
    .disassemble = dis_bell,
    .execute = exe_bell
};

static const char* asm_dint(const char* text, asm_state_t* state) {
    state->rom[state->rom_loc] = PROGFLOW_MASK | SIG_MASK | SIG_DINT;
    state->rom_loc ++;
    return text;
}
static void exe_dint(cpu_state_t* cpu) {
    cpu->int_en = false;
    cpu->pc ++;
}
static void dis_dint(const cpu_state_t* cpu, addr_t addr, char* buf, size_t len) {
    snprintf(buf, len, "dint");
}

const instruction_t i_dint = {
    .mnemonic = "dint",
    .opcode_mask = 0,
    .opcode = PROGFLOW_MASK | SIG_MASK | SIG_DINT,
    .assemble = asm_dint,
    .disassemble = dis_dint,
    .execute = exe_dint
};

static const char* asm_eint(const char* text, asm_state_t* state) {
    state->rom[state->rom_loc] = PROGFLOW_MASK | SIG_MASK | SIG_EINT;
    state->rom_loc ++;
    return text;
}
static void exe_eint(cpu_state_t* cpu) {
    cpu->int_en = true;
    cpu->pc ++;
}
static void dis_eint(const cpu_state_t* cpu, addr_t addr, char* buf, size_t len) {
    snprintf(buf, len, "eint");
}

const instruction_t i_eint = {
    .mnemonic = "eint",
    .opcode_mask = 0,
    .opcode = PROGFLOW_MASK | SIG_MASK | SIG_EINT,
    .assemble = asm_eint,
    .disassemble = dis_eint,
    .execute = exe_eint
};

static const char* asm_iclr(const char* text, asm_state_t* state) {
    state->rom[state->rom_loc] = PROGFLOW_MASK | SIG_MASK | SIG_ICLR;
    state->rom_loc ++;
    return text;
}
static void exe_iclr(cpu_state_t* cpu) {
    cpu->int_active = false;
    cpu->pc ++;
}
static void dis_iclr(const cpu_state_t* cpu, addr_t addr, char* buf, size_t len) {
    snprintf(buf, len, "iclr");
}

const instruction_t i_iclr = {
    .mnemonic = "iclr",
    .opcode_mask = 0,
    .opcode = PROGFLOW_MASK | SIG_MASK | SIG_ICLR,
    .assemble = asm_iclr,
    .disassemble = dis_iclr,
    .execute = exe_iclr
};

static const char* asm_iset(const char* text, asm_state_t* state) {
    state->rom[state->rom_loc] = PROGFLOW_MASK | SIG_MASK | SIG_ISET;
    state->rom_loc ++;
    return text;
}
static void exe_iset(cpu_state_t* cpu) {
    cpu->int_active = true;
    cpu->pc ++;
}
static void dis_iset(const cpu_state_t* cpu, addr_t addr, char* buf, size_t len) {
    snprintf(buf, len, "iset");
}

const instruction_t i_iset = {
    .mnemonic = "iset",
    .opcode_mask = 0,
    .opcode = PROGFLOW_MASK | SIG_MASK | SIG_ISET,
    .assemble = asm_iset,
    .disassemble = dis_iset,
    .execute = exe_iset
};