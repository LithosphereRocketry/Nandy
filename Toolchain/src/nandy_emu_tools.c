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

// Devices after 8 in this order won't get interrupt channels
static const iorange_t iomap[] = {
    {0, 1, io_step_intcontrol}
};
static const size_t n_ios = sizeof(iomap) / sizeof(iorange_t);
static word_t ioints;

bool io_step_intcontrol(cpu_state_t* cpu, bool active) {
    if(active && cpu->io_rd) { cpu->ioin = ioints; }
    return false;
}

// TODO: more flexibility
#define COOLDOWN (1000000 / 1200)
bool emu_step(cpu_state_t* state, FILE* outstream, iorange_t* iomap, size_t n_io) {
    static size_t lastIOcycle = 0;
    // instruction execute phase (up clock)
    state->io_rd = false;
    state->io_wr = false;
    const instruction_t* instr = ilookup(peek(state, state->pc));
    if(!instr) {
        printf("Invalid or unknown instruction byte 0x%hhx\n", peek(state, state->pc));
        return true;
    }
    instr->execute(state);

    for(size_t i = 0; i < n_io; i++) {
        bool inbounds = (state->ioaddr >= iomap[i].base 
                       & state->ioaddr < iomap[i].base + iomap[i].bound);
        bool interrupt = iomap[i].operation(state, inbounds);
        if(i >= 0 && i < 9) {
            
        }
    }
    

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
            state->int_in = true;
        }
        lastIOcycle = state->elapsed;
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