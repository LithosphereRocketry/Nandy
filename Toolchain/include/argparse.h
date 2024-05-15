#ifndef ARGPARSE_H
#define ARGPARSE_H

#include <stdio.h>
#include <stdbool.h>

#define ARG_TOKEN '-'

typedef struct argument {
    const char abbr;
    const char* name;
    bool hasval;
    union {
        bool present;
        char* value;
    } result;
} argument_t;

int argparse(argument_t** args, size_t nargs, int argc, char** argv);

#endif