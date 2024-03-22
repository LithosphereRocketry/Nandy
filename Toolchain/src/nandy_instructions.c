#include "nandy_instructions.h"
#include "nandy_instr_defs.h"
#include "stdio.h"


static void exe__addi(cpu_state_t* cpu) {
    cpu->acc += peek(cpu, cpu->pc + 1);
    cpu->pc += 2;
}

static void dis__addi(const cpu_state_t* cpu, addr_t addr, char* buf, size_t len) {
    snprintf(buf, len, "nop");
}

static const instruction_t* default_instrs[] = {
    &i_nop, 
    &i_rd,
    // {
    //     .mnemonic = "_addi",
    //     .opcode_mask = 0,
    //     .opcode = MULTICYCLE_MASK | ALU_SEL_MASK | ALU_ADD,
    //     .disassemble = dis__addi,
    //     .execute = exe__addi
    // }
};

const ilist_t NANDY_ILIST = {
    .size = sizeof(default_instrs) / sizeof(instruction_t*),
    .list = default_instrs
};