#include "shuntingyard_funcs.h"

void op_push(op_stack_t* stack, stack_element_t element) {
    if(stack->level >= stack->cap) {
        if(stack->cap == 0) {
            stack->cap = 1;
        } else {
            stack->cap *= 2;
        }
        stack->stack = realloc(stack->stack, stack->cap * sizeof(stack_element_t));
    }
    stack->stack[stack->level] = element;
    stack->level ++;
}

stack_element_t op_pop(op_stack_t* stack) {
    stack->level --;
    return stack->stack[stack->level];
}

int f_add(op_stack_t* stack) {
    if(stack->level < 2) { return -1; }
    int64_t a = op_pop(stack).value;
    int64_t b = op_pop(stack).value;
    stack_element_t element = { .value = a + b };
    op_push(stack, element);
    return 0;
}

int f_sub(op_stack_t* stack) {
    if(stack->level < 2) { return -1; }
    int64_t a = op_pop(stack).value;
    int64_t b = op_pop(stack).value;
    stack_element_t element = { .value = b - a };
    op_push(stack, element);
    return 0;
}

int f_negate(op_stack_t* stack) {
    if(stack->level < 1) { return -1; }
    int64_t a = op_pop(stack).value;
    stack_element_t element = { .value = -a };
    op_push(stack, element);
    return 0;
}

const operator_token_t operators[] = {
    [OPENPAREN] = {"(", true, 0, NULL},
    [CLOSEDPAREN] = {")", false, 0, NULL},
    {"+", false, 9, f_add},
    {"-", false, 9, f_sub},
    {"-", true, 9, f_negate}
};
const size_t n_operators = sizeof(operators)/sizeof(operator_token_t);