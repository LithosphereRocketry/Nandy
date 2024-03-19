#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "iotools.h"
#include "nandy_tools.h"

bool isValidLabel(char c) {
    return isalpha(c) || c == '_';
}

char* asmLabel(const char* str, const char** startOfText) {
    const char* ptr = str;
    const char* endOfWord = NULL;
    while(1) {
        if(*ptr == ':') {
            if(!endOfWord) endOfWord = ptr;
            if(endOfWord > str) {
                *startOfText = ptr + 1;
                return strndup(str, endOfWord-str);
            } else {
                return NULL;
            }
        } else if(*ptr == '\n') {
            return NULL;
        } else if(isspace(*ptr)) {
            if(!endOfWord) endOfWord = ptr;
            ptr++;
        } else if(endOfWord) { // not a space, not :, and past an existing space
            return NULL;
        } else if(!isValidLabel(*ptr)) {
            return NULL;
        } else {
            ptr++;
        }
    }
}

instruction_t* asmInstr(const char* str, const char** startOfText) {
    
}

int assemble(const char* str, asm_state_t* dest) {
    if(!str) return -1;
    if(*str == '\0') return 0;
    if(isspace(*str)) return assemble(str+1, dest);

    const char* endOfLabel = NULL;
    const char* nextToken = NULL;
    const char* lbl;
    if(lbl = asmLabel(str, &nextToken)) {
        if(addLabel(dest, lbl, dest->rom_loc)) {
            printf("Failed to add symbol\n");
            return -1;
        }
    }
    return assemble(nextToken, dest);
}

int main(int argc, char** argv) {
    if(argc != 3) {
        printf("Wrong number of args\n");
        return -1;
    }
    FILE* f = fopen(argv[1], "r");
    char* text = fread_dup(f);
    fclose(f);
    if(text == NULL) {
        printf("Out of memory\n");
        return -1;
    }

    asm_state_t state = INIT_ASM;
    int err = assemble(text, &state);
    if(err) {
        printf("Failed to assemble: error %i\n", err);
        return err;
    }

    for(int i = 0; i < state.resolved_sz; i++) {
        printf("%s : %li\n", state.resolved[i].name, state.resolved[i].value);
    }

    FILE* fdest = fopen(argv[2], "w");
    fwrite(state.cpu.rom, sizeof(word_t), ROM_SIZE, fdest);

    free(text);
}