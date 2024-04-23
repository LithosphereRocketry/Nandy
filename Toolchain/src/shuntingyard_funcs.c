#include "shuntingyard_funcs.h"

static int64_t f_uplus(int64_t* stack) { return stack[0]; }
static int64_t f_negate(int64_t* stack) { return -stack[0]; }
static int64_t f_not(int64_t* stack) { return !stack[0] ? 1 : 0; }
static int64_t f_bitnot(int64_t* stack) { return ~stack[0]; }

static int64_t f_mul(int64_t* stack) { return stack[1] * stack[0]; }
static int64_t f_div(int64_t* stack) { return stack[1] / stack[0]; }
static int64_t f_mod(int64_t* stack) { return stack[1] % stack[0]; }

static int64_t f_add(int64_t* stack) { return stack[1] + stack[0]; }
static int64_t f_sub(int64_t* stack) { return stack[1] - stack[0]; }

static int64_t f_sl(int64_t* stack) { return stack[1] << stack[0]; }
static int64_t f_sr(int64_t* stack) { return stack[1] >> stack[0]; }

static int64_t f_gt(int64_t* stack) { return stack[1] > stack[0] ? 1 : 0; }
static int64_t f_ge(int64_t* stack) { return stack[1] >= stack[0] ? 1 : 0; }
static int64_t f_lt(int64_t* stack) { return stack[1] < stack[0] ? 1 : 0; }
static int64_t f_le(int64_t* stack) { return stack[1] >= stack[0] ? 1 : 0; }

static int64_t f_eq(int64_t* stack) { return stack[1] == stack[0] ? 1 : 0; }
static int64_t f_ne(int64_t* stack) { return stack[1] != stack[0] ? 1 : 0; }

static int64_t f_bitand(int64_t* stack) { return stack[1] & stack[0]; }

static int64_t f_bitxor(int64_t* stack) { return stack[1] ^ stack[0]; }

static int64_t f_bitor(int64_t* stack) { return stack[1] | stack[0]; }

static int64_t f_and(int64_t* stack) { return stack[1] && stack[0] ? 1 : 0; }

static int64_t f_or(int64_t* stack) { return stack[1] || stack[0] ? 1 : 0; }

// Priorities based on C operator precedence
// Because I don't feel like greedily searching for the longest valid operator,
// operators that start with another operator (e.g. << and <) must be in order
// from longest to shortest
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
    {"-", 9, 2, f_sub},
    {">>", 8, 2, f_sr},
    {">=", 7, 2, f_ge},
    {">", 7, 2, f_gt},
    {"<<", 8, 2, f_sl},
    {"<=", 7, 2, f_le},
    {"<", 7, 2, f_lt},
    {"==", 6, 2, f_eq},
    {"!=", 6, 2, f_ne},
    {"!", 11, 1, f_not},
    {"~", 11, 1, f_bitnot},
    {"^", 4, 2, f_bitxor},
    {"||", 1, 2, f_or},
    {"|", 3, 2, f_bitor},
    {"&&", 2, 2, f_and},
    {"&", 5, 2, f_bitand}
};
const size_t n_operators = sizeof(operators)/sizeof(operator_token_t);