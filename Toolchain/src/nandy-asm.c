#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nandy_asm_tools.h"
#include "iotools.h"

int main(int argc, char** argv) {
    if(argc != 3) {
        printf("Wrong number of args\n");
        return -1;
    }
    FILE* f = fopen(argv[1], "r");
    if(!f) {
        printf("File %s not found\n", argv[1]);
        return -1;
    }
    char* text = fread_dup(f);
    fclose(f);
    if(text == NULL) {
        printf("Out of memory\n");
        return -1;
    }

    asm_state_t state = INIT_ASM;
    addLabel(&state, strdup("ISR"), ISR_ADDR);
    int err = assemble(text, &state);
    if(err) {
        printf("Failed to assemble: error %i\n", err);
        return err;
    }

    for(size_t i = 0; i < state.resolved.len; i++) {
        printf("%s : %li\n", state.resolved.symbols[i].name,
                             state.resolved.symbols[i].value);
    }

    FILE* fdest = fopen(argv[2], "w");
    fwrite(state.rom, sizeof(word_t), ROM_SIZE, fdest);
    fclose(fdest);
    asm_state_destroy(&state);

    free(text);
}