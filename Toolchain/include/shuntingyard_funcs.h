#ifndef SHUNTINGYARD_FUNCS_H
#define SHUNTINGYARD_FUNCS_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct operator_token {
    const char* const str;
    int priority;
    size_t args;
    int64_t (*func)(int64_t*);
} operator_token_t;

enum { OPENPAREN, CLOSEDPAREN };
extern const operator_token_t operators[];
extern const size_t n_operators;

#endif