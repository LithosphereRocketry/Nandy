#include "nandy_parse_tools.h"
#include "nandy_instr_defs.h"
#include "shuntingyard.h"
#include <stdio.h>

static bool resolveRdp(asm_state_t* state, const char* text, addr_t pos, FILE* debug) {
    int64_t value;
    shunting_status_t status = parseExp(&state->resolved, text, &value, debug);
    if(status == SHUNT_DONE) {
        if(!isBounded(value, 16, BOUND_UNSIGNED)) {
            if(debug) fprintf(debug, "Error: address %s is out of range! (%li)\n", text, value);
            return false;
        }
        state->rom[pos+5] = (value >> 8) & 0xFF;
        state->rom[pos+2] = value & 0xFF;
        printf("%lx %hhx %hhx\n", value, state->rom[pos+5], state->rom[pos+2]);
        return true;
    } else {
        if(debug) fprintf(debug, "Parse failed: %i\n", status);
        return false;
    }
}
static const char* asm_rdp(const instruction_t* instr, const char* text, asm_state_t* state) {
    // An instruction with a null argument skips label resolution, since we're
    // going to be doing that manually
    const char* end = addUnresolved(state, text, resolveRdp);
    instr_assemble(&i_wr, regnames[REG_PH], state);
    instr_assemble(&i_rdi, NULL, state);
    instr_assemble(&i_wr, regnames[REG_PL], state);
    instr_assemble(&i_rdi, NULL, state);
    instr_assemble(&i_sw, regnames[REG_PH], state);
    return end;
}
const instruction_t i_rdp = {
    .mnemonic = "rdp",
    .assemble = asm_rdp
};

static const char* asm_call(const instruction_t* instr, const char* text, asm_state_t* state) {
    const char* end = instr_assemble(&i_rdp, text, state);
    instr_assemble(&i_jpr, "", state);
    return end;
}
const instruction_t i_call = {
    .mnemonic = "call",
    .assemble = asm_call
};

static const char* asm_goto(const instruction_t* instr, const char* text, asm_state_t* state) {
    const char* end = instr_assemble(&i_rdp, text, state);
    instr_assemble(&i_jp, "", state);
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
            instr_assemble(&i_rd, regnames[a], state);
        } else if(a == REG_ACC) {
            instr_assemble(&i_wr, regnames[b], state);
        } else if(a == REG_IO) {
            instr_assemble(&i_sw, regnames[b], state);
            instr_assemble(&i_rd, regnames[a], state);
            instr_assemble(&i_sw, regnames[b], state);
        } else {
            instr_assemble(&i_sw, regnames[a], state);
            instr_assemble(&i_wr, regnames[b], state);
            instr_assemble(&i_sw, regnames[a], state);
        }
    }
    return text;
}
const instruction_t i_move = {
    .mnemonic = "move",
    .assemble = asm_move
};

static const char* asm_swap(const instruction_t* instr, const char* text, asm_state_t* state) {
    regid_t a, b;
    text = parseReg(text, &a);
    if(!text) return NULL;
    text = parseReg(text, &b);
    if(!text) return NULL;
    if(a != b) {
        if(b == REG_ACC) {
            instr_assemble(&i_sw, regnames[a], state);
        } else if(a == REG_ACC) {
            instr_assemble(&i_sw, regnames[b], state);
        } else if(a == REG_IO) {
            instr_assemble(&i_sw, regnames[b], state);
            instr_assemble(&i_sw, regnames[a], state);
            instr_assemble(&i_sw, regnames[b], state);
        } else {
            instr_assemble(&i_sw, regnames[a], state);
            instr_assemble(&i_sw, regnames[b], state);
            instr_assemble(&i_sw, regnames[a], state);
        }
    }
    return text;
}
const instruction_t i_swap = {
    .mnemonic = "swap",
    .assemble = asm_swap
};

static const char* asm_isc(const instruction_t* instr, const char* text, asm_state_t* state) {
    instr_assemble(&i_sw, regnames[REG_SP], state);
    text = i_adci.assemble(&i_adci, text, state);
    instr_assemble(&i_sw, regnames[REG_SP], state);
    return text;
}
const instruction_t i_isc = {
    .mnemonic = "isc",
    .assemble = asm_isc
};

static const char* asm_isp(const instruction_t* instr, const char* text, asm_state_t* state) {
    instr_assemble(&i_sw, regnames[REG_SP], state);
    text = instr_assemble(&i_addi, text, state);
    instr_assemble(&i_sw, regnames[REG_SP], state);
    return text;
}
const instruction_t i_isp = {
    .mnemonic = "isp",
    .assemble = asm_isp
};

// This is a fairly wasteful implementation but at this point saving effort is
// a lot more important than saving bytes
static const char* asm_pusha(const instruction_t* instr, const char* text, asm_state_t* state) {
    i_isc.assemble(&i_isc, "-2", state);

    state->rom_loc--;
    const char* end = addUnresolved(state, text, resolveRdp);
    state->rom_loc++;
    instr_assemble(&i_rdi, NULL, state);
    instr_assemble(&i_st, "sp 0", state);
    instr_assemble(&i_rdi, NULL, state);
    instr_assemble(&i_st, "sp 1", state);
    return end;
}
const instruction_t i_pusha = {
    .mnemonic = "pusha",
    .assemble = asm_pusha,
};