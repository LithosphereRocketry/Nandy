#include <stdlib.h>
#include <stdio.h>

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
    
    while(!emu_step(&state, fout));

    printf("Complete, executed %li cycles\n", state.elapsed);

    fclose(f);
    if(arg_out.result.value) { // technically it's probably harmless to close
            // stdout here but it makes me sad
        fclose(fout);
    }
}