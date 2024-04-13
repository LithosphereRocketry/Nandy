#include "nandy_tools.h"
#include "nandy_instructions.h"
#include "nandy_parse_tools.h"
#include <stdio.h>
#define __STDC_WANT_LIB_EXT2__ 1
#include <string.h>
#include <ctype.h>

// By default all the variables initialize to 0 which is what we want
// can add initialized values here later if we want
const cpu_state_t INIT_STATE = {};

const asm_state_t INIT_ASM = {
    .instrs = &NANDY_ILIST,

    .rom_loc = 0,
    .ram_loc = ADDR_RAM_MASK,

    .resolved = SYMTAB_INIT,

    .unresolved_sz = 0,
    .unresolved_cap = 0,
    .unresolved = NULL
};

const char* const regnames[5] = { "sp", "io", "dx", "dy", "acc" };

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
    symtab_put(&state->resolved, label, value);
    if(symtab_get(&state->resolved, label)) {
        return 0;
    } else {
        return -1;
    }
}

const char* addUnresolved(asm_state_t* state, const char* arg, inst_resolve_t func) {
    if(state->unresolved_sz >= state->unresolved_cap) {
        if(state->unresolved_cap == 0) {
            state->unresolved_cap = 1;
        } else {
            state->unresolved_cap *= 2;
        }
        state->unresolved = realloc(state->unresolved, 
                state->unresolved_cap*sizeof(unresolved_t));
    }
    const char* endloc = endOfInput(arg);
    state->unresolved[state->unresolved_sz].func = func;
    state->unresolved[state->unresolved_sz].location = state->rom_loc;
    state->unresolved[state->unresolved_sz].str = strndup(arg, endloc-arg);
    state->unresolved_sz ++;
    return endloc;
}
