#include "nandy_parse_tools.h"
#include "nandy_check_tools.h"
#include "nandy_instr_defs.h"
#include "nandy_tools.h"

static addr_t getImm12(word_t opcode, word_t imm8) {
    return signExtend((((addr_t) opcode) << 8) | imm8, 12);
}

static int_state_t checkIntStateHelper(asm_state_t *state, addr_t pc, int_state_t status) {
    if(pc >= state->rom_loc) {
        return status;
    }
    
    word_t opcode = state->rom[pc];
    if((opcode & i_ja.opcode) == i_ja.opcode) {
        //TODO
        return checkIntStateHelper(state, pc + nbytes(opcode), status);
    } else if((opcode & i_jri.opcode) == i_jri.opcode) {
        //TODO
        return checkIntStateHelper(state, pc + nbytes(opcode), status);
    } else if((opcode & i_jar.opcode) == i_jar.opcode) {
        //TODO
        return checkIntStateHelper(state, pc + nbytes(opcode), status);
    } else if((opcode & i_j.opcode) == i_j.opcode) {
        addr_t offset = getImm12(opcode, state->rom[pc+1]);
        return checkIntStateHelper(state, pc + offset + 1, status);
    } else if((opcode & i_jcz.opcode) == i_jcz.opcode) {
        addr_t offset = getImm12(opcode, state->rom[pc+1]);
        int_state_t taken = checkIntStateHelper(state, pc + offset + 1, status);
        int_state_t notTaken = checkIntStateHelper(state, pc + nbytes(opcode), status);
        if(taken != notTaken) return INT_STATE_UNKNOWN;
        return taken;
    } else {
        return checkIntStateHelper(state, pc + nbytes(opcode), status);
    }
}

int_state_t checkIntState(asm_state_t *state) {
    return checkIntStateHelper(state, 0, INT_STATE_DISABLED);
}