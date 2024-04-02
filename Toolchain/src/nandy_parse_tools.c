#include <stdio.h>

#include "nandy_parse_tools.h"
#include "shuntingyard.h"

bool isBounded(int64_t value, int64_t bitwidth) {
    return value < (1 << bitwidth) && value >= -(1 << (bitwidth-1));
}

bool resolveImm8(asm_state_t* state, const char* text, addr_t pos) {
    int64_t value;
    shunting_status_t status = parseExp(&state->resolved, text, &value);
    if(status == SHUNT_DONE) {
        if(!isBounded(value, 8)) {
            printf("Warning: value of %s ( == %li) will be truncated\n", text, value);
        }
        state->rom[pos+1] = value;
        return true;
    } else {
        printf("Parse failed: %i\n", status);
        return false;
    }
}