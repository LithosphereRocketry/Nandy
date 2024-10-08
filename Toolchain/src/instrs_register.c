#include "nandy_instr_defs.h"
#include "nandy_parse_tools.h"
#include "stdio.h"

static void exe_nop(cpu_state_t* cpu) {
    cpu->pc ++; cpu->elapsed ++;
}
const instruction_t i_nop = {
    .mnemonic = "nop",
    .opcode_mask = REGID_MASK,
    .opcode = 0,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe_nop
};

static void exe_rd(cpu_state_t* cpu) {
    word_t ibyte = peek(cpu, cpu->pc);
    switch(ibyte & i_rd.opcode_mask) {
        case REG_SP: cpu->acc = cpu->sp; break;
        case REG_IO: cpu->acc = cpu->ioin; cpu->io_rd = true; break;
        case REG_DX: cpu->acc = getXYReg(cpu, false); break;
        case REG_DY: cpu->acc = getXYReg(cpu, true); break;
    }
    cpu->pc ++; cpu->elapsed ++;
}
const instruction_t i_rd = {
    .mnemonic = "rd",
    .opcode_mask = REGID_MASK,
    .opcode = RD_MASK,
    .assemble = asm_register,
    .disassemble = dis_register,
    .execute = exe_rd
};

static void exe_wr(cpu_state_t* cpu) {
    word_t ibyte = peek(cpu, cpu->pc);
    switch(ibyte & i_wr.opcode_mask) {
        case REG_SP: cpu->sp = cpu->acc; break;
        case REG_IO: cpu->ioout = cpu->acc; cpu->io_wr = true; break;
        case REG_DX: putXYreg(cpu, false, cpu->acc); break;
        case REG_DY: putXYreg(cpu, true, cpu->acc); break;
    }
    cpu->pc ++; cpu->elapsed ++;
}
const instruction_t i_wr = {
    .mnemonic = "wr",
    .opcode_mask = REGID_MASK,
    .opcode = WR_MASK,
    .assemble = asm_register,
    .disassemble = dis_register,
    .execute = exe_wr
};

static void exe_sw(cpu_state_t* cpu) {
    word_t ibyte = peek(cpu, cpu->pc);
    word_t tmp = cpu->acc;
    switch(ibyte & i_rd.opcode_mask) {
        case REG_SP: cpu->acc = cpu->sp; cpu->sp = tmp; break;
        case REG_IO: cpu->acc = cpu->ioin; cpu->ioout = tmp; 
                     cpu->io_rd = true; cpu->io_wr = true; break;
        case REG_DX: cpu->acc = getXYReg(cpu, false); putXYreg(cpu, false, tmp); break;
        case REG_DY: cpu->acc = getXYReg(cpu, true); putXYreg(cpu, true, tmp); break;
    }
    cpu->pc ++; cpu->elapsed ++;
}
const instruction_t i_sw = {
    .mnemonic = "sw",
    .opcode_mask = REGID_MASK,
    .opcode = RD_MASK | WR_MASK,
    .assemble = asm_register,
    .disassemble = dis_register,
    .execute = exe_sw
};