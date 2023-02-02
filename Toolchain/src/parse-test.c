#include "parseNum.h"
#include <stdio.h>

int main(int argc, char** argv) {
    if(argc > 1) {
        int result;
        if(parseInt(argv[1], &result)) {
            printf("Result: %i\n", result);
        } else {
            printf("Invalid int literal %s\n", argv[1]);
        }
    } 
}