#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

#define COMMENT_TOK "#"

int main(int argc, char** argv) {
    if(argc != 3) {
        printf("%s: Expected 2 arguments, got %i", argv[0], argc-1);
        return 0;
    }
    char* srcpath = argv[1];
    char* dstpath = argv[2];

    FILE* srcfile = fopen(srcpath, "r");
    FILE* dstfile = fopen(dstpath, "w");

    while(!feof(srcfile)) {
        char buf[512];
        fgets(buf, 511, srcfile);
        char* comment = strstr(buf, COMMENT_TOK);
        if(comment) { *comment = '\0'; }
        if(strlen(buf) > 0) {
            char cmd[512], arg[512];
            bool isarg = sscanf(buf, "%s%s", cmd, arg)-1;
            if(isarg) {
                printf("Call %s with argument %s\n", cmd, arg);
            } else {
                printf("Call %s\n", cmd);
            }
        }
    }
}