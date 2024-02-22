#include <stdio.h>
#include <stdlib.h>

#include "iotools.h"

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