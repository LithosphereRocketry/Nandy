#include "instructions.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

const instruction_t INSTRUCTION_INITIALIZER = {0, NULL, NULL, NULL};

instruction_t* buildInstruction(char* line) {
    char *label = 0, *inst = 0, *param = 0;
    char* comment = strstr(line, COMMENT_TOK);
    if(comment) { *comment = '\0'; }
    char* labeldiv = strstr(line, LABEL_TOK);
    if(labeldiv) {
        *labeldiv = '\0';
        label = line;
        inst = labeldiv + strlen(LABEL_TOK);
    } else {
        inst = line;
    }
    while(isspace(*inst)) {
        inst++;
    }
    param = inst; // navigate to the start of the next word
    while(*param && !isspace(*param)) { param++; }
    *param = '\0';
    do { param++; } while(*param && isspace(*param));
    char* endtrim = param + strlen(param);
    while(isspace(*(endtrim-1))) {
        endtrim--;
    }
    *endtrim = '\0';

    if(strlen(inst) == 0) {
        return 0;
    }

    instruction_t* result = malloc(sizeof(instruction_t));
    if(!result) { return 0; }
    result->token = encodeToken(inst);
    result->prev = NULL;
    result->next = NULL;
    result->params = malloc(strlen(param) + 1);
    if(!result->params) {
        free(result);
        return 0;
    }
    strcpy(result->params, param);

    return result;
}

void insertInstruction(instruction_t* after, instruction_t* instr) {
    instr->next = after->next;
    instr->prev = after;
    if(after->next) {
        after->next->prev = instr;
    }
    after->next = instr;
}

void deleteInstruction(instruction_t* instr) {
    free(instr->params);
    if(instr->next) { instr->next->prev = instr->prev; }
    if(instr->prev) { instr->prev->next = instr->next; }
    free(instr);
}

void printInstruction(instruction_t* instr) {
    printf("%s %s\n", decodeToken(instr->token), instr->params);
}

void flattenInstruction(instruction_t* instr) {
    if(instr->token == encodeToken("rdi")) {
        if(instr->params) {
            
        }
    } else { // things that are already flat
        return;
    }
}