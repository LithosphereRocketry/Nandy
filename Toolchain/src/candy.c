#define DEBUG

#include "iotools.h"
#include "candy.h"

int main(int argc, char **argv) {
    assert(argc == 2, "Candy needs exactly one argument, the filename to compile, but was given %d arguments\n", argc - 1);
    
    FILE* f = fopen(argv[1], "r");
    assert(f, "File %s was not found\n", argv[1]);
    char* text = fread_dup(f);
    fclose(f);
    assert(text, "Out of memory\n");
    
    free(text);
    return 0;
}

