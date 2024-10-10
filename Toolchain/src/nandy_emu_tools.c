#include "nandy_emu_tools.h"

#include <stdbool.h>

#include "nandy_instructions.h"
#include "nandy_ios.h"

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
// TODO: what about devices that take up more than one interrupt channel?
static const iorange_t iomap[] = {
    {0, 1, io_step_tty}
};
static const size_t n_ios = sizeof(iomap) / sizeof(iorange_t);

// TODO: more flexibility
bool emu_step(cpu_state_t* state, FILE* outstream) {
    tty_outstream = outstream;
    // instruction execute phase (up clock)
    state->io_rd = false;
    state->io_wr = false;
    const instruction_t* instr = ilookup(peek(state, state->pc));
    if(!instr) {
        printf("Invalid or unknown instruction byte 0x%hhx\n", peek(state, state->pc));
        return true;
    }
    instr->execute(state);

    // I/O phase (down clock)
    word_t ioints = 0;
    state->ioin = 0;
    for(size_t i = 0; i < n_ios; i++) {
        bool inbounds = (state->ioaddr >= iomap[i].base 
                      && state->ioaddr < iomap[i].base + iomap[i].bound);
        bool interrupt = iomap[i].operation(state, inbounds);
        if(i < 8 && interrupt) {
            ioints |= 1 << i;
        }
    }
    state->int_in = (ioints != 0);
    if(state->ioaddr == 0x1f && state->io_rd) {
        state->ioin |= ioints;
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