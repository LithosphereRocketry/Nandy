#ifndef NANDY_PARSE_TOOLS_H
#define NANDY_PARSE_TOOLS_H

#include <stdint.h>
#include <stdbool.h>

#include "nandy_tools.h"

// Verification
bool isBounded(int64_t value, int64_t bitwidth);

// Label resolution
bool resolveImm8(asm_state_t* state, const char* text, addr_t pos);

#endif