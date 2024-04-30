#include "nandy_emu_tools.h"

#include <stdbool.h>

#include "nandy_instructions.h"


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

bool emu_step(cpu_state_t* state, FILE* outstream) {
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
    if(state->io_wr) {
        putc(state->ioout, outstream);
    }
    return state->idbg;
}