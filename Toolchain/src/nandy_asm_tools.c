#include "nandy_asm_tools.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "nandy_parse_tools.h"
#include "nandy_check_tools.h"
#include "iotools.h"


bool isValidLabel(char c, bool isfirst) {
    return isalpha(c) || c == '_' || (!isfirst && isalnum(c));
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
        } else if(!isValidLabel(*ptr, ptr == str)) {
            return NULL;
        } else {
            ptr++;
        }
    }
}

const instruction_t* parseInstr(const ilist_t* instrs, const char* str, const char** startOfText) {
    for(size_t i = 0; i < instrs->size; i++) {
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
    if(!strncmp(str, COMMENT_TOK, strlen(COMMENT_TOK))) {
        return assemble_helper(eol(str), dest, inst_line);
    }
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
            nextToken = instr->assemble(instr, nextToken, dest);
            if(!nextToken) {
                printf("Failed parse of instruction %s\n", instr->mnemonic);
                return -5;
            }
            inst_line = false;
        } else {
            printf("Instruction %s missing assembly\n", instr->mnemonic);
            return -6;
        }
    } else {
        const char* ptr = eol(str);
        printf("Failed to parse line \"%.*s\"\n", (int) (ptr-str), str);
        return -2;
    }
    return assemble_helper(nextToken, dest, inst_line);
}
int assemble(const char* str, asm_state_t* dest) {
    addNextCtrlBlock(&dest->ctrl_graph, 0, 0);
    
    addLabel(dest, strdup("ISR"), ISR_ADDR);
    int asmstatus = assemble_helper(str, dest, true);
    addLabel(dest, strdup("FREE_MEM"), dest->ram_loc);
    if(asmstatus != 0) { return asmstatus; }; // TODO: memory leak
    for(size_t i = 0; i < dest->unresolved_sz; i++) {
        if(!dest->unresolved[i].func(dest, 
            dest->unresolved[i].str,
            dest->unresolved[i].location,
            stdout
        )) {
            return -7;
        }
    }
    
    updateNextCtrlLink(&dest->ctrl_graph, dest->rom_loc, 0);
    addFloatingCtrlBlock(&dest->ctrl_graph, ISR_ADDR);
    
    int checkstatus = staticCheck(dest);
    if (checkstatus != 0) { return checkstatus; }
    
    return 0;
}