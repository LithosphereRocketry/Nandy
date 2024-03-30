#include "shuntingyard.h"
#include "shuntingyard_funcs.h"

#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

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

static void op_push(op_stack_t* stack, stack_element_t element) {
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

static stack_element_t op_pop(op_stack_t* stack) {
    stack->level --;
    return stack->stack[stack->level];
}

static const struct {
    const char* const prefix;
    const int64_t base;
} prefixes[] = {
    {"0x", 16},
    {"0X", 16},
    {"0b", 2},
    {"0B", 2},
    {"0o", 8},
    {"0O", 8}
};
static const size_t n_prefixes = sizeof(prefixes)/sizeof(*prefixes);

int64_t charToNumeral(char val) {
    if(val >= '0' && val <= '9') {
        return val - '0';
    } else if(val >= 'a' && val <= 'z') {
        return val - 'a' + 0xa;
    } else if(val >= 'A' && val <= 'Z') {
        return val - 'A' + 0xa;
    } else {
        return -1;
    }
}

const char* parseNumeric(const char* expstr, int64_t base, int64_t* value) {
    int tmp = 0;
    const char* ptr = expstr;
    while(1) {
        int64_t digit = charToNumeral(*ptr);
        if(digit >= 0 && digit < base) {
            tmp *= base;
            tmp += digit;
            ptr++;
        } else if(digit >= 0) {
            return NULL;
        } else {
            if(ptr != expstr) {
                *value = tmp;
                return ptr;
            } else {
                return NULL;
            }
        }
    }
}


const char* parseInt(const char* expstr, stack_element_t* out) {
    for(size_t i = 0; i < n_prefixes; i++) {
        if(!strncmp(expstr, prefixes[i].prefix, strlen(prefixes[i].prefix))) {
            return parseNumeric(expstr+strlen(prefixes[i].prefix),
                    prefixes[i].base, &out->value);
        }
    }
    return parseNumeric(expstr, 10, &out->value);
}

const char* parseOperator(const char* expstr, stack_element_t* out, bool unary) {
    for(size_t i = 0; i < n_operators; i++) {
        if(unary == (operators[i].args == 1)
                && !strncmp(expstr, operators[i].str, strlen(operators[i].str))) {
            out->token = &operators[i];
            return expstr + strlen(operators[i].str);
        }
    }
    return NULL;
}

int evalOperator(op_stack_t* stack, const operator_token_t* op) {
    if(op->func == NULL) { return -1; }
    if(stack->level < op->args) { return -2; }
    stack_element_t element;
    int64_t* values = malloc(op->args * sizeof(int64_t));
    for(size_t i = 0; i < op->args; i++) {
        values[i] = op_pop(stack).value;
    }
    element.value = op->func(values);
    op_push(stack, element);
    free(values);
    return 0;
}

// Based on: https://en.wikipedia.org/wiki/Shunting_yard_algorithm
// and: https://web.archive.org/web/20110718214204/http://en.literateprograms.org/Shunting_yard_algorithm_(C)
shunting_status_t parseExp(const symtab_t* symbols, const char* expstr, int64_t* result) {
    op_stack_t operator_stack = {NULL, 0, 0};
    op_stack_t value_stack = {NULL, 0, 0};
    const char* ptr = expstr;
    shunting_status_t exitcode = SHUNT_DONE;
    bool lastNumber = false;
    while(*ptr != '\0' && exitcode == SHUNT_DONE) {
        stack_element_t element;
        const char* next;
        if(isspace(*ptr)) {
            ptr++;
        } else if(!lastNumber && (next = parseInt(ptr, &element))) {
            op_push(&value_stack, element);
            ptr = next;
            lastNumber = true;
        } else if((next = parseOperator(ptr, &element, !lastNumber))) {
            if(element.token == &operators[OPENPAREN]) {
                op_push(&operator_stack, element);
                lastNumber = false;
            } else if(element.token == &operators[CLOSEDPAREN]) {
                lastNumber = true;
                while(1) {
                    if(operator_stack.level <= 0) {
                        printf("No matching left parenthesis\n");
                        exitcode = SHUNT_MISMATCHED_CLOSE;
                        break;
                    }
                    stack_element_t op = op_pop(&operator_stack);
                    if(op.token == &operators[OPENPAREN]) break;
                    if(evalOperator(&value_stack, op.token)) {
                        exitcode = SHUNT_TOO_MANY_POP_PAREN;
                        break;
                    }
                }
            } else {
                lastNumber = false;
                while(operator_stack.level > 0) {
                    stack_element_t op = op_pop(&operator_stack);
                    if(op.token->priority < element.token->priority) {
                        op_push(&operator_stack, op);
                        break;
                    }
                    if(evalOperator(&value_stack, op.token)) {
                        exitcode = SHUNT_TOO_MANY_POP;
                        break;
                    }
                }
                op_push(&operator_stack, element);
                lastNumber = false;
            }
            ptr = next;
        } else {
            printf("Could not recognize symbol \"%s\"\n", ptr);
            exitcode = SHUNT_UNRECOGNIZED;
        }
    }
    while(exitcode == SHUNT_DONE && operator_stack.level > 0) {
        stack_element_t op = op_pop(&operator_stack);
        if(evalOperator(&value_stack, op.token)) {
            printf("Missing values on stack, probably missing operand\n");
            exitcode = SHUNT_MISSING_VALUE;
        }
    }
    if(exitcode == SHUNT_DONE) {
        if(value_stack.level == 1) {
            *result = op_pop(&value_stack).value;
        } else if(value_stack.level == 0) {
            printf("No input provided\n");
            exitcode = SHUNT_NO_VALUES;
        } else {
            printf("Values left on stack, probably missing right parenthesis\n");
            exitcode = SHUNT_MISMATCHED_OPEN;
        }
    }
    if(operator_stack.stack != NULL) free(operator_stack.stack);
    if(value_stack.stack != NULL) free(value_stack.stack);
    return exitcode;
}