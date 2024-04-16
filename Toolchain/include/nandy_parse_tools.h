#ifndef NANDY_PARSE_TOOLS_H
#define NANDY_PARSE_TOOLS_H

#include <stdint.h>
#include <stdbool.h>

#include "nandy_tools.h"

// Instruction parsing
const char* endOfInput(const char* str);
// Debugging tools
extern const char** regnames[];
extern const size_t n_regnames;
// Assembly & parsing tools
const char* parseFallback(const char* text);
const char* parseReg(const char* text, regid_t* dest);
const char* parseRegRequired(const char* text, regid_t* dest);

// Genericized assembly formats
const char* asm_basic(const instruction_t* instr, const char* text, asm_state_t* state);
const char* asm_register(const instruction_t* instr, const char* text, asm_state_t* state);
const char* asm_alu_reg(const instruction_t* instr, const char* text, asm_state_t* state);
const char* asm_alu_imm(const instruction_t* instr, const char* text, asm_state_t* state);
const char *asm_imm4s(const instruction_t *instr, const char *text, asm_state_t *state);
const char *asm_imm4u(const instruction_t *instr, const char *text, asm_state_t *state);

// Execution tools
word_t getXYReg(cpu_state_t* cpu, bool isY);
word_t getALUReg(cpu_state_t* cpu);
addr_t getAbsAddr(cpu_state_t* cpu);
addr_t getStackAddr(cpu_state_t* cpu);

// Disassembly tools
void dis_basic(const instruction_t* instr, cpu_state_t* cpu, addr_t addr, char* buf, size_t len);
void dis_register(const instruction_t* instr, cpu_state_t* cpu, addr_t addr, char* buf, size_t len);
void dis_alu_reg(const instruction_t* instr, cpu_state_t* cpu, addr_t addr, char* buf, size_t len);
void dis_alu_imm(const instruction_t* instr, cpu_state_t* cpu, addr_t addr, char* buf, size_t len);
void dis_imm4s(const instruction_t* instr, cpu_state_t* cpu, addr_t addr, char* buf, size_t len);
void dis_imm4u(const instruction_t* instr, cpu_state_t* cpu, addr_t addr, char* buf, size_t len);

// Verification
typedef enum {
    BOUND_UNSIGNED,
    BOUND_SIGNED,
    BOUND_EITHER
} bound_mode_t;
bool isBounded(int64_t value, int64_t bitwidth, bound_mode_t bound);
int64_t signExtend(int64_t value, int64_t bitwidth);

// Label resolution
// bool resolveImm4s(asm_state_t* state, const char* text, addr_t pos);
// bool resolveImm8(asm_state_t* state, const char* text, addr_t pos);

#endif