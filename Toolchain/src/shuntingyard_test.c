#include "shuntingyard.h"
#include "iotools.h"
#include <stdio.h>

symbol_t syms[] = {
    { .name = "one", .value = 1 },
    { .name = "two", .value = 2 },
    { .name = "three", .value = 3 }
};

symtab_t table = {
    .cap = sizeof(syms)/sizeof(symbol_t),
    .len = sizeof(syms)/sizeof(symbol_t),
    .symbols = syms    
};

int main(int argc, char** argv) {
    while(1) {
        char* str = fgets_dup(stdin);
        int64_t val;
        shunting_status_t status = parseExp(&table, str, &val, stdout);
        free(str);
        if(status != SHUNT_DONE) {
            printf("Parse failed: %i\n", status);
            return -1;
        } else {
            printf("Value is %li\n", val);
        }
    }
}