#include <stdio.h>
#include <stdlib.h>

#include "iotools.h"
#include "nandy_tools.h"

int main(int argc, char** argv) {
    if(argc != 2) {
        puts("Wrong number of args");
        return 0;
    }
    FILE* f = fopen(argv[1], "r");
    char* text = fread_dup(f);
    fclose(f);
    if(text != NULL) {
        puts(text);
    } else {
        puts("Out of memory");
    }
    free(text);
}

typedef struct unresolved {
    addr_t location;
    char* str;
} unresolved_t;

typedef struct label {
    char* name;
    int64_t value;
} label_t;

typedef struct asmstate {
    cpu_state_t cpu;
    
    size_t resolved_sz;
    size_t resolved_cap;
    label_t* resolved;

    size_t unresolved_sz;
    size_t unresolved_cap;
    unresolved_t unresolved;
} asmstate_t;