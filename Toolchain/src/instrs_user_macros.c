#include "nandy_parse_tools.h"
#include "nandy_instr_defs.h"
#include <stdio.h>

// This is the big scary one
static const char* asm_macro_macro(const instruction_t* instr, const char* text, asm_state_t* state) {
    const char* end = endOfInput(text);
    printf("Macros not implemented!\n");
    return NULL;
}
const instruction_t i_macro_macro = {
    .mnemonic = "@macro",
    .assemble = asm_macro_macro
};