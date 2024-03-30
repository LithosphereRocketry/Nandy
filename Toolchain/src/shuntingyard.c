#include "shuntingyard.h"
#include "shuntingyard_funcs.h"

#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

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
        if(unary == operators[i].isUnary 
                && !strncmp(expstr, operators[i].str, strlen(operators[i].str))) {
            out->token = &operators[i];
            return expstr + strlen(operators[i].str);
        }
    }
    return NULL;
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
                stack_element_t op = op_pop(&operator_stack);
                lastNumber = true;
                do {
                    op.token->func(&value_stack);
                    op = op_pop(&operator_stack);
                } while(op.token != &operators[OPENPAREN]);
            } else {
                while(operator_stack.level > 0) {
                    stack_element_t op = op_pop(&operator_stack);
                    if(op.token->priority < element.token->priority) {
                        op_push(&operator_stack, op);
                        break;
                    } else {
                        if(op.token->func(&value_stack)) {
                            exitcode = SHUNT_TOO_FEW_VALUES;
                            break;
                        }
                    }
                }
                op_push(&operator_stack, element);
                lastNumber = false;
            }
            ptr = next;
        } else {
            printf("Failed at %s\n", ptr);
            exitcode = SHUNT_UNRECOGNIZED;
        }
    }

    while(exitcode == SHUNT_DONE && operator_stack.level > 0) {
        stack_element_t op = op_pop(&operator_stack);
        if(op.token->func(&value_stack)) {
            exitcode = SHUNT_TOO_FEW_VALUES;
        }
    }
    if(value_stack.level == 1) {
        *result = op_pop(&value_stack).value;
    } else {
        exitcode = SHUNT_TOO_MANY_VALUES;
    }
    if(operator_stack.stack != NULL) free(operator_stack.stack);
    if(value_stack.stack != NULL) free(value_stack.stack);
    return exitcode;
}