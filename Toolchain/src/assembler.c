#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "instructions.h"
#include "programs.h"

typedef struct label {
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
    // FILE* dstfile = fopen(dstpath, "w");
    
    program_t program = PROGRAM_INITIALIZER;
    while(!feof(srcfile)) {
        char buf[512];
        memset(buf, 0, 512);
        fgets(buf, 511, srcfile);
        
        instruction_t* instr = buildInstruction(buf);
        if(instr) {
            append(&program, instr);
        }
    }
    print(&program);
    delete(&program);
    
        
    //     instruction_t* instr = buildInstruction(buf);
    //     printInstruction(instr);
    //     printf("hi");
    //     if(instr) {
    //         append(&program, instr);
    //     }
    // }
    // print(&program);
    // delete(&program);
}