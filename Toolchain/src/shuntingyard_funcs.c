#include "shuntingyard_funcs.h"

static int64_t f_uplus(int64_t* stack) { return stack[0]; }
static int64_t f_negate(int64_t* stack) { return -stack[0]; }
static int64_t f_mul(int64_t* stack) { return stack[1] * stack[0]; }
static int64_t f_div(int64_t* stack) { return stack[1] / stack[0]; }
static int64_t f_mod(int64_t* stack) { return stack[1] % stack[0]; }
static int64_t f_add(int64_t* stack) { return stack[1] + stack[0]; }
static int64_t f_sub(int64_t* stack) { return stack[1] - stack[0]; }

// Priorities based on C operator precedence
const operator_token_t operators[] = {
    // Open & closed parentheses should never actually evaluate
    [OPENPAREN] = {"(", 0, 1, NULL},
    [CLOSEDPAREN] = {")", 0, 0, NULL},
    {"+", 11, 1, f_uplus},
    {"-", 11, 1, f_negate},
    {"*", 10, 2, f_mul},
    {"/", 10, 2, f_div},
    {"%", 10, 2, f_mod},
    {"+", 9, 2, f_add},
    {"-", 9, 2, f_sub}
};
const size_t n_operators = sizeof(operators)/sizeof(operator_token_t);