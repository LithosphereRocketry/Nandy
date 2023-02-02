// this doesn't work and is bad

#include "parseNum.h"
#include "string.h"
#include "stdio.h"

bool parseInt(char* str, int* resptr) {
    bool helper(char* str, int len, int* resptr) {
        char* addr;
        if(len <= 0) {
            return false;
        } else if(addr = memchr(str, '+', len)) {
            int pos = addr-str;
            int tmp1, tmp2;
            if(!helper(str, pos, &tmp1)
            || !helper(addr+1, len-pos-1, &tmp2)) {
                return false;
            }
            *resptr = tmp1+tmp2;
            return true;
        } else if(addr = memchr(str, '-', len)) {
            int pos = addr-str;
            int tmp1, tmp2;
            if(!helper(str, pos, &tmp1)
            || !helper(addr+1, len-pos-1, &tmp2)) {
                return false;
            }
            *resptr = tmp1-tmp2;
            return true;
        } else { // all else failing, try to read digits
            *resptr = 0;
            for(int i = 0; i < len; i++) { // this is iterative because once we start parsing a number we can't stop
                if(str[i] < '0' || str[i] > '9') {
                    return false;
                }
                *resptr *= 10;
                *resptr += str[i]-'0';
            }
            return true;
        }
    }

    return helper(str, strlen(str), resptr);
}