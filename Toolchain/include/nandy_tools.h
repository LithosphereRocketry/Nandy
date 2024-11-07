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
#include <stdio.h>

#include "symtab.h"
#include "nandy_definitions.h"

// CPU state
typedef struct cpu_state {
    size_t elapsed;
    addr_t pc;
    word_t acc, sp, dx, dy, irx, iry, ioin, ioout, ioaddr;
    bool cycle, carry, int_en, int_active, int_in, io_rd, io_wr, idbg, cs;
    word_t rom[ROM_SIZE];
    word_t ram[RAM_SIZE];
} cpu_state_t;

// For convenience in the high-level emulator; note that the CPU registers WILL
// NOT be initialized with nice values like this most of the time
extern const cpu_state_t INIT_STATE;

typedef struct label {
    const char* name;
    int64_t value;
} label_t;

typedef struct instruction instruction_t;
typedef struct ilist {
    const size_t size;
    const instruction_t** list;
} ilist_t;

typedef enum int_state {
    // The default state upon init must be 0
    INT_STATE_DISABLED = 0,
    INT_STATE_ENABLED,
    INT_STATE_UNKNOWN,
} int_state_t;

typedef struct ctrl_block {
    addr_t block_pc;
    addr_t block_loc;
    
    int_state_t int_state;
    
    size_t next_idx;
    size_t branch_idx;
} ctrl_block_t;

typedef struct ctrl_graph {
    size_t block_sz;
    size_t block_cap;
    size_t current_idx;
    // An ordered, array-backed linked list of ctrl blocks
    ctrl_block_t* blocks;
} ctrl_graph_t;

typedef struct unresolved unresolved_t;
typedef struct asm_state {
    const ilist_t* instrs;

    ctrl_graph_t ctrl_graph;

    word_t rom[ROM_SIZE];
    addr_t rom_loc;
    addr_t ram_loc;
    
    symtab_t resolved;

    size_t unresolved_sz;
    size_t unresolved_cap;
    unresolved_t* unresolved;
} asm_state_t;

extern const asm_state_t INIT_ASM;
void asm_state_destroy(asm_state_t* state);

word_t peek(const cpu_state_t* cpu, addr_t addr);
void poke(cpu_state_t* cpu, addr_t addr, word_t value);

addr_t getImm12(word_t opcode, word_t imm8);

addr_t nbytes(word_t inst);
size_t nclocks(word_t inst);

addr_t nextinst(const cpu_state_t* cpu, addr_t addr);

// NOTE: takes ownership of label, which should be heap-allocated
int addLabel(asm_state_t* state, const char* label, int64_t value);

// Some function pointer typedefs
typedef void (*inst_disassemble_t)(const instruction_t*, cpu_state_t*, addr_t, char*, size_t);
typedef void (*inst_execute_t)(cpu_state_t*);
typedef const char* (*inst_assemble_t)(const instruction_t*, const char*, asm_state_t*);
typedef bool (*inst_resolve_t)(asm_state_t*, const char*, addr_t, FILE*);

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

typedef struct unresolved {
    addr_t location;
    char* str;
    inst_resolve_t func;
} unresolved_t;

const char* addUnresolved(asm_state_t* state, const char* arg, inst_resolve_t func);

#endif