#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "nandy_parse_tools.h"
#include "shuntingyard.h"


static const char* regname_sp[] = {"sp", NULL};
static const char* regname_io[] = {"io", NULL};
static const char* regname_dx[] = {"dx", "x", "dl", NULL};
static const char* regname_dy[] = {"dy", "y", "dh", NULL};
static const char* regname_acc[] = {"acc", NULL};
const char** regnames[] = {
    [REG_SP] = regname_sp,
    [REG_IO] = regname_io,
    [REG_DX] = regname_dx,
    [REG_DY] = regname_dy,
    [REG_ACC] = regname_acc
};
const size_t n_regnames = sizeof(regnames) / sizeof(const char**);

const char* endOfInput(const char* str) {
    const char* ptr = str;
    while(*ptr != '\0' && *ptr != '\n' && strncmp(ptr, COMMENT_TOK, strlen(COMMENT_TOK))) {
        ptr++;
    }
    return ptr;
}

const char* parseFallback(const char* text) {
    while(!isspace(*text)) text++;
    return text;
}

const char* parseReg(const char* text, regid_t* dest) {
    while(isspace(*text) && *text != '\n') text++;
    for(size_t i = 0; i < n_regnames; i++) {
        const char** reg = regnames[i];
        while(*reg) {
            if(strncmp(text, *reg, strlen(*reg)) == 0) {
                *dest = i;
                return text + strlen(*reg);
            }
            reg++;
        }
    }
    return NULL;
}

const char* parseRegRequired(const char* text, regid_t* dest) {
    const char* after = parseReg(text, dest);
    if(!after) {
        after = parseFallback(text);
        if(after == text) {
            printf("No register name provided\n");
        } else {
            printf("Unrecognized register name \"%.*s\"\n", (int) (after-text), text);
        }
        return NULL;
    }
    return after;
}

static bool resolveImm4s(asm_state_t* state, const char* text, addr_t pos, FILE* debug) {
    int64_t value;
    shunting_status_t status = parseExp(&state->resolved, text, &value, debug);
    if(status == SHUNT_DONE) {
        if(!isBounded(value, 4, BOUND_SIGNED)) {
            if(debug) fprintf(debug, "Warning: value of %s ( == %li) will be truncated\n", text, value);
        }
        state->rom[pos] |= value & IMM4_MASK;
        return true;
    } else {
        if(debug) fprintf(debug, "Parse failed: %i\n", status);
        return false;
    }
}

static bool resolveImm4u(asm_state_t* state, const char* text, addr_t pos, FILE* debug) {
    int64_t value;
    shunting_status_t status = parseExp(&state->resolved, text, &value, debug);
    if(status == SHUNT_DONE) {
        if(!isBounded(value, 4, BOUND_UNSIGNED)) {
            if(debug) fprintf(debug, "Warning: value of %s ( == %li) will be truncated\n", text, value);
        }
        state->rom[pos] |= value & IMM4_MASK;
        return true;
    } else {
        if(debug) fprintf(debug, "Parse failed: %i\n", status);
        return false;
    }
}

static bool resolveImm8(asm_state_t* state, const char* text, addr_t pos, FILE* debug) {
    int64_t value;
    shunting_status_t status = parseExp(&state->resolved, text, &value, debug);
    if(status == SHUNT_DONE) {
        if(!isBounded(value, 8, BOUND_EITHER)) {
            if(debug) fprintf(debug, "Warning: value of %s ( == %li) will be truncated\n", text, value);
        }
        state->rom[pos+1] = value;
        return true;
    } else {
        if(debug) fprintf(debug, "Parse failed: %i\n", status);
        return false;
    }
}

const char* asm_basic(const instruction_t* instr, const char* text, asm_state_t* state) {
    state->rom[state->rom_loc] = instr->opcode;
    state->rom_loc ++;
    return text;
}

const char* asm_register(const instruction_t* instr, const char* text, asm_state_t* state) {
    regid_t reg;
    const char* after = parseRegRequired(text, &reg);
    if(reg == REG_ACC) {
        printf("Cannot move accumulator to/from itself\n");
        return NULL;
    }
    state->rom[state->rom_loc] = instr->opcode | reg;
    state->rom_loc ++;
    return after;
}

