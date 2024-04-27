#ifndef CANDY_H
#define CANDY_H

#include "stdio.h"
#include "stdlib.h"

// All of the following forms are valid for this:
// assert(condition)
// assert(condition, message)
// assert(condition, message, ...)
#ifdef DEBUG
    #define assert(condition, ...) do { \
        if (!(condition)) { \
            _Pragma("GCC diagnostic ignored \"-Wformat-zero-length\""); \
            printf("" __VA_ARGS__); \
            _Pragma("GCC diagnostic warning \"-Wformat-zero-length\""); \
            exit(-1); \
        } \
    } while (0)
#else
    #define assert(condition, ...) ((void)0)
#endif

typedef struct string {
    char *text;
    long count;
} string_t;

#endif