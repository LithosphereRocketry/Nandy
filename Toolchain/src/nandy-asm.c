#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "iotools.h"
#include "nandy_tools.h"

bool isValidLabel(char c) {
    return isalpha(c) || c == '_';
}

char* parseLabel(const char* str, const char** startOfText) {
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

const instruction_t* parseInstr(const ilist_t* instrs, const char* str, const char** startOfText) {
    for(int i = 0; i < instrs->size; i++) {
        const instruction_t* instr = instrs->list[i];
        if(!strncmp(str, instr->mnemonic, strlen(instr->mnemonic))
                && (isspace(str[strlen(instr->mnemonic)])
                    || str[strlen(instr->mnemonic)] == '\0')) {
            *startOfText = str + strlen(instr->mnemonic);
            return instr;
        }
    }
    return NULL;
}

int assemble_helper(const char* str, asm_state_t* dest, bool inst_line) {
    if(!str) {
        printf("No string provided, or previous parse failed to give status\n");
        return -1;
    }
    if(*str == '\0') return 0;
    if(*str == '\n') return assemble_helper(str+1, dest, true);
    if(isspace(*str)) return assemble_helper(str+1, dest, inst_line);
    if(*str == '#') return assemble_helper(eol(str), dest, inst_line);
    if(!inst_line) {
        const char* ptr = eol(str);
        printf("Unexpected text at \"%.*s\"\n", (int) (ptr-str), str);
        return -3;
    }
    const char* nextToken = NULL;

    const char* lbl;
    const instruction_t* instr;
    if((lbl = parseLabel(str, &nextToken))) {
        if(addLabel(dest, lbl, dest->rom_loc)) {
            printf("Failed to add symbol\n");
            return -4;
        }
    } else if((instr = parseInstr(dest->instrs, str, &nextToken))) {
        if(instr->assemble) {
            nextToken = instr->assemble(nextToken, dest);
            inst_line = false;
        } else {
            printf("Instruction %s missing assembly\n", instr->mnemonic);
            return -5;
        }
    } else {
        const char* ptr = eol(str);
        printf("Failed to parse line \"%.*s\"\n", (int) (ptr-str), str);
        return -2;
    }
    return assemble_helper(nextToken, dest, inst_line);
}
int assemble(const char* str, asm_state_t* dest) {
    return assemble_helper(str, dest, true);
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