#ifndef TOKENS_H
#define TOKENS_H

#include <stdint.h>

typedef uint64_t token_t;

token_t encodeToken(const char* str);
char* decodeToken(token_t token);

#endif