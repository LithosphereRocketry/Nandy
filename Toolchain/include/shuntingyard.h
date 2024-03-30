#ifndef SHUNTINGYARD_H
#define SHUNTINGYARD_H

#include <stdint.h>

#include "symtab.h"

typedef enum {
    SHUNT_DONE,
    SHUNT_TOO_MANY_VALUES,
    SHUNT_TOO_FEW_VALUES,
    SHUNT_UNRECOGNIZED,
    SHUNT_UNIMPLEMENTED
} shunting_status_t;

shunting_status_t parseExp(const symtab_t* symbols, const char* expstr, int64_t* result);

#endif