#include <stdlib.h>
#include <stdio.h>

#include "nandy_emu_tools.h"

cpu_state_t state;

int main(int argc, char** argv) {
    if(argc != 2) {
        printf("Wrong number of args\n");
        return -1;
    }
    FILE* f = fopen(argv[1], "rb");
    if(!f) {
        printf("File %s not found\n", argv[1]);
        return -1;
    }
    state = INIT_STATE;
    fread(state.rom, sizeof(word_t), ADDR_RAM_MASK, f);
    
    while(!emu_step(&state));
}