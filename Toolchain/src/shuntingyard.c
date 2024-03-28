#include "shuntingyard.h"

static shunting_status_t parseHelper(const symtab_t* symbols, const char* expstr,
        int64_t* result, const char** end) {
    if(1) {

    }
    return DONE;
}

shunting_status_t parseExp(const symtab_t* symbols, const char* expstr, int64_t* result) {
    return parseHelper(symbols, expstr, result, NULL);
}