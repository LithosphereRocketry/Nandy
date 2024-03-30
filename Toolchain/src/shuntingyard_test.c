#include "shuntingyard.h"
#include "iotools.h"
#include <stdio.h>

int main(int argc, char** argv) {
    while(1) {
        char* str = fgets_dup(stdin);
        int64_t val;
        shunting_status_t status = parseExp(NULL, str, &val);
        free(str);
        if(status != SHUNT_DONE) {
            printf("Parse failed: %i\n", status);
            return -1;
        } else {
            printf("Value is %li\n", val);
        }
    }
}