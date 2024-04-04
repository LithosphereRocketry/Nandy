#include "nandy_instr_defs.h"
#include "nandy_parse_tools.h"
#include "stdio.h"

static const char* asm_ja(const char* text, asm_state_t* state) {
    state->rom[state->rom_loc] = PROGFLOW_MASK;
    state->rom_loc ++;
    return text;
}
static void exe_ja(cpu_state_t* cpu) {
    cpu->pc = (((addr_t) cpu->dy) << 8) + cpu->dx;
}
static void dis_ja(const cpu_state_t* cpu, addr_t addr, char* buf, size_t len) {
    snprintf(buf, len, "ja");
}

const instruction_t i_ja = {
    .mnemonic = "ja",
    .opcode_mask = 0b1,
    .opcode = PROGFLOW_MASK,
    .assemble = asm_ja,
    .disassemble = dis_ja,
    .execute = exe_ja
};

static const char* asm_jri(const char* text, asm_state_t* state) {
    state->rom[state->rom_loc] = PROGFLOW_MASK | CI_MASK;
    state->rom_loc ++;
    return text;
}
static void exe_jri(cpu_state_t* cpu) {
    cpu->pc = (((addr_t) cpu->dy) << 8) + cpu->dx;
    cpu->int_active = false;
}
static void dis_jri(const cpu_state_t* cpu, addr_t addr, char* buf, size_t len) {
    snprintf(buf, len, "jri");
}

const instruction_t i_jri = {
    .mnemonic = "jri",
    .opcode_mask = 0b1,
    .opcode = PROGFLOW_MASK | CI_MASK,
    .assemble = asm_jri,
    .disassemble = dis_jri,
    .execute = exe_jri
};

static const char* asm_jar(const char* text, asm_state_t* state) {
    state->rom[state->rom_loc] = PROGFLOW_MASK | RET_MASK;
    state->rom_loc ++;
    return text;
}
static void exe_jar(cpu_state_t* cpu) {
    addr_t prevAddr = cpu->pc + 1;
    cpu->pc = (((addr_t) cpu->dy) << 8) + cpu->dx;
    cpu->dx = prevAddr & 0xFF;
    cpu->dy = prevAddr >> 8;
}
static void dis_jar(const cpu_state_t* cpu, addr_t addr, char* buf, size_t len) {
    snprintf(buf, len, "jar");
}

const instruction_t i_jar = {
    .mnemonic = "jar",
    .opcode_mask = 0b1,
    .opcode = PROGFLOW_MASK | RET_MASK,
    .assemble = asm_jar,
    .disassemble = dis_jar,
    .execute = exe_jar
};