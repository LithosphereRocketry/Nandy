#include "argparse.h"

#include <string.h>
#include <stdlib.h>

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

static void dohelp(argument_t** args, size_t nargs) {
    size_t max_width = 0;
    size_t* widths = malloc(nargs * sizeof(size_t));
    for(size_t i = 0; i < nargs; i++) {
        argument_t* arg = args[i];
        widths[i] = 0;
        if(arg->abbr) widths[i] += 2;
        if(arg->abbr && arg->name) widths[i]++;
        if(arg->name) widths[i] += strlen(arg->name) + 2;
        if(widths[i] > max_width) max_width = widths[i];
    }

    printf("Arguments:\n");
    printf("  -h/--help");
    for(size_t j = 0; j < (max_width - 7); j++) printf(" ");
    printf("Prints this help text.\n");
    for(size_t i = 0; i < nargs; i++) {
        argument_t* arg = args[i];
        printf("  ");
        if(arg->abbr) printf("-%c", arg->abbr);
        if(arg->abbr && arg->name) printf("/");
        if(arg->name) printf("--%s", arg->name);
        for(size_t j = 0; j < (max_width - widths[i]); j++) printf(" ");
        if(arg->help) printf("  %s", arg->help);
        printf("\n");
    }
    free(widths);
    exit(0);
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
                    if(*seekptr == 'h') {
                        dohelp(args, nargs);
                    }
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