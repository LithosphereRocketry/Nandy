#include "nandy_instr_defs.h"
#include "nandy_parse_tools.h"
#include "stdio.h"


static void exe_rmove(cpu_state_t* cpu) {
    word_t ibyte = peek(cpu, cpu->pc);
    // by casting to int16 early we make editing top bytes a little easier
    addr_t tmp = cpu->acc;

    if(ibyte & RD_MASK) {
        switch(ibyte & REGID_MASK) {
            // IO operations are handled by the IO device
            case REG_SP: cpu->acc = cpu->sp; break;
            case REG_IO: cpu->io_rd = true; break;
            case REG_X: cpu->acc = cpu->x; break;
            case REG_Y: cpu->acc = cpu->y; break;
            case REG_PH: cpu->acc = cpu->p >> 8; break;
            case REG_PL: cpu->acc = cpu->p & 0xFF; break;
            case REG_QH: cpu->acc = cpu->q >> 8; break;
            case REG_QL: cpu->acc = cpu->q & 0xFF; break;
        }
    }
    if(ibyte & WR_MASK) {
        switch(ibyte & REGID_MASK) {
            case REG_SP: cpu->sp = tmp; break;
            case REG_IO: cpu->io_wr = true; break;
            case REG_X: cpu->x = tmp; break;
            case REG_Y: cpu->y = tmp; break;
            case REG_PH: cpu->p = (cpu->p & 0xFF) | (tmp << 8); break;
            case REG_PL: cpu->p = (cpu->p & 0xFF00) | tmp; break;
            case REG_QH: cpu->q = (cpu->q & 0xFF) | (tmp << 8); break;
            case REG_QL: cpu->q = (cpu->q & 0xFF00) | tmp; break;
        }
    }
    cpu->pc ++; cpu->elapsed ++;
}

const instruction_t i_rd = {
    .mnemonic = "rd",
    .opcode_mask = REGID_MASK,
    .opcode = RD_MASK,
    .assemble = asm_register,
    .disassemble = dis_register,
    .execute = exe_rmove
};

const instruction_t i_wr = {
    .mnemonic = "wr",
    .opcode_mask = REGID_MASK,
    .opcode = WR_MASK,
    .assemble = asm_register,
    .disassemble = dis_register,
    .execute = exe_rmove
};

const instruction_t i_sw = {
    .mnemonic = "sw",
    .opcode_mask = REGID_MASK,
    .opcode = RD_MASK | WR_MASK,
    .assemble = asm_register,
    .disassemble = dis_register,
    .execute = exe_rmove
};