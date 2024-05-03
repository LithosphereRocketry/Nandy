#include "nandy_emu_tools.h"

#include <stdbool.h>

#include "nandy_instructions.h"
#include "stdin_avail.h"


const instruction_t* ilookup(word_t word) {
    static bool instrs_cached = false;
    static const instruction_t* cache[256];
    if(!instrs_cached) {
        for(int i = 0; i < 256; i++) {
            for(size_t j = 0; j < NANDY_ILIST.size; j++) {
                const instruction_t* instr = NANDY_ILIST.list[j];
                if((instr->opcode == (((word_t) i) & ~instr->opcode_mask)) && instr->execute) {
                    cache[i] = instr;
                    break;
                }
            }
        }
        instrs_cached = true;
    }

    return cache[(unsigned char) word];
}

// TODO: more flexibility
#define COOLDOWN (1000000 / 1200)
bool emu_step(cpu_state_t* state, FILE* outstream) {
    static size_t lastIOcycle = 0;
    // instruction execute phase (up clock)
    state->io_rd = false;
    state->io_wr = false;
    const instruction_t* instr = ilookup(peek(state, state->pc));
    if(!instr) {
        printf("Invalid or unknown instruction byte %hhx\n", peek(state, state->pc));
        return true;
    }
    instr->execute(state);


    // I/O phase (down clock)
    // TODO: this whole block should really be modularized
    if(state->io_rd) {
        // Experimenting with a new method of interrupt handling here
        state->int_in = false;
    }
    if(state->io_wr) {
        putc(state->ioout, outstream);
    }
    if(state->elapsed - lastIOcycle > COOLDOWN) {
        if(stdinAvail()) {
            state->ioin = getc(stdin);
            lastIOcycle = state->elapsed;
            state->int_in = true;
        }
    }
    // End of I/O block
    if(state->int_en && state->int_in && !state->int_active) {
        state->irx = state->pc & 0xFF;
        state->iry = (state->pc >> 8) & 0xFF;
        state->pc = ISR_ADDR;
        state->int_active = true;
    }
    return state->idbg;
}