#include "nandy_instr_defs.h"
#include "nandy_parse_tools.h"
#include "nandy_check_tools.h"
#include "stdio.h"

static const char* asm_brk(const instruction_t *instr, const char *text, asm_state_t *state) {
    const char* retval = asm_basic(instr, text, state);
    addNextCtrlBlock(&state->ctrl_graph, state->rom_loc, 0);
    return retval;
}

static void exe_brk(cpu_state_t* cpu) {
    cpu->idbg = true;
    cpu->pc ++; cpu->elapsed ++;
}

const instruction_t i_brk = {
    .mnemonic = "brk",
    .opcode_mask = 0,
    .opcode = PROGFLOW_MASK | SIG_MASK | SIG_BRK,
    .assemble = asm_brk,
    .disassemble = dis_basic,
    .execute = exe_brk
};

static void exe_bell(cpu_state_t* cpu) {
    putchar('\a');
    cpu->pc ++; cpu->elapsed ++;
}
const instruction_t i_bell = {
    .mnemonic = "bell",
    .opcode_mask = 0,
    .opcode = PROGFLOW_MASK | SIG_MASK | SIG_BELL,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe_bell
};

static void exe_csclr(cpu_state_t* cpu) {
    cpu->cs = false;
    cpu->pc ++; cpu->elapsed ++;
}
const instruction_t i_csclr = {
    .mnemonic = "csclr",
    .opcode_mask = 0,
    .opcode = PROGFLOW_MASK | SIG_MASK | SIG_CSCLR,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe_csclr
};

static void exe_csset(cpu_state_t* cpu) {
    cpu->cs = true;
    cpu->pc ++; cpu->elapsed ++;
}
const instruction_t i_csset = {
    .mnemonic = "csset",
    .opcode_mask = 0,
    .opcode = PROGFLOW_MASK | SIG_MASK | SIG_CSSET,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe_csset
};

static void exe_dint(cpu_state_t* cpu) {
    cpu->int_en = false;
    cpu->pc ++; cpu->elapsed ++;
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
    cpu->pc ++; cpu->elapsed ++;
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
    cpu->pc ++; cpu->elapsed ++;
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
    cpu->pc ++; cpu->elapsed ++;
}
const instruction_t i_iset = {
    .mnemonic = "iset",
    .opcode_mask = 0,
    .opcode = PROGFLOW_MASK | SIG_MASK | SIG_ISET,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe_iset
};

static void exe_ioa(cpu_state_t* cpu) {
    cpu->ioaddr = peek(cpu, cpu->pc) & IMM5_MASK;
    cpu->pc ++; cpu->elapsed ++;
}
const instruction_t i_ioa = {
    .mnemonic = "ioa",
    .opcode_mask = IMM5_MASK,
    .opcode = IOA_MASK,
    .assemble = asm_imm5u,
    .disassemble = dis_imm5u,
    .execute = exe_ioa
};