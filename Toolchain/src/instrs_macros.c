#include "nandy_instr_defs.h"
#include "nandy_parse_tools.h"
#include "shuntingyard.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

static bool resolveEcho(asm_state_t* state, const char* text, addr_t) {
    int64_t value;
    shunting_status_t status = parseExp(&state->resolved, text, &value);
    if(status == SHUNT_DONE) {
        printf("%li\n", value);
        return true;
    } else {
        printf("Parse failed: %i\n", status);
        return false;
    }
}
static const char* asm_macro_echo(const instruction_t* instr, const char* text, asm_state_t* state) {
    return addUnresolved(state, text, resolveEcho);
}
const instruction_t i_macro_echo = {
    .mnemonic = "@echo",
    .assemble = asm_macro_echo
};

static bool resolveDefine(asm_state_t* state, const char* text, addr_t) {
    while(isspace(*text) && *text != '\n') text++;
    int64_t value;
    const char* division = parseFallback(text);
    shunting_status_t status = parseExp(&state->resolved, division, &value);
    if(status == SHUNT_DONE) {
        const char* lblName = strndup(text, division-text);
        addLabel(state, lblName, value);
        return true;
    } else {
        printf("Parse failed: %i\n", status);
        return false;
    }
}
static const char* asm_macro_define(const instruction_t* instr, const char* text, asm_state_t* state) {
    return addUnresolved(state, text, resolveDefine);
}
const instruction_t i_macro_define = {
    .mnemonic = "@define",
    .assemble = asm_macro_define
};

static bool resolveAssert(asm_state_t* state, const char* text, addr_t) {
    int64_t value;
    shunting_status_t status = parseExp(&state->resolved, text, &value);
    if(status == SHUNT_DONE) {
        if(value) {
            return true;
        } else {
            printf("Assertion failed (%s)\n", text);
            return false;
        }
    } else {
        printf("Parse failed: %i\n", status);
        return false;
    }
}
static const char* asm_macro_assert(const instruction_t* instr, const char* text, asm_state_t* state) {
    return addUnresolved(state, text, resolveAssert);
}
const instruction_t i_macro_assert = {
    .mnemonic = "@assert",
    .assemble = asm_macro_assert
};