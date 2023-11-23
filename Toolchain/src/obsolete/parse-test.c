#include "parseNum.h"
#include <stdio.h>


int main(int argc, char** argv) {
    int x;
    for(int i = 1; i < argc; i++) {
        if(parseInt(&x, argv[i])) {
            printf("%s = %i\n", argv[i], x);
        } else {
            printf("Failed to parse %s: %s\n", argv[i], pnum_parse_err);
        }
    }
}