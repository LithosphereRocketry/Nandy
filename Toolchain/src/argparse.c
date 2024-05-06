#include "argparse.h"

#include <string.h>

static int doarg(argument_t* arg, int i, int argc, char** argv) {
    if(arg->hasval) {
        if(i < argc-1) {
            arg->result.value = argv[i+1];
            return i+1;
        } else {
            printf("Argument \"%s\" expects a value\n", argv[i]);
            return 0;
        }
    } else {
        arg->result.present = true;
        return i;
    }
}

int argparse(argument_t** args, size_t nargs, int argc, char** argv) {
    int realcount = 1;
    for(int i = 1; i < argc; i++) {
        if(argv[i][0] == ARG_TOKEN) {
            if(argv[i][1] == ARG_TOKEN) {
                bool found = false;
                for(size_t j = 0; j < nargs; j++) {
                    if(args[j]->name != NULL && !strcmp(argv[i] + 2, args[j]->name)) {
                        i = doarg(args[j], i, argc, argv);
                        if(i == 0) return 0;
                        found = true;
                        break;                        
                    }
                }
                if(!found) {
                    printf("Unrecognized argument \"%s\"\n", argv[i]);
                    return 0;
                }
            } else {
                char* seekptr = argv[i] + 1;
                while(*seekptr) {
                    bool found = false;
                    int newi = 0;
                    for(size_t j = 0; j < nargs; j++) {
                        if(args[j]->abbr != '\0' && *seekptr == args[j]->abbr) {
                            newi = doarg(args[j], i, argc, argv);
                            found = true;
                            break;
                        }

                    }
                    if(!found) {
                        printf("Unrecognized argument \"%c%c\"\n", ARG_TOKEN, *seekptr);
                        return 0;
                    }
                    if(newi != i) {
                        i = newi;
                        break;
                    }
                    seekptr++;
                }
            }
        } else {
            argv[realcount] = argv[i];
            realcount++;
        }
    }
    return realcount;
}