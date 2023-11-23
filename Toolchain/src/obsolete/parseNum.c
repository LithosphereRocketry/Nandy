// this is bad and inflexible but hey it works

#include "parseNum.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
const char* pnum_parse_err = 0;
const char* PNUM_ERR_MISMATCHED_OPEN = "Mismatched parentheses, probably an extra (";
const char* PNUM_ERR_MISMATCHED_CLOSE = "Mismatched parentheses, probably an extra )";
const char* PNUM_ERR_UNPARSEABLE = "Non-parseable characters or bad literal found";

int _add(int x, int y) { return x+y; }
int _sub(int x, int y) { return x-y; }
int _mul(int x, int y) { return x*y; }
int _div(int x, int y) { return x/y; }

const char binsyms[] = {'+', '-', '*', '/'};
int (*binfuncs[])(int, int) = {_add, _sub, _mul, _div};
// _STATIC_ASSERT(sizeof(binsyms) == sizeof(binfuncs)/sizeof(int (*)(int, int)));

int toDigit(char digit) {
    if(digit >= '0' && digit <= '9') {
        return digit-'0';
    } else if(digit >= 'A' && digit <= 'F') {
        return digit-'A'+10;
    } else if(digit >= 'a' && digit <= 'f') {
        return digit-'a'+10;
    } else {
        return -1;
    }
}

bool parseLiteral(int* to, const char* str, int len) {
    int base = 10;
    int literal = 0;
    int index = 0;
    if(len > 0 && str[0] == '0') {
        if(len < 2) {
            *to = 0;
            return true;
        }
        switch(str[1]) {
            case '\0':
                return 0;
            case 'x':
            case 'X':
                base = 16;
                index += 2;
                break;
            case 'b':
            case 'B':
                base = 2;
                index += 2;
                break;
            default:
                base = 8;
                index += 1;
                break;
        }
    }
    for(; index < len; index++) {
        int dig = toDigit(str[index]);
        if(dig >= 0 && dig < base) {
            literal *= base;
            literal += dig;
        } else {
            return false;
        }
    }
    *to = literal;
    return true;
}

bool parseInt_helper(int* to, const char* str, const int* depths, int mindepth, size_t len) {
    if(isspace(str[0])) { return parseInt_helper(to, str+1, depths+1, mindepth, len-1); }
    if(isspace(str[len-1])) { return parseInt_helper(to, str, depths, mindepth, len-1); }

    bool drop = true;
    for(int i = 0; i < len-1; i++) {
        if(depths[i] == mindepth) {
            drop = false;
            break;
        }
    }
    if(drop && str[0] == '(') { return parseInt_helper(to, str+1, depths+1, mindepth+1, len-2); }

    if(parseLiteral(to, str, len)) {
        return true;
    }
    
    for(int s = 0; s < sizeof(binsyms); s++) {    
        for(int i = 0; i < len; i++) {
            // this should really be something using strsep() so that operators can be more than 1 character (e.g. <<)
            if(str[i] == binsyms[s] && depths[i] == mindepth) {
                int left, right;
                bool success = parseInt_helper(&left, str, depths, mindepth, i)
                            && parseInt_helper(&right, str+i+1, depths+i+1, mindepth, len-i-1);
                if(success) { *to = binfuncs[s](left, right); }
                return success;
            }
        }
    }
    pnum_parse_err = PNUM_ERR_UNPARSEABLE;
    return false;
}

bool parseInt(int* to, const char* str) {
    int len = strlen(str);
    int* depths = malloc(len * sizeof(int));
    int depth = 0;
    bool result = true;
    for(int i = 0; i < len; i++) {
        switch(str[i]) {
            case '(':
                depth++;
                break;
            case ')':
                depth--;
                break;
        }
        if(depth < 0) {
            result = false;
            pnum_parse_err = PNUM_ERR_MISMATCHED_CLOSE;
            break;
        }
        depths[i] = depth;
    }
    if(depth != 0) {
        pnum_parse_err = PNUM_ERR_MISMATCHED_OPEN;
    }

    result = result && parseInt_helper(to, str, depths, 0, len);
    free(depths);
    return result;
}