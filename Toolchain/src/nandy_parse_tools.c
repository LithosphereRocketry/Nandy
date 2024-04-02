#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "nandy_parse_tools.h"
#include "shuntingyard.h"

const char* endOfInput(const char* str) {
    const char* ptr = str;
    while(*ptr != '\0' && *ptr != '\n' && strncmp(ptr, COMMENT_TOK, strlen(COMMENT_TOK))) {
        ptr++;
    }
    return ptr;
}

const char* parseFallback(const char* text) {
    while(!isspace(*text)) text++;
    return text;
}

const char* parseReg(const char* text, regid_t* dest) {
    while(isspace(*text) && *text != '\n') text++;
    for(size_t i = 0; i < sizeof(regnames)/sizeof(char*); i++) {
        if(strncmp(text, regnames[i], strlen(regnames[i])) == 0) {
            *dest = i;
            return text + strlen(regnames[i]);
        }
    }
    return NULL;
}

const char* parseRegRequired(const char* text, regid_t* dest) {
    const char* after = parseReg(text, dest);
    if(!after) {
        after = parseFallback(text);
        if(after == text) {
            printf("No register name provided\n");
        } else {
            printf("Unrecognized register name \"%.*s\"\n", (int) (after-text), text);
        }
        return NULL;
    }
    return after;
}

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