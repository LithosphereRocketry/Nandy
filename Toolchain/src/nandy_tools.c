#include "tools.h"

// By default all the variables initialize to 0 which is what we want
// can add initialized values here later if we want
const cpu_state_t INIT_STATE = {};

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

addr_t previnst(const cpu_state_t* cpu, addr_t addr) {
    /**
     * This function is going to be really nasty without debug symbols but
     * that's really useful functionality to have; because of some really nasty
     * possible programming techniques, it may not be resolvable *at all*.
     * Basically, instruction misalignment can create some really screwy
     * bytecode geometry problems that mean working backwards from a given
     * location 
     * 
     * Ex:
     * 
     * 0        rdi
     * 1    l:  j 0x0..
     * 2        wr sp
     * 3        wr dx
     * 4        rd sp
     * 5        add dx
     * 6        addi 65
     * 7        ...
     * 8        j l
     * 9        ...
     * 10       wr io
     * 11       brk
    */
}
