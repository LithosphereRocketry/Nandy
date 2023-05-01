#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "instructions.h"

typedef struct {
    char* name;
    instruction_t* location;
} label_t;

int main(int argc, char** argv) {
    if(argc != 3) {
        printf("%s: Expected 2 arguments, got %i", argv[0], argc-1);
        return 0;
    }
    char* srcpath = argv[1];
    char* dstpath = argv[2];

    FILE* srcfile = fopen(srcpath, "r");
    FILE* dstfile = fopen(dstpath, "w");

    while(!feof(srcfile)) {
        char buf[512];
        memset(buf, 0, 512);
        fgets(buf, 511, srcfile);
        
        instruction_t* instr = buildInstruction(buf);
        if(instr) {
            printInstruction(instr);
            deleteInstruction(instr);
        }
    }
}