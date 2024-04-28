#include "nandy_instr_defs.h"
#include "nandy_parse_tools.h"
#include "stdio.h"
#include "shuntingyard.h"

static void exe_ja(cpu_state_t* cpu) {
    cpu->pc = (((addr_t) cpu->dy) << 8) + cpu->dx;
    cpu->elapsed ++;
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
    cpu->elapsed ++;
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
    cpu->elapsed ++;
}
const instruction_t i_jar = {
    .mnemonic = "jar",
    .opcode_mask = 0b1,
    .opcode = PROGFLOW_MASK | RET_MASK,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe_jar
};

static bool resolveReljump(asm_state_t* state, const char* text, addr_t pos, FILE* debug) {
    int64_t value;
    shunting_status_t status = parseExp(&state->resolved, text, &value, debug);
    if(status == SHUNT_DONE) {
        int64_t offset = value - pos - 1;
        if(!isBounded(offset, 12, BOUND_SIGNED)) {
            printf("Error: jump to %s is too long! (%li)\n", text, offset);
            return false;
        }
        state->rom[pos] |= (offset >> 8) & IMM4_MASK;
        state->rom[pos+1] = offset & 0xFF;
        return true;
    } else {
        if(debug) fprintf(debug, "Parse failed: %i\n", status);
        return false;
    }
}

static const char* asm_reljump(const instruction_t *instr, const char *text, asm_state_t *state) {
    state->rom[state->rom_loc] = instr->opcode;
    const char* endptr = addUnresolved(state, text, resolveReljump);
    state->rom_loc += 2;
    return endptr;
}

void dis_reljump(const instruction_t* instr, cpu_state_t* cpu, addr_t addr, char* buf, size_t len) {
    int64_t offset = signExtend((((int) peek(cpu, addr)) << 8) | peek(cpu, addr+1), 12);
    snprintf(buf, len, "%s %lx", instr->mnemonic, addr + offset + 1);
}

static void exe_j(cpu_state_t* cpu) {
    cpu->pc += signExtend((((int) peek(cpu, cpu->pc)) << 8) | peek(cpu, cpu->pc+1), 12) + 1;
    cpu->elapsed += 2;
}
const instruction_t i_j = {
    .mnemonic = "j",
    .opcode_mask = IMM4_MASK,
    .opcode = MULTICYCLE_MASK | ALU_SEL_MASK | JUMP_MASK,
    .assemble = asm_reljump,
    .disassemble = dis_reljump,
    .execute = exe_j
};

static void exe_jcz(cpu_state_t* cpu) {
    if(!cpu->carry) {
        cpu->pc += signExtend((((int) peek(cpu, cpu->pc)) << 8) | peek(cpu, cpu->pc+1), 12) + 1;
    } else {
        cpu->pc += 2;
    }
    cpu->elapsed += 2;
}
const instruction_t i_jcz = {
    .mnemonic = "jcz",
    .opcode_mask = IMM4_MASK,
    .opcode = MULTICYCLE_MASK | ALU_SEL_MASK | JUMP_MASK | COND_MASK,
    .assemble = asm_reljump,
    .disassemble = dis_reljump,
    .execute = exe_jcz
};