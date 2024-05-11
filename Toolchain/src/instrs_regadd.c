#include "nandy_instr_defs.h"
#include "nandy_parse_tools.h"
#include "nandy_alufuncs.h"
#include "shuntingyard.h"
#include <stdio.h>

static bool resolveInc(asm_state_t* state, const char* text, addr_t pos, FILE* debug) {
    int64_t value;
    shunting_status_t status = parseExp(&state->resolved, text, &value, debug);
    if(status == SHUNT_DONE) {
        if((value & 0b11) == 0) {
            if(debug) fprintf(debug, "Error: value of %s ( == %li) is not valid for inc", text, value);
            return false;
        } else if(value > 4 || value < -4) {
            if(debug) fprintf(debug, "Warning: value of %s ( == %li) will be truncated\n", text, value);
        }
        state->rom[pos] |= value & INC_MASK;
        return true;
    } else {
        if(debug) fprintf(debug, "Parse failed: %i\n", status);
        return false;
    }
}

static const char* asm_inc(const instruction_t* instr, const char* text, asm_state_t* state) {
    state->rom[state->rom_loc] = instr->opcode;
    const char* endptr = addUnresolved(state, text, resolveInc);
    state->rom_loc ++;
    return endptr;
}

void dis_inc(const instruction_t* instr, cpu_state_t* cpu, addr_t addr, char* buf, size_t len) {
    snprintf(buf, len, "%s %li", instr->mnemonic, signExtend(peek(cpu, addr), 3));
}

static void exe__add(cpu_state_t* cpu) {
    cpu->acc = alu_add(cpu->acc, getALUReg(cpu), false, NULL);
    cpu->pc ++; cpu->elapsed ++;
}
const instruction_t i__add = {
    .mnemonic = "_add",
    .opcode_mask = XY_MASK,
    .opcode = ALU_SEL_MASK | ALU_ADD,
    .assemble = asm_alu_reg,
    .disassemble = dis_alu_reg,
    .execute = exe__add
};

static void exe__addc(cpu_state_t* cpu) {
    cpu->acc = alu_add(cpu->acc, getALUReg(cpu), cpu->carry, NULL);
    cpu->pc ++; cpu->elapsed ++;
}
const instruction_t i__addc = {
    .mnemonic = "_addc",
    .opcode_mask = XY_MASK,
    .opcode = ALU_SEL_MASK | ALU_ADDC,
    .assemble = asm_alu_reg,
    .disassemble = dis_alu_reg,
    .execute = exe__addc
};

static void exe__sub(cpu_state_t* cpu) {
    cpu->acc = alu_sub(cpu->acc, getALUReg(cpu), true, NULL);
    cpu->pc ++; cpu->elapsed ++;
}
const instruction_t i__sub = {
    .mnemonic = "_sub",
    .opcode_mask = XY_MASK,
    .opcode = ALU_SEL_MASK | ALU_SUB,
    .assemble = asm_alu_reg,
    .disassemble = dis_alu_reg,
    .execute = exe__sub
};

static void exe__subc(cpu_state_t* cpu) {
    cpu->acc = alu_sub(cpu->acc, getALUReg(cpu), cpu->carry, NULL);
    cpu->pc ++; cpu->elapsed ++;
}
const instruction_t i__subc = {
    .mnemonic = "_subc",
    .opcode_mask = XY_MASK,
    .opcode = ALU_SEL_MASK | ALU_SUBC,
    .assemble = asm_alu_reg,
    .disassemble = dis_alu_reg,
    .execute = exe__subc
};

static void exe__inc(cpu_state_t* cpu) {
    cpu->acc = alu_add(cpu->acc, signExtend(peek(cpu, cpu->pc), 3), false, NULL);
    cpu->pc ++; cpu->elapsed ++;
}
const instruction_t i__inc = {
    .mnemonic = "_inc",
    .opcode_mask = INC_MASK,
    .opcode = ALU_SEL_MASK | XY_MASK | ONEOP_MASK,
    .assemble = asm_inc,
    .disassemble = dis_inc,
    .execute = exe__inc
};

static void exe__acf(cpu_state_t* cpu) {
    cpu->acc = alu_add(cpu->acc, 0, cpu->carry, NULL);
    cpu->pc ++; cpu->elapsed ++;
}
const instruction_t i__acf = {
    .mnemonic = "_acf",
    .opcode_mask = 0,
    .opcode = ALU_SEL_MASK | XY_MASK | ONEOP_MASK,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe__acf
};

