#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "tokens.h"

#define COMMENT_TOK "#"
#define LABEL_TOK ":"

typedef struct instruction {
    token_t token;
    char* params;
    struct instruction* next;
    struct instruction* prev;
} instruction_t;

const extern instruction_t INSTRUCTION_INITIALIZER;

instruction_t* buildInstruction(char* line);
void insertInstruction(instruction_t* after, instruction_t* instr);
void deleteInstruction(instruction_t* instr);
void printInstruction(instruction_t* instr);
void flattenInstruction(instruction_t* instr);

#endif