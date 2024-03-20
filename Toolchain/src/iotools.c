#include <stdlib.h>

#include "iotools.h"

size_t fsize(FILE* stream) {
    size_t oldpos = ftell(stream);
    fseek(stream, 0, SEEK_END);
    size_t sz = ftell(stream);
    fseek(stream, oldpos, SEEK_SET);
    return sz;
}

/**
 * Helper function for read-until-character to heap
 * Reads until the given character, or EOF if until=-1
*/
static char* fread_dup_helper(FILE* stream, int until) {
    char* str = malloc(1);
    size_t size = 0;
    size_t cap = 1;
    if(!str) return NULL;

    while(1) {
        str[size] = fgetc(stream);
        if(feof(stream)) break;

        size ++;
        if(size >= cap) {
            cap *= 2;
            char* newstr = realloc(str, cap);
            if(!newstr) {
                free(str);
                return NULL;
            }
            str = newstr;
        }
        if(str[size-1] == until) break;
    }
    str[size] = '\0';
    str = realloc(str, size+1);
    return str;
}

char* fgets_dup(FILE* stream) {
    return fread_dup_helper(stream, '\n');
}

char* fread_dup(FILE* stream) {
    return fread_dup_helper(stream, -1);
}

const char* eol(const char* str) {
    const char* ptr = str;
    while(*ptr != '\0' && *ptr != '\n') {
        ptr++;
    }
    return ptr;
}