#include "nandy_parse_tools.h"
#include "nandy_instr_defs.h"
#include "shuntingyard.h"
#include <stdio.h>

static bool resolveRda(asm_state_t* state, const char* text, addr_t pos, FILE* debug) {
    int64_t value;
    shunting_status_t status = parseExp(&state->resolved, text, &value, debug);
    if(status == SHUNT_DONE) {
        if(!isBounded(value, 16, BOUND_UNSIGNED)) {
            if(debug) fprintf(debug, "Error: address %s is out of range! (%li)\n", text, value);
            return false;
        }
        state->rom[pos+5] = (value >> 8) & 0xFF;
        state->rom[pos+2] = value & 0xFF;
        return true;
    } else {
        if(debug) fprintf(debug, "Parse failed: %i\n", status);
        return false;
    }
}
static const char* asm_rda(const instruction_t* instr, const char* text, asm_state_t* state) {
    // An instruction with a null argument skips label resolution, since we're
    // going to be doing that manually
    const char* end = addUnresolved(state, text, resolveRda);
    i_wr.assemble(&i_wr, regnames[REG_DY][0], state);
    i_rdi.assemble(&i_rdi, NULL, state);
    i_wr.assemble(&i_wr, regnames[REG_DX][0], state);
    i_rdi.assemble(&i_rdi, NULL, state);
    i_sw.assemble(&i_sw, regnames[REG_DY][0], state);
    return end;
}
const instruction_t i_rda = {
    .mnemonic = "rda",
    .assemble = asm_rda
};

static const char* asm_call(const instruction_t* instr, const char* text, asm_state_t* state) {
    const char* end = i_rda.assemble(&i_rda, text, state);
    i_jar.assemble(&i_jar, "", state);
    return end;
}
const instruction_t i_call = {
    .mnemonic = "call",
    .assemble = asm_call
};