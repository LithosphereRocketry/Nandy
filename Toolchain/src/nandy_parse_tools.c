#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "nandy_parse_tools.h"
#include "shuntingyard.h"

const char* regnames[] = {
    [REG_SP] = "sp",
    [REG_IO] = "io",
    [REG_X] = "x",
    [REG_Y] = "y",
    [REG_PH] = "ph",
    [REG_PL] = "pl",
    [REG_QH] = "qh",
    [REG_QL] = "ql",
    [REG_ACC] = "acc"
};
const size_t n_regnames = sizeof(regnames) / sizeof(const char*);

const char* mmnames[] = {
    [MM_STACK] = "sp",
    [MM_P] = "p",
    [MM_Q] = "q",
    [MM_PPOST] = "p+"
};
const size_t n_mmnames = sizeof(mmnames) / sizeof(const char*);

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
        if(strncmp(text, regnames[i], strlen(regnames[i])) == 0) {
            *dest = i;
            return text + strlen(regnames[i]);
        }
    }
    return NULL;
}

const char* parseMemMode(const char* text, memmode_t* dest) {
    while(isspace(*text) && *text != '\n') text++;
    const char* end_name = text;
    while(!isspace(*end_name)) end_name++;
    // slightly hacky but we know these are spaced out by 2^5
    for(size_t i = 0; i < n_mmnames; i += (1 << 5)) {
        if(strncmp(text, mmnames[i], end_name-text) == 0) {
            *dest = i;
            return text + strlen(mmnames[i]);
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

static bool resolveImm5u(asm_state_t* state, const char* text, addr_t pos, FILE* debug) {
    int64_t value;
    shunting_status_t status = parseExp(&state->resolved, text, &value, debug);
    if(status == SHUNT_DONE) {
        if(!isBounded(value, 5, BOUND_UNSIGNED)) {
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
    if(reg == REG_X) {
        state->rom[state->rom_loc] = instr->opcode;
    } else if(reg == REG_Y) {
        state->rom[state->rom_loc] = instr->opcode | XY_MASK;
    } else {
        printf("Register %s not valid for ALU instruction\n", regnames[reg]);
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

const char *asm_mem(const instruction_t *instr, const char *text, asm_state_t *state) {
    memmode_t mm;
    const char* midptr = parseMemMode(text, &mm);
    if(!midptr) return NULL;
    state->rom[state->rom_loc] = instr->opcode | mm;
    while(isspace(*midptr)) midptr++;
    const char* endptr = addUnresolved(state, midptr, resolveImm4u);
    state->rom_loc ++;
    return endptr;
}

word_t getALUReg(cpu_state_t* cpu) {
    return peek(cpu, cpu->pc) & XY_MASK ? cpu->y : cpu->x;
}

addr_t getMemAddr(cpu_state_t* cpu) {
    word_t instr = peek(cpu, cpu->pc);
    switch(instr & MMODE_MASK) {
        addr_t tmp;
        case MM_STACK:
            return (0xFE00 | (cpu->int_active ? 0 : 0x100) | cpu->sp) + (instr & IMM4_MASK);
        case MM_P:
            return cpu->p + (instr & IMM4_MASK);
        case MM_Q:
            return cpu->q + (instr & IMM4_MASK);
        case MM_PPOST:
            tmp = cpu->p;
            cpu->p += (instr & IMM4_MASK);
            return tmp;        
    }
}

void dis_basic(const instruction_t* instr, cpu_state_t* cpu, addr_t addr, char* buf, size_t len) {
    snprintf(buf, len, "%s", instr->mnemonic);
}
void dis_register(const instruction_t* instr, cpu_state_t* cpu, addr_t addr, char* buf, size_t len) {
    snprintf(buf, len, "%s %s", instr->mnemonic, regnames[peek(cpu, addr) & REGID_MASK]);
}
void dis_alu_reg(const instruction_t* instr, cpu_state_t* cpu, addr_t addr, char* buf, size_t len) {
    snprintf(buf, len, "%s %s", instr->mnemonic,
            regnames[(peek(cpu, addr) & XY_MASK) ? REG_Y : REG_X]);
}
void dis_alu_imm(const instruction_t* instr, cpu_state_t* cpu, addr_t addr, char* buf, size_t len) {
    snprintf(buf, len, "%s %hhi", instr->mnemonic, peek(cpu, addr+1));
}
void dis_mem(const instruction_t* instr, cpu_state_t* cpu, addr_t addr, char* buf, size_t len) {
    word_t ibyte = peek(cpu, addr);
    const char* regname = mmnames[ibyte & MMODE_MASK];
    snprintf(buf, len, "%s %s %i", instr->mnemonic, regname, peek(cpu, addr) & IMM4_MASK);
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