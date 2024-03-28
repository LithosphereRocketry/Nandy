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
#include <stdlib.h>

#include "symtab.h"
#include "nandy_definitions.h"

// CPU state
typedef struct cpu_state {
    addr_t pc;
    word_t acc, sp, dx, dy, irx, iry, ioin, ioout;
    bool cycle, carry, int_en, int_active, int_prev, int_in;
    word_t rom[ROM_SIZE];
    word_t ram[RAM_SIZE];
} cpu_state_t;

// For convenience in the high-level emulator; note that the CPU registers WILL
// NOT be initialized with nice values like this most of the time
extern const cpu_state_t INIT_STATE;

typedef struct unresolved {
    addr_t location;
    char* str;
} unresolved_t;

typedef struct label {
    const char* name;
    int64_t value;
} label_t;

typedef struct instruction instruction_t;
typedef struct ilist {
    const size_t size;
    const instruction_t** list;
} ilist_t;

typedef struct asm_state {
    const ilist_t* instrs;

    cpu_state_t cpu;
    addr_t rom_loc;
    addr_t ram_loc;
    
    symtab_t resolved;

    size_t unresolved_sz;
    size_t unresolved_cap;
    unresolved_t* unresolved;
} asm_state_t;

extern const asm_state_t INIT_ASM;

word_t peek(const cpu_state_t* cpu, addr_t addr);
void poke(cpu_state_t* cpu, addr_t addr, word_t value);

addr_t nbytes(word_t inst);
size_t nclocks(word_t inst);

addr_t nextinst(const cpu_state_t* cpu, addr_t addr);

// NOTE: takes ownership of label, which should be heap-allocated
int addLabel(asm_state_t* state, const char* label, int64_t value);

// Some function pointer typedefs
typedef void (*inst_disassemble_t)(const cpu_state_t*, addr_t, char*, size_t);
typedef void (*inst_execute_t)(cpu_state_t*);
typedef const char* (*inst_assemble_t)(const char*, asm_state_t*);

// Instruction definitions
typedef struct instruction {
    // Assembly
    const char* mnemonic;
    
    // Emulation / Disassembly
    const word_t opcode_mask;
    const word_t opcode;
    inst_assemble_t assemble;
    inst_disassemble_t disassemble;
    inst_execute_t execute;
} instruction_t;

// Debugging tools
extern const char* const regnames[5];
// Assembly & parsing tools
const char* parseFallback(const char* text);
const char* parseReg(const char* text, regid_t* dest);
const char* parseRegRequired(const char* text, regid_t* dest);

#endif