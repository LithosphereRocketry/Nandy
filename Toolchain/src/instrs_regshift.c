#include "nandy_instr_defs.h"
#include "nandy_parse_tools.h"
#include "nandy_alufuncs.h"
#include <stdio.h>

static void exe__sl(cpu_state_t* cpu) {
    cpu->acc = alu_sl(cpu->acc, 0, false, NULL);
    cpu->pc ++;
}
const instruction_t i__sl = {
    .mnemonic = "_sl",
    .opcode_mask = 0,
    .opcode = ALU_SEL_MASK | ALU_SL,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe__sl
};

static void exe__slc(cpu_state_t* cpu) {
    cpu->acc = alu_sl(cpu->acc, 0, cpu->carry, NULL);
    cpu->pc ++;
}
const instruction_t i__slc = {
    .mnemonic = "_slc",
    .opcode_mask = 0,
    .opcode = ALU_SEL_MASK | ALU_SLC,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe__slc
};

static void exe__sla(cpu_state_t* cpu) {
    cpu->acc = alu_sl(cpu->acc, 0, cpu->acc & 1, NULL);
    cpu->pc ++;
}
const instruction_t i__sla = {
    .mnemonic = "_sla",
    .opcode_mask = 0,
    .opcode = ALU_SEL_MASK | ALU_SLA,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe__sla
};

static void exe__slr(cpu_state_t* cpu) {
    cpu->acc = alu_sl(cpu->acc, 0, cpu->acc & 0x80, NULL);
    cpu->pc ++;
}
const instruction_t i__slr = {
    .mnemonic = "_slr",
    .opcode_mask = 0,
    .opcode = ALU_SEL_MASK | ALU_SLR,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe__slr
};

static void exe__sr(cpu_state_t* cpu) {
    cpu->acc = alu_sr(cpu->acc, 0, false, NULL);
    cpu->pc ++;
}
const instruction_t i__sr = {
    .mnemonic = "_sr",
    .opcode_mask = 0,
    .opcode = ALU_SEL_MASK | SR_MASK | ALU_SL,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe__sr
};

static void exe__src(cpu_state_t* cpu) {
    cpu->acc = alu_sr(cpu->acc, 0, cpu->carry, NULL);
    cpu->pc ++;
}
const instruction_t i__src = {
    .mnemonic = "_src",
    .opcode_mask = 0,
    .opcode = ALU_SEL_MASK | SR_MASK | ALU_SLC,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe__src
};

static void exe__srr(cpu_state_t* cpu) {
    cpu->acc = alu_sr(cpu->acc, 0, cpu->acc & 0x80, NULL);
    cpu->pc ++;
}
const instruction_t i__srr = {
    .mnemonic = "_srr",
    .opcode_mask = 0,
    .opcode = ALU_SEL_MASK | SR_MASK | ALU_SLA,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe__srr
};

static void exe__sra(cpu_state_t* cpu) {
    cpu->acc = alu_sr(cpu->acc, 0, cpu->acc & 1, NULL);
    cpu->pc ++;
}
const instruction_t i__sra = {
    .mnemonic = "_sra",
    .opcode_mask = 0,
    .opcode = ALU_SEL_MASK | SR_MASK | ALU_SLR,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe__sra
};

static void exe_sl(cpu_state_t* cpu) {
    cpu->acc = alu_sl(cpu->acc, 0, false, &cpu->carry);
    cpu->pc ++;
}
const instruction_t i_sl = {
    .mnemonic = "sl",
    .opcode_mask = 0,
    .opcode = ALU_SEL_MASK | ALU_SL,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe_sl
};

static void exe_slc(cpu_state_t* cpu) {
    cpu->acc = alu_sl(cpu->acc, 0, cpu->carry, &cpu->carry);
    cpu->pc ++;
}
const instruction_t i_slc = {
    .mnemonic = "slc",
    .opcode_mask = 0,
    .opcode = ALU_SEL_MASK | ALU_SLC,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe_slc
};

static void exe_sla(cpu_state_t* cpu) {
    cpu->acc = alu_sl(cpu->acc, 0, cpu->acc & 1, &cpu->carry);
    cpu->pc ++;
}
const instruction_t i_sla = {
    .mnemonic = "sla",
    .opcode_mask = 0,
    .opcode = ALU_SEL_MASK | ALU_SLA,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe_sla
};

static void exe_slr(cpu_state_t* cpu) {
    cpu->acc = alu_sl(cpu->acc, 0, cpu->acc & 0x80, &cpu->carry);
    cpu->pc ++;
}
const instruction_t i_slr = {
    .mnemonic = "slr",
    .opcode_mask = 0,
    .opcode = ALU_SEL_MASK | ALU_SLR,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe_slr
};

static void exe_sr(cpu_state_t* cpu) {
    cpu->acc = alu_sr(cpu->acc, 0, false, &cpu->carry);
    cpu->pc ++;
}
const instruction_t i_sr = {
    .mnemonic = "sr",
    .opcode_mask = 0,
    .opcode = ALU_SEL_MASK | SR_MASK | ALU_SL,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe_sr
};

static void exe_src(cpu_state_t* cpu) {
    cpu->acc = alu_sr(cpu->acc, 0, cpu->carry, &cpu->carry);
    cpu->pc ++;
}
const instruction_t i_src = {
    .mnemonic = "src",
    .opcode_mask = 0,
    .opcode = ALU_SEL_MASK | SR_MASK | ALU_SLC,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe_src
};

static void exe_srr(cpu_state_t* cpu) {
    cpu->acc = alu_sr(cpu->acc, 0, cpu->acc & 0x80, &cpu->carry);
    cpu->pc ++;
}
const instruction_t i_srr = {
    .mnemonic = "srr",
    .opcode_mask = 0,
    .opcode = ALU_SEL_MASK | SR_MASK | ALU_SLA,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe_srr
};

static void exe_sra(cpu_state_t* cpu) {
    cpu->acc = alu_sr(cpu->acc, 0, cpu->acc & 1, &cpu->carry);
    cpu->pc ++;
}
const instruction_t i_sra = {
    .mnemonic = "sra",
    .opcode_mask = 0,
    .opcode = ALU_SEL_MASK | SR_MASK | ALU_SLR,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe_sra
};