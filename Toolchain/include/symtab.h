#ifndef SYMTAB_H
#define SYMTAB_H

#include <stdint.h>
#include <stdlib.h>

typedef struct symbol {
    const char* name;
    int64_t value;
} symbol_t;

typedef struct symtab symtab_t;
typedef struct symtab {
    size_t len;
    size_t cap;
    symbol_t* symbols;
    symtab_t* parent;
} symtab_t;

static const symtab_t SYMTAB_INIT = {0, 0, NULL, NULL};
void symtab_destroy(symtab_t* table);

// Claims ownership of name
void symtab_put(symtab_t* table, const char* name, int64_t value);

int64_t* symtab_get(const symtab_t* table, const char* name);

#endif