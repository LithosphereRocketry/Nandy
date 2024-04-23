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

static const char* asm_goto(const instruction_t* instr, const char* text, asm_state_t* state) {
    const char* end = i_rda.assemble(&i_rda, text, state);
    i_ja.assemble(&i_ja, "", state);
    return end;
}
const instruction_t i_goto = {
    .mnemonic = "goto",
    .assemble = asm_goto
};

static const char* asm_move(const instruction_t* instr, const char* text, asm_state_t* state) {
    regid_t a, b;
    text = parseReg(text, &a);
    if(!text) return NULL;
    text = parseReg(text, &b);
    if(!text) return NULL;
    if(a != b) {
        if(b == REG_ACC) {
            i_rd.assemble(&i_rd, regnames[a][0], state);
        } else if(a == REG_ACC) {
            i_wr.assemble(&i_wr, regnames[b][0], state);
        } else if(a == REG_IO) {
            i_sw.assemble(&i_sw, regnames[b][0], state);
            i_rd.assemble(&i_rd, regnames[a][0], state);
            i_sw.assemble(&i_sw, regnames[b][0], state);
        } else {
            i_sw.assemble(&i_sw, regnames[a][0], state);
            i_wr.assemble(&i_wr, regnames[b][0], state);
            i_sw.assemble(&i_sw, regnames[a][0], state);
        }
    }
    return text;
}
const instruction_t i_move = {
    .mnemonic = "move",
    .assemble = asm_move
};