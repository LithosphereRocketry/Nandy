#ifndef SHUNTINGYARD_FUNCS_H
#define SHUNTINGYARD_FUNCS_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct op_stack op_stack_t;
typedef struct operator_token {
    const char* const str;
    bool isUnary;
    int priority;
    int (*func)(op_stack_t*);
} operator_token_t;

// non-discriminated union because tokens will always be in one place and
// numbers will always be in another
typedef struct operator_token operator_token_t;
typedef union stack_element {
    const operator_token_t* token;
    int64_t value;
} stack_element_t;

typedef struct op_stack {
    stack_element_t* stack;
    size_t cap;
    size_t level;
} op_stack_t;

void op_push(op_stack_t* stack, stack_element_t element);
stack_element_t op_pop(op_stack_t* stack);

enum { OPENPAREN, CLOSEDPAREN };
extern const operator_token_t operators[];
extern const size_t n_operators;

#endif