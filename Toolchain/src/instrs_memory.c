#include "nandy_instr_defs.h"
#include "nandy_parse_tools.h"
#include "nandy_alufuncs.h"

static void exe__isp(cpu_state_t* cpu) {
    cpu->sp = alu_add(cpu->sp, signExtend(peek(cpu, cpu->pc), 4), false, NULL);
    cpu->pc ++;
}
const instruction_t i__isp = {
    .mnemonic = "_isp",
    .opcode_mask = IMM4_MASK,
    .opcode = ISP_MASK,
    .assemble = asm_imm4s,
    .disassemble = dis_imm4s,
    .execute = exe__isp
};

static void exe_isp(cpu_state_t* cpu) {
    cpu->sp = alu_add(cpu->sp, signExtend(peek(cpu, cpu->pc), 4), false, &cpu->carry);
    cpu->pc ++;
}
const instruction_t i_isp = {
    .mnemonic = "isp",
    .opcode_mask = IMM4_MASK,
    .opcode = ISP_MASK | CARRY_SEL_MASK,
    .assemble = asm_imm4s,
    .disassemble = dis_imm4s,
    .execute = exe_isp
};

static void exe_lda(cpu_state_t* cpu) {
    cpu->acc = peek(cpu, getAbsAddr(cpu));
    cpu->pc ++;
}
const instruction_t i_lda = {
    .mnemonic = "lda",
    .opcode_mask = IMM4_MASK,
    .opcode = MULTICYCLE_MASK,
    .assemble = asm_imm4u,
    .disassemble = dis_imm4u,
    .execute = exe_lda
};

static void exe_lds(cpu_state_t* cpu) {
    cpu->acc = peek(cpu, getStackAddr(cpu));
    cpu->pc ++;
}
const instruction_t i_lds = {
    .mnemonic = "lds",
    .opcode_mask = IMM4_MASK,
    .opcode = MULTICYCLE_MASK | MEM_STACK_MASK,
    .assemble = asm_imm4u,
    .disassemble = dis_imm4u,
    .execute = exe_lds
};

static void exe_stra(cpu_state_t* cpu) {
    poke(cpu, getAbsAddr(cpu), cpu->acc);
    cpu->pc ++;
}
const instruction_t i_stra = {
    .mnemonic = "stra",
    .opcode_mask = IMM4_MASK,
    .opcode = MULTICYCLE_MASK | MEM_WRITE_MASK,
    .assemble = asm_imm4u,
    .disassemble = dis_imm4u,
    .execute = exe_stra
};

static void exe_strs(cpu_state_t* cpu) {
    poke(cpu, getStackAddr(cpu), cpu->acc);
    cpu->pc ++;
}
const instruction_t i_strs = {
    .mnemonic = "strs",
    .opcode_mask = IMM4_MASK,
    .opcode = MULTICYCLE_MASK | MEM_WRITE_MASK | MEM_STACK_MASK,
    .assemble = asm_imm4u,
    .disassemble = dis_imm4u,
    .execute = exe_strs
};