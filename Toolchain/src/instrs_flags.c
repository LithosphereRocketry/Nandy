#include "nandy_instr_defs.h"
#include "nandy_parse_tools.h"
#include "stdio.h"

static void exe_brk(cpu_state_t* cpu) {
    // TODO: exit to debugger
    cpu->pc ++;
}

const instruction_t i_brk = {
    .mnemonic = "brk",
    .opcode_mask = 0,
    .opcode = PROGFLOW_MASK | SIG_MASK | SIG_BRK,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe_brk
};

static void exe_bell(cpu_state_t* cpu) {
    putchar('\a');
    cpu->pc ++;
}
const instruction_t i_bell = {
    .mnemonic = "bell",
    .opcode_mask = 0,
    .opcode = PROGFLOW_MASK | SIG_MASK | SIG_BELL,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe_bell
};

static void exe_dint(cpu_state_t* cpu) {
    cpu->int_en = false;
    cpu->pc ++;
}
const instruction_t i_dint = {
    .mnemonic = "dint",
    .opcode_mask = 0,
    .opcode = PROGFLOW_MASK | SIG_MASK | SIG_DINT,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe_dint
};

static void exe_eint(cpu_state_t* cpu) {
    cpu->int_en = true;
    cpu->pc ++;
}
const instruction_t i_eint = {
    .mnemonic = "eint",
    .opcode_mask = 0,
    .opcode = PROGFLOW_MASK | SIG_MASK | SIG_EINT,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe_eint
};

static void exe_iclr(cpu_state_t* cpu) {
    cpu->int_active = false;
    cpu->pc ++;
}
const instruction_t i_iclr = {
    .mnemonic = "iclr",
    .opcode_mask = 0,
    .opcode = PROGFLOW_MASK | SIG_MASK | SIG_ICLR,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe_iclr
};

static void exe_iset(cpu_state_t* cpu) {
    cpu->int_active = true;
    cpu->pc ++;
}
const instruction_t i_iset = {
    .mnemonic = "iset",
    .opcode_mask = 0,
    .opcode = PROGFLOW_MASK | SIG_MASK | SIG_ISET,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe_iset
};