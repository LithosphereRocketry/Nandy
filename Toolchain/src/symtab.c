#include <string.h>

#include "symtab.h"

// Claims ownership of name
void symtab_put(symtab_t* table, const char* name, int64_t value) {
    int64_t* prevval = symtab_get(table, name);
    if(prevval) {
        *prevval = value;
    } else {
        if(table->len >= table->cap) {
            if(table->cap == 0) {
                table->cap = 1;
            } else {
                table->cap *= 2;
            }
            table->symbols = realloc(table->symbols, table->cap*sizeof(symbol_t));
        }
        table->symbols[table->len].name = name;
        table->symbols[table->len].value = value;
        table->len ++;
    }
}

int64_t* symtab_get(const symtab_t* table, const char* name) {
    for(size_t i = 0; i < table->len; i++) {
        if(!strcmp(name, table->symbols[i].name)) {
            return &(table->symbols[i].value);
        }
    }
    return NULL;
}