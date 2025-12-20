#include "nandy_emu_tools.h"

#include <stdbool.h>

#include "nandy_instructions.h"
#include "stdin_avail.h"
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

bool io_step_intcontrol(cpu_state_t* cpu, bool active) {
    if(active && cpu->io_rd) { cpu->ioin = cpu->ints_in; }
    return false;
}

// Devices after 8 in this order won't get interrupt channels
static const iorange_t iomap[] = {
    {0x00, 8, io_step_tty},
};
static const size_t n_ios = sizeof(iomap) / sizeof(iorange_t);

// TODO: more flexibility
bool emu_step(cpu_state_t* state, FILE* outstream/*, iorange_t* iomap, size_t n_ios*/) {
    // instruction execute phase (up clock)
    state->io_rd = false;
    state->io_wr = false;
    const instruction_t* instr = ilookup(peek(state, state->pc));
    if(!instr) {
        printf("Invalid or unknown instruction byte 0x%hhx\n", peek(state, state->pc));
        return true;
    }
    instr->execute(state);

    for(size_t i = 0; i < n_ios; i++) {
        bool inbounds = (state->y >= iomap[i].base 
                      && state->y < iomap[i].base + iomap[i].bound);
        bool interrupt = iomap[i].operation(state, inbounds);
        if(i < 8) {
            if(interrupt) {
                state->ints_in |= 1 << i;
            } else {
                state->ints_in &= ~(1 << i);
            }
        }
    }
    
    if(state->int_en && state->ints_in && !state->int_active) {
        state->ia = state->pc;
        state->pc = ISR_ADDR;
        state->int_active = true;
    }
    return state->idbg;
}