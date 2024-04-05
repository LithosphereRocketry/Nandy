#include "nandy_instr_defs.h"
#include "nandy_parse_tools.h"
#include "stdio.h"

static void exe_ja(cpu_state_t* cpu) {
    cpu->pc = (((addr_t) cpu->dy) << 8) + cpu->dx;
}
const instruction_t i_ja = {
    .mnemonic = "ja",
    .opcode_mask = 0b1,
    .opcode = PROGFLOW_MASK,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe_ja
};

static void exe_jri(cpu_state_t* cpu) {
    cpu->pc = (((addr_t) cpu->dy) << 8) + cpu->dx;
    cpu->int_active = false;
}
const instruction_t i_jri = {
    .mnemonic = "jri",
    .opcode_mask = 0b1,
    .opcode = PROGFLOW_MASK | CI_MASK,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe_jri
};

static void exe_jar(cpu_state_t* cpu) {
    addr_t prevAddr = cpu->pc + 1;
    cpu->pc = (((addr_t) cpu->dy) << 8) + cpu->dx;
    cpu->dx = prevAddr & 0xFF;
    cpu->dy = prevAddr >> 8;
}
const instruction_t i_jar = {
    .mnemonic = "jar",
    .opcode_mask = 0b1,
    .opcode = PROGFLOW_MASK | RET_MASK,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe_jar
};