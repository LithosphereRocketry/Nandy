/**
 * NANDy C tooling library
 * 
 * This might be better split into separate emulation and assembly layers but
 * I'm unsure
 * 
 * Some benefit could be gained from being able to assemble & emulate in one
 * step; e.g. line numbers & comments in debug mode
*/

#ifndef NANDY_TOOLS_H
#define NANDY_TOOLS_H

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
    word_t acc, sp, dx, dy, irx, iry, ioin, ioout;
    bool cycle, carry, int_en, int_active, int_prev, int_in;
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

// Some function pointer typedefs
typedef void (*inst_disassemble_t)(const cpu_state_t*, addr_t, char*, size_t);
typedef void (*inst_execute_t)(cpu_state_t*);

// Instruction definitions
typedef struct instruction {
    // Assembly
    const char* mnemonic;
    
    // Emulation / Disassembly
    const word_t opcode_mask;
    const word_t opcode;
    inst_disassemble_t disassemble;
    inst_execute_t execute;
} instruction_t;

typedef struct ilist {
    const size_t size;
    const instruction_t* list;
} ilist_t;

// Debugging tools
extern const char* const regnames[4];

#endif