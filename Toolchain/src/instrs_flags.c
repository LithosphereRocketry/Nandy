#include "nandy_instr_defs.h"
#include "nandy_parse_tools.h"
#include "stdio.h"

static void exe_brk(cpu_state_t* cpu) {
    cpu->idbg = true;
    cpu->pc ++; cpu->elapsed ++;
}

const instruction_t i_brk = {
    .mnemonic = "brk",
    .opcode_mask = 0,
    .opcode = 0,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe_brk
};

static void exe_int(cpu_state_t* cpu) {
    printf("int not yet supported\n");
    cpu->pc ++; cpu->elapsed ++;
}
const instruction_t i_int = {
    .mnemonic = "int",
    .opcode_mask = 0,
    .opcode = 1,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe_int
};

static void exe_dint(cpu_state_t* cpu) {
    cpu->int_en = false;
    cpu->pc ++; cpu->elapsed ++;
}
const instruction_t i_dint = {
    .mnemonic = "dint",
    .opcode_mask = 0,
    .opcode = 2,
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
    .opcode = 3,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe_eint
};
