#include "nandy_tools.h"
#include "nandy_instructions.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// By default all the variables initialize to 0 which is what we want
// can add initialized values here later if we want
const cpu_state_t INIT_STATE = {};

const asm_state_t INIT_ASM = {
    .instrs = &NANDY_ILIST,

    .cpu = INIT_STATE,
    .rom_loc = 0,
    .ram_loc = ADDR_RAM_MASK,

    .resolved_sz = 0,
    .resolved_cap = 0,
    .resolved = NULL,

    .unresolved_sz = 0,
    .unresolved_cap = 0,
    .unresolved = NULL
};

const char* const regnames[5] = { "sp", "io", "dx", "dy", "acc" };

// Internal tools
bool parity(word_t w) {
	// there's faster ways to do this but who cares it's 8 bits
    for(int i = 0; i < 8; i++) {
		w ^= w >> i;
	}
	return w & 1;
}


word_t peek(const cpu_state_t* cpu, addr_t addr) {
    return addr & ADDR_RAM_MASK ?
        cpu->ram[addr & ~ADDR_RAM_MASK]
      : cpu->rom[addr & ~ADDR_RAM_MASK];
}

void poke(cpu_state_t* cpu, addr_t addr, word_t value) {
    // Properly emulates real CPU's behavior on trying to write ROM
    cpu->ram[addr & ~ADDR_RAM_MASK] = value;
}

addr_t nbytes(word_t inst) {
    return (inst & MULTICYCLE_MASK) && (inst & ALU_SEL_MASK) ? 2 : 1;
}

size_t nclocks(word_t inst) {
    return (inst & MULTICYCLE_MASK) ? 2 : 1;
}

addr_t nextinst(const cpu_state_t* cpu, addr_t addr) {
    return addr + nbytes(peek(cpu, addr));
}

int addLabel(asm_state_t* state, const char* label, int64_t value) {
    if(!state->resolved) {
        state->resolved = malloc(sizeof(label_t));
        state->resolved_sz = 0;
        state->resolved_cap = 1;
    } else {
        if(state->resolved_sz >= state->resolved_cap) {
            state->resolved_cap *= 2;
            label_t* new_resolved = realloc(state->resolved,
                    state->resolved_cap * sizeof(label_t));
            if(new_resolved) {
                state->resolved = new_resolved;
            } else {
                return -1;
            }
        }
    }
    state->resolved[state->resolved_sz].name = label;
    state->resolved[state->resolved_sz].value = value;
    state->resolved_sz ++;
    return 0;
}

const char* parseFallback(const char* text) {
    while(!isspace(*text)) text++;
    return text;
}

const char* parseReg(const char* text, regid_t* dest) {
    while(isspace(*text) && *text != '\n') text++;
    for(int i = 0; i < sizeof(regnames)/sizeof(char*); i++) {
        if(strncmp(text, regnames[i], strlen(regnames[i])) == 0) {
            *dest = i;
            return text + strlen(regnames[i]);
        }
    }
    return NULL;
}