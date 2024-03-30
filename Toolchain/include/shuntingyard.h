#ifndef SHUNTINGYARD_H
#define SHUNTINGYARD_H

#include <stdint.h>

#include "symtab.h"

typedef enum {
    SHUNT_DONE,
    SHUNT_NO_VALUES,
    SHUNT_MISSING_VALUE,
    SHUNT_TOO_MANY_POP,
    SHUNT_TOO_MANY_POP_PAREN,
    SHUNT_MISMATCHED_OPEN,
    SHUNT_MISMATCHED_CLOSE,
    SHUNT_UNRECOGNIZED
} shunting_status_t;

shunting_status_t parseExp(const symtab_t* symbols, const char* expstr, int64_t* result);

#endif