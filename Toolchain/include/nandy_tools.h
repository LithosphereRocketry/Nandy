/**
 * NANDy C tooling library
 * 
 * This might be better split into separate emulation and assembly layers but
 * I'm unsure
 * 
 * Some benefit could be gained from being able to assemble & emulate in one
 * step; e.g. line numbers & comments in debug mode
*/

#ifndef TOOLS_H
#define TOOLS_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// Instruction bitmasks
#define ADDR_RAM_MASK (1<<15)
#define MULTICYCLE_MASK (1<<7)
#define ALU_SEL_MASK (1<<6)
#define ISP_MASK (1<<5)
#define XY_MASK (1<<5)
#define MEM_WRITE_MASK (1<<5)
#define JUMP_MASK (1<<5)
#define CARRY_SEL_MASK (1<<4)
#define PROGFLOW_MASK (1<<4)
#define MEM_STACK_MASK (1<<4)
#define COND_MASK (1<<4)
#define SIG_MASK (1<<3)
#define WR_MASK (1<<3)
#define RD_MASK (1<<2)
#define RET_MASK (1<<2)
#define CI_MASK (1<<1)

// Data sizes
typedef int8_t word_t;
typedef uint16_t addr_t;

// CPU state
typedef struct cpu_state {
    addr_t pc;
    word_t acc, sp, dx, dy, irx, iry, ioout;
    bool cycle, carry, int_en, int_active, int_prev;
    word_t rom[1 << 15];
    word_t ram[1 << 15];
} cpu_state_t;

// For convenience in the high-level emulator; note that the CPU registers WILL
// NOT be initialized with nice values like this most of the time
extern const cpu_state_t INIT_STATE;

word_t peek(const cpu_state_t* cpu, addr_t addr);
void poke(cpu_state_t* cpu, addr_t addr, word_t value);

addr_t nbytes(word_t inst);
size_t nclocks(word_t inst);

addr_t nextinst(const cpu_state_t* cpu, addr_t addr);
addr_t previnst(const cpu_state_t* cpu, addr_t addr);

void disasm(const cpu_state_t* cpu, addr_t addr, char* buf, size_t len);

#endif