static void exe__scf(cpu_state_t* cpu) {
    cpu->acc = alu_add(cpu->acc, -1, cpu->carry, NULL);
    cpu->pc ++; cpu->elapsed ++;
}
const instruction_t i__scf = {
    .mnemonic = "_scf",
    .opcode_mask = 0,
    .opcode = ALU_SEL_MASK | XY_MASK | ONEOP_MASK | SCF_MASK,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe__scf
};

static void exe_add(cpu_state_t* cpu) {
    cpu->acc = alu_add(cpu->acc, getALUReg(cpu), false, &cpu->carry);
    cpu->pc ++; cpu->elapsed ++;
}
const instruction_t i_add = {
    .mnemonic = "add",
    .opcode_mask = XY_MASK,
    .opcode = ALU_SEL_MASK | CARRY_SEL_MASK | ALU_ADD,
    .assemble = asm_alu_reg,
    .disassemble = dis_alu_reg,
    .execute = exe_add
};

static void exe_addc(cpu_state_t* cpu) {
    cpu->acc = alu_add(cpu->acc, getALUReg(cpu), cpu->carry, &cpu->carry);
    cpu->pc ++; cpu->elapsed ++;
}
const instruction_t i_addc = {
    .mnemonic = "addc",
    .opcode_mask = XY_MASK,
    .opcode = ALU_SEL_MASK | CARRY_SEL_MASK | ALU_ADDC,
    .assemble = asm_alu_reg,
    .disassemble = dis_alu_reg,
    .execute = exe_addc
};

static void exe_sub(cpu_state_t* cpu) {
    cpu->acc = alu_sub(cpu->acc, getALUReg(cpu), true, &cpu->carry);
    cpu->pc ++; cpu->elapsed ++;
}
const instruction_t i_sub = {
    .mnemonic = "sub",
    .opcode_mask = XY_MASK,
    .opcode = ALU_SEL_MASK | CARRY_SEL_MASK | ALU_SUB,
    .assemble = asm_alu_reg,
    .disassemble = dis_alu_reg,
    .execute = exe_sub
};

static void exe_subc(cpu_state_t* cpu) {
    cpu->acc = alu_sub(cpu->acc, getALUReg(cpu), cpu->carry, &cpu->carry);
    cpu->pc ++; cpu->elapsed ++;
}
const instruction_t i_subc = {
    .mnemonic = "subc",
    .opcode_mask = XY_MASK,
    .opcode = ALU_SEL_MASK | CARRY_SEL_MASK | ALU_SUBC,
    .assemble = asm_alu_reg,
    .disassemble = dis_alu_reg,
    .execute = exe_subc
};

static void exe_inc(cpu_state_t* cpu) {
    cpu->acc = alu_add(cpu->acc, signExtend(peek(cpu, cpu->pc), 3), false, &cpu->carry);
    cpu->pc ++; cpu->elapsed ++;
}
const instruction_t i_inc = {
    .mnemonic = "inc",
    .opcode_mask = INC_MASK,
    .opcode = ALU_SEL_MASK | CARRY_SEL_MASK | XY_MASK | ONEOP_MASK,
    .assemble = asm_inc,
    .disassemble = dis_inc,
    .execute = exe_inc
};

static void exe_acf(cpu_state_t* cpu) {
    cpu->acc = alu_add(cpu->acc, 0, cpu->carry, &cpu->carry);
    cpu->pc ++; cpu->elapsed ++;
}
const instruction_t i_acf = {
    .mnemonic = "acf",
    .opcode_mask = 0,
    .opcode = ALU_SEL_MASK | CARRY_SEL_MASK | XY_MASK | ONEOP_MASK,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe_acf
};

static void exe_scf(cpu_state_t* cpu) {
    cpu->acc = alu_add(cpu->acc, -1, cpu->carry, &cpu->carry);
    cpu->pc ++; cpu->elapsed ++;
}
const instruction_t i_scf = {
    .mnemonic = "scf",
    .opcode_mask = 0,
    .opcode = ALU_SEL_MASK | CARRY_SEL_MASK | XY_MASK | ONEOP_MASK | SCF_MASK,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe_scf
};