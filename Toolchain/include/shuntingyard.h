#ifndef SHUNTINGYARD_H
#define SHUNTINGYARD_H

#include <stdint.h>

#include "symtab.h"

typedef enum {
    DONE,
    OPENPAREN,
    CLOSEPAREN
} shunting_status_t;

shunting_status_t parseExp(const symtab_t* symbols, const char* expstr, int64_t* result);

#endif