const char* asm_alu_reg(const instruction_t* instr, const char* text, asm_state_t* state) {
    regid_t reg;
    const char* after = parseRegRequired(text, &reg);
    if(!after) { return NULL; }
    if(reg == REG_DX) {
        state->rom[state->rom_loc] = instr->opcode;
    } else if(reg == REG_DY) {
        state->rom[state->rom_loc] = instr->opcode | XY_MASK;
    } else {
        printf("Register %s not valid for ALU instruction\n", regnames[reg][0]);
        return NULL;
    }
    state->rom_loc ++;
    return after;
}

const char* asm_alu_imm(const instruction_t* instr, const char* text, asm_state_t* state) {
    state->rom[state->rom_loc] = instr->opcode;
    const char* endptr = addUnresolved(state, text, resolveImm8);
    state->rom_loc += 2;    
    return endptr;
}

const char *asm_imm4s(const instruction_t *instr, const char *text, asm_state_t *state) {
    state->rom[state->rom_loc] = instr->opcode;
    const char* endptr = addUnresolved(state, text, resolveImm4s);
    state->rom_loc ++;
    return endptr;
}

const char *asm_imm4u(const instruction_t *instr, const char *text, asm_state_t *state) {
    state->rom[state->rom_loc] = instr->opcode;
    const char* endptr = addUnresolved(state, text, resolveImm4u);
    state->rom_loc ++;
    return endptr;
}


word_t getXYReg(cpu_state_t* cpu, bool isY) {
    return cpu->int_active ? (isY ? cpu->iry : cpu->irx)
                           : (isY ? cpu->dy : cpu->dx);
}

word_t getALUReg(cpu_state_t* cpu) {
    return getXYReg(cpu, peek(cpu, cpu->pc) & XY_MASK);
}

addr_t getAbsAddr(cpu_state_t* cpu) {
    return (((int) getXYReg(cpu, true)) << 8 | getXYReg(cpu, false))
             + (peek(cpu, cpu->pc) & IMM4_MASK);
}

addr_t getStackAddr(cpu_state_t* cpu) {
    return (0xFF00 | cpu->sp) + (peek(cpu, cpu->pc) & IMM4_MASK);
}


void dis_basic(const instruction_t* instr, cpu_state_t* cpu, addr_t addr, char* buf, size_t len) {
    snprintf(buf, len, "%s", instr->mnemonic);
}
void dis_register(const instruction_t* instr, cpu_state_t* cpu, addr_t addr, char* buf, size_t len) {
    snprintf(buf, len, "%s %s", instr->mnemonic, regnames[peek(cpu, addr) & 0b11][0]);
}
void dis_alu_reg(const instruction_t* instr, cpu_state_t* cpu, addr_t addr, char* buf, size_t len) {
    snprintf(buf, len, "%s %s", instr->mnemonic,
            regnames[(peek(cpu, addr) & XY_MASK) ? REG_DY : REG_DX][0]);
}
void dis_alu_imm(const instruction_t* instr, cpu_state_t* cpu, addr_t addr, char* buf, size_t len) {
    snprintf(buf, len, "%s %hhi", instr->mnemonic, peek(cpu, addr+1));
}
void dis_imm4s(const instruction_t* instr, cpu_state_t* cpu, addr_t addr, char* buf, size_t len) {
    snprintf(buf, len, "%s %li", instr->mnemonic, signExtend(peek(cpu, addr), 4));
}
void dis_imm4u(const instruction_t* instr, cpu_state_t* cpu, addr_t addr, char* buf, size_t len) {
    snprintf(buf, len, "%s %i", instr->mnemonic, peek(cpu, addr) & IMM4_MASK);
}

bool isBounded(int64_t value, int64_t bitwidth, bound_mode_t bound) {
    switch(bound) {
        case BOUND_SIGNED:
            return value < (1 << (bitwidth-1)) && value >= -(1 << (bitwidth-1));
        case BOUND_UNSIGNED:
            return value < (1 << bitwidth) && value >= 0;
        case BOUND_EITHER:
            return value < (1 << bitwidth) && value >= -(1 << (bitwidth-1));
        default:
            return false;
    }
}

int64_t signExtend(int64_t value, int64_t bits) {
	int64_t mask = ~((1 << bits) - 1);
	if(value & (1 << (bits-1))) {
		return value | mask;
	} else {
		return value & ~mask;
	}
}