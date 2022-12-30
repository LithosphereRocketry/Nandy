#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef int8_t word_t;
typedef uint8_t inst_t;

typedef enum {
    IMM_NONE,
    IMM_3,
    IMM_4
} ImmType;

typedef struct {
    char* mnemonic;
    ImmType imm;
    inst_t base;
} Instruction;

unsigned int index = 0;
inst_t binary[256];

void assemble(char* line) {
    
}