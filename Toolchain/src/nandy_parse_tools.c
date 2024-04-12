#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "nandy_parse_tools.h"
#include "shuntingyard.h"

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
    for(size_t i = 0; i < sizeof(regnames)/sizeof(char*); i++) {
        if(strncmp(text, regnames[i], strlen(regnames[i])) == 0) {
            *dest = i;
            return text + strlen(regnames[i]);
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

const char *asm_imm4s(const instruction_t *instr, const char *text, asm_state_t *state) {
    state->rom[state->rom_loc] = instr->opcode;
    const char* endptr = addUnresolved(state, text, resolveImm4s);
}

word_t getALUReg(cpu_state_t* cpu) {
    return (peek(cpu, cpu->pc) & XY_MASK) ? cpu->dy : cpu->dx;
}

void dis_basic(const instruction_t* instr, cpu_state_t* cpu, addr_t addr, char* buf, size_t len) {
    snprintf(buf, len, "%s", instr->mnemonic);
}
void dis_register(const instruction_t* instr, cpu_state_t* cpu, addr_t addr, char* buf, size_t len) {
    snprintf(buf, len, "%s %s", instr->mnemonic, regnames[peek(cpu, addr) & 0b11]);
}
void dis_alu_reg(const instruction_t* instr, cpu_state_t* cpu, addr_t addr, char* buf, size_t len) {
    snprintf(buf, len, "%s %s", instr->mnemonic,
            regnames[(peek(cpu, addr) & XY_MASK) ? REG_DY : REG_DX]);
}
void dis_alu_imm(const instruction_t* instr, cpu_state_t* cpu, addr_t addr, char* buf, size_t len) {
    snprintf(buf, len, "%s %hhi", instr->mnemonic, peek(cpu, addr+1));
}
void dis_imm4s(const instruction_t* instr, cpu_state_t* cpu, addr_t addr, char* buf, size_t len) {
    snprintf(buf, len, "%s %hhi", instr->mnemonic, signExtend(peek(cpu, addr), 4));
}


bool isBounded(int64_t value, int64_t bitwidth, bound_mode_t bound) {
    switch(bound) {
        case BOUND_SIGNED:
            return value < (1 << (bitwidth-1)) && value >= -(1 << (bitwidth-1));
        case BOUND_UNSIGNED:
            return value < (1 << bitwidth) && value >= 0;
        case BOUND_EITHER:
            return value < (1 << bitwidth) && value >= -(1 << (bitwidth-1));
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

bool resolveImm4s(asm_state_t* state, const char* text, addr_t pos) {
    int64_t value;
    shunting_status_t status = parseExp(&state->resolved, text, &value);
    if(status == SHUNT_DONE) {
        if(!isBounded(value, 4, BOUND_SIGNED)) {
            printf("Warning: value of %s ( == %li) will be truncated\n", text, value);
        }
        state->rom[pos] |= value & IMM4_MASK;
        return true;
    } else {
        printf("Parse failed: %i\n", status);
        return false;
    }
}

bool resolveImm8(asm_state_t* state, const char* text, addr_t pos) {
    int64_t value;
    shunting_status_t status = parseExp(&state->resolved, text, &value);
    if(status == SHUNT_DONE) {
        if(!isBounded(value, 8, BOUND_EITHER)) {
            printf("Warning: value of %s ( == %li) will be truncated\n", text, value);
        }
        state->rom[pos+1] = value;
        return true;
    } else {
        printf("Parse failed: %i\n", status);
        return false;
    }
}