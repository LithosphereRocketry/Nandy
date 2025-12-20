#include "nandy_instr_defs.h"
#include "nandy_parse_tools.h"
#include "stdio.h"
#include "shuntingyard.h"

static void exe_jp(cpu_state_t* cpu) {
    cpu->pc = cpu->p;
    cpu->elapsed ++;
}
const instruction_t i_jp = {
    .mnemonic = "jp",
    .opcode_mask = 0,
    .opcode = 0b100,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe_jp
};

static void exe_jpr(cpu_state_t* cpu) {
    addr_t prevAddr = cpu->pc + 1;
    cpu->pc = cpu->p;
    cpu->p = prevAddr;
    cpu->elapsed ++;
}
const instruction_t i_jpr = {
    .mnemonic = "jpr",
    .opcode_mask = 0,
    .opcode = 0b101,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe_jpr
};

static void exe_jri(cpu_state_t* cpu) {
    cpu->pc = cpu->ia;
    cpu->int_active = false;
    cpu->elapsed ++;
}
const instruction_t i_jri = {
    .mnemonic = "jri",
    .opcode_mask = 0,
    .opcode = 0b110,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe_jri
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
    snprintf(buf, len, "%s %04lx", instr->mnemonic, addr + offset + 1);
}

static void exe_j(cpu_state_t* cpu) {
    cpu->pc += signExtend((((int) peek(cpu, cpu->pc)) << 8) | peek(cpu, cpu->pc+1), 12) + 1;
    cpu->elapsed += 2;
}
const instruction_t i_j = {
    .mnemonic = "j",
    .opcode_mask = IMM4_MASK,
    .opcode = 0b01000000,
    .assemble = asm_reljump,
    .disassemble = dis_reljump,
    .execute = exe_j
};

static void exe_jc(cpu_state_t* cpu) {
    if(!cpu->carry) {
        exe_j(cpu);
    } else {
        cpu->pc += 2;
        cpu->elapsed += 2;
    }
}
const instruction_t i_jc = {
    .mnemonic = "jc",
    .opcode_mask = IMM4_MASK,
    .opcode = 0b01010000,
    .assemble = asm_reljump,
    .disassemble = dis_reljump,
    .execute = exe_jc
};