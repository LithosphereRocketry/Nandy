#ifndef NANDY_PARSE_TOOLS_H
#define NANDY_PARSE_TOOLS_H

#include <stdint.h>
#include <stdbool.h>

#include "nandy_tools.h"

// Instruction parsing
const char* endOfInput(const char* str);
// Debugging tools
extern const char* const regnames[5];
// Assembly & parsing tools
const char* parseFallback(const char* text);
const char* parseReg(const char* text, regid_t* dest);
const char* parseRegRequired(const char* text, regid_t* dest);

// Genericized assembly formats
const char* asm_alu_reg(const char* text, asm_state_t* state, alu_mode_t m);

// Verification
bool isBounded(int64_t value, int64_t bitwidth);

// Label resolution
bool resolveImm8(asm_state_t* state, const char* text, addr_t pos);

#endif