#include "nandy_instr_defs.h"
#include "nandy_parse_tools.h"
#include "stdio.h"
#include "shuntingyard.h"

static void exe_jp(cpu_state_t* cpu) {
    cpu->pc = cpu->p+1;
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
    addr_t prevAddr = cpu->pc;
    cpu->pc = cpu->p+1;
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

static void exe_jxi(cpu_state_t* cpu) {
    cpu->pc = cpu->ia+1;
    cpu->int_active = false;
    cpu->elapsed ++;
}
const instruction_t i_jxi = {
    .mnemonic = "jxi",
    .opcode_mask = 0,
    .opcode = 0b110,
    .assemble = asm_basic,
    .disassemble = dis_basic,
    .execute = exe_jxi
};

static bool resolveReljump(asm_state_t* state, const char* text, addr_t pos, FILE* debug) {
    int64_t value;
    shunting_status_t status = parseExp(&state->resolved, text, &value, debug);
    if(status == SHUNT_DONE) {
        int64_t offset = value - pos - 1;
        if(!isBounded(offset, 11, BOUND_SIGNED)) {
            printf("Error: jump to %s is too long! (%li)\n", text, offset);
            return false;
        }
        state->rom[pos] |= (offset >> 7) & IMMJ_MASK;
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

static addr_t offset_from_instr(cpu_state_t* cpu, addr_t addr) {
    return  signExtend(
        ((int) peek(cpu, addr) & IMMJ_MASK) << 7
        | (uint8_t) peek(cpu, addr+1)
    , 11);
}

void dis_reljump(const instruction_t* instr, cpu_state_t* cpu, addr_t addr, char* buf, size_t len) {
    int64_t offset = offset_from_instr(cpu, addr);
    snprintf(buf, len, "%s %04hx", instr->mnemonic, addr + ((addr_t) offset) + 1);
}

static void exe_j(cpu_state_t* cpu) {
    cpu->pc += offset_from_instr(cpu, cpu->pc) + 1;
    cpu->elapsed += 2;
}
const instruction_t i_j = {
    .mnemonic = "j",
    .opcode_mask = IMMJ_MASK,
    .opcode = MULTIBYTE_MASK,
    .assemble = asm_reljump,
    .disassemble = dis_reljump,
    .execute = exe_j
};

static void exe_jr(cpu_state_t* cpu) {
    cpu->p = cpu->pc+1;
    cpu->pc += offset_from_instr(cpu, cpu->pc) + 1;
    cpu->elapsed += 2;
}
const instruction_t i_jr = {
    .mnemonic = "jr",
    .opcode_mask = IMMJ_MASK,
    .opcode = MULTIBYTE_MASK | RET_MASK,
    .assemble = asm_reljump,
    .disassemble = dis_reljump,
    .execute = exe_jr
};

static void exe_jc(cpu_state_t* cpu) {
    if(cpu->carry) {
        exe_j(cpu);
    } else {
        cpu->pc += 2;
        cpu->elapsed += 2;
    }
}
const instruction_t i_jc = {
    .mnemonic = "jc",
    .opcode_mask = IMM4_MASK,
    .opcode = MULTIBYTE_MASK | COND_MASK,
    .assemble = asm_reljump,
    .disassemble = dis_reljump,
    .execute = exe_jc
};

static void exe_jcr(cpu_state_t* cpu) {
    if(cpu->carry) {
        exe_jr(cpu);
    } else {
        cpu->pc += 2;
        cpu->elapsed += 2;
    }
}
const instruction_t i_jcr = {
    .mnemonic = "jcr",
    .opcode_mask = IMMJ_MASK,
    .opcode = MULTIBYTE_MASK | COND_MASK | RET_MASK,
    .assemble = asm_reljump,
    .disassemble = dis_reljump,
    .execute = exe_jcr
};