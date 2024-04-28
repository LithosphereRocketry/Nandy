#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "nandy_emu_tools.h"
#include "argparse.h"

argument_t arg_debug = { .abbr = 'g', .name = "debug", .hasval = false };
argument_t arg_out = { .abbr = 'o', .name = "out", .hasval = true };

argument_t* args[] = {
    &arg_debug,
    &arg_out
};
const size_t n_args = sizeof(args) / sizeof(argument_t*);

cpu_state_t state;

void printDebug(cpu_state_t* state) {
    /** Layout should look like this:
PC  0x123F  CARRY 1         FF      ...
ACC 0x3F    SP  0x3F        FF      rd io
DX  0x3F    DY  0x3F        FF      sw dy
IRX 0x3F    IRY 0x3F        FF      add dx
IN  0x3F    OUT 0x3F        FF  PC> addi 1
IE  1       INT 1           FF      jri
                            FF      nop
Cycles 1234567890       SP> FF      nop
> 
    */
    char linebuf[40][8];
    for(int i = 0; i < 8; i++) {
        strncpy(linebuf[i], "hihihi", 40);
    }

    printf("PC  0x%04hx  CARRY %c         %02hhx      %-40s\n",
            state->pc, state->carry ? '1' : '0', peek(state, 0xFF00 + state->sp + 7), linebuf[0]);
    printf("ACC 0x%02hhx    SP  0x%02hhx        %02hhx      %-40s\n",
            state->acc, state->sp, peek(state, 0xFF00 + state->sp + 6), linebuf[1]);
    printf("DX  0x%02hhx    DY  0x%02hhx        %02hhx      %-40s\n",
            state->dx, state->dy, peek(state, 0xFF00 + state->sp + 5), linebuf[2]);
    printf("IRX 0x%02hhx    IRY 0x%02hhx        %02hhx      %-40s\n",
            state->irx, state->iry, peek(state, 0xFF00 + state->sp + 4), linebuf[3]);
    printf("IN  0x%02hhx    OUT 0x%02hhx        %02hhx  PC> %-40s\n",
            state->irx, state->iry, peek(state, 0xFF00 + state->sp + 3), linebuf[4]);
    printf("IE  %c       INT %c           %02hhx      %-40s\n",
            state->int_en ? '1' : '0', state->int_active ? '1' : '0', peek(state, 0xFF00 + state->sp + 2), linebuf[5]);
    printf("                            %02hhx      %-40s\n",
            peek(state, 0xFF00 + state->sp + 1), linebuf[6]);
    char cyclesbuf[17];
    snprintf(cyclesbuf, 17, "%lu", state->elapsed);
    printf("Cycles %-16s SP> %02hhx      %-40s\n",
            cyclesbuf, peek(state, 0xFF00 + state->sp), linebuf[7]);

}

int main(int argc, char** argv) {
    argc = argparse(args, n_args, argc, argv);
    if(argc <= 0) return -1;
    if(argc != 2) {
        printf("Wrong number of args\n");
        return -1;
    }
    
    FILE* f = fopen(argv[1], "rb");
    if(!f) {
        printf("File %s not found\n", argv[1]);
        return -1;
    }

    FILE* fout = stdout;
    if(arg_out.result.value) {
        fout = fopen(arg_out.result.value, "wb");
        if(!fout) {
            printf("File %s not found\n", arg_out.result.value);
            return -1;
        }
    }

    state = INIT_STATE;
    fread(state.rom, sizeof(word_t), ADDR_RAM_MASK, f);
    
    while(1) {
        while(!emu_step(&state, fout));
        if(arg_debug.result.present) {
            printDebug(&state);
            break;
        } else {
            break;
        }
    }

    printf("Complete, executed %li cycles\n", state.elapsed);

    fclose(f);
    if(arg_out.result.value) { // technically it's probably harmless to close
            // stdout here but it makes me sad
        fclose(fout);
    }
}