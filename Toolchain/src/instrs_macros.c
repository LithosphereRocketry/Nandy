#include "nandy_instr_defs.h"
#include "nandy_parse_tools.h"
#include "shuntingyard.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

static bool resolveEcho(asm_state_t* state, const char* text, addr_t, FILE* debug) {
    int64_t value;
    shunting_status_t status = parseExp(&state->resolved, text, &value, debug);
    if(status == SHUNT_DONE) {
        if(debug) fprintf(debug, "%s : %li\n", text, value);
        return true;
    } else {
        if(debug) fprintf(debug, "Parse failed: %i\n", status);
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

static bool resolveDefine(asm_state_t* state, const char* text, addr_t, FILE* debug) {
    while(isspace(*text) && *text != '\n') text++;
    int64_t value;
    const char* division = parseFallback(text);
    shunting_status_t status = parseExp(&state->resolved, division, &value, debug);
    if(status == SHUNT_DONE) {
        const char* lblName = strndup(text, division-text);
        addLabel(state, lblName, value);
        return true;
    } else {
        if(debug) fprintf(debug, "Parse failed: %i\n", status);
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

static bool resolveAssert(asm_state_t* state, const char* text, addr_t, FILE* debug) {
    int64_t value;
    shunting_status_t status = parseExp(&state->resolved, text, &value, debug);
    if(status == SHUNT_DONE) {
        if(value) {
            return true;
        } else {
            if(debug) fprintf(debug, "Assertion failed (%s)\n", text);
            return false;
        }
    } else {
        if(debug) fprintf(debug, "Parse failed: %i\n", status);
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

static const char* asm_macro_loc(const instruction_t* instr, const char* text, asm_state_t* state) {
    FILE* debug = stdout;
    int64_t value;
    char* arg = strndup(text, endOfInput(text)-text);
    shunting_status_t status = parseExp(&state->resolved, arg, &value, debug);
    if(status == SHUNT_DONE) {
        if(isBounded(value, 16, BOUND_UNSIGNED)) {
            if(value > state->rom_loc) {
                state->rom_loc = value;
                free(arg);
                return endOfInput(text);
            } else {
                if(debug) fprintf(debug, "Location 0x%lx (%s) is before current address %i\n",
                        value, text, state->rom_loc);
            }
        } else {
            if(debug) fprintf(debug, "Assertion failed (%s)\n", text);
        }
    } else {
        if(debug) fprintf(debug, "Parse failed: %i\n", status);
    }
    free(arg);
    return NULL;
}
const instruction_t i_macro_loc = {
    .mnemonic = "@loc",
    .assemble = asm_macro_loc
};

static const char* asm_macro_memloc(const instruction_t* instr, const char* text, asm_state_t* state) {
    FILE* debug = stdout;
    int64_t value;
    const char* end = endOfInput(text);
    char* arg = strndup(text, end-text);
    shunting_status_t status = parseExp(&state->resolved, arg, &value, debug);
    if(status == SHUNT_DONE) {
        if(isBounded(value, 16, BOUND_UNSIGNED)) {
            if(value > state->ram_loc) {
                state->ram_loc = value;
                return end;
            } else {
                if(debug) fprintf(debug, "Memory location 0x%lx (%s) is before current address %i\n",
                        value, arg, state->ram_loc);
                return NULL;
            }
        } else {
            if(debug) fprintf(debug, "Assertion failed (%s)\n", text);
            return NULL;
        }
    } else {
        if(debug) fprintf(debug, "Parse failed: %i\n", status);
        return NULL;
    }
}
const instruction_t i_macro_memloc = {
    .mnemonic = "@memloc",
    .assemble = asm_macro_memloc
};

static const char* asm_macro_static(const instruction_t* instr, const char* text, asm_state_t* state) {
    FILE* debug = stdout;
    while(isspace(*text) && *text != '\n') text++;
    const char* end = endOfInput(text);
    char* arg = strndup(text, end-text);
    int64_t value;
    const char* division = parseFallback(arg);
    shunting_status_t status = parseExp(&state->resolved, division, &value, debug);
    if(status == SHUNT_DONE) {
        if(value < 0) {
            if(debug) fprintf(debug, "Static size %li (%s) cannot be negative\n", value, division);
        } else if(state->ram_loc + value > 0xFF00) {
            if(debug) fprintf(debug, "Static allocation collides with stack\n");
        } else {
            const char* lblName = strndup(arg, division-arg);
            addLabel(state, lblName, state->ram_loc);
            state->ram_loc += value;
            return end;
        }
    } else {
        if(debug) fprintf(debug, "Parse failed: %i\n", status);
    }
    free(arg);
    return NULL;
}
const instruction_t i_macro_static = {
    .mnemonic = "@static",
    .assemble = asm_macro_static
};