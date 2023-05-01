#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "tokens.h"

token_t encodeToken(const char* str) {
    if(strlen(str) > sizeof(token_t)) {
        printf("Warning: truncating token %s to %.4s\n", str, str);
    }
    token_t token = 0;
    bool strexists = true;
    for(int i = 0; i < sizeof(token_t); i++) {
        if(!str[i]) {
            strexists = false;
        }
        token <<= 8;
        if(strexists) {
            token |= str[i];
        }
    }
    return token;
}

char tokenBuf[5];
char* decodeToken(token_t token) {
    tokenBuf[ sizeof(token_t)] = '\0';
    for(int i = sizeof(token_t)-1; i >= 0; i--) {
        tokenBuf[i] = token & 0xFF;
        token >>= 8;
    }
    return tokenBuf;
}