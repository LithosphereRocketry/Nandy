#include "charparse.h"

const char* parseChar(const char* start, char* dst, FILE* debug) {
    const char* ptr = start;
    char value;
    if(*ptr == '\\') {
        ptr++;
        int count;
        switch(*ptr) {
            case 'a': value = '\a'; ptr++; break;
            case 'b': value = '\b'; ptr++; break;
            case 'f': value = '\f'; ptr++; break;
            case 'n': value = '\n'; ptr++; break;
            case 'r': value = '\r'; ptr++; break;
            case 't': value = '\t'; ptr++; break;
            case 'v': value = '\v'; ptr++; break;
            case '\"':
            case '\\':
            case '\'': value = *ptr; ptr++; break;
            case 'x':
                ptr++;
                if(sscanf(ptr, "%2hhx%n", &value, &count) == 1) {
                    ptr += count;
                } else {
                    if(debug) fprintf(debug, "Unrecognized hexadecimal literal\n");
                    return NULL;
                }
                break;
            default:
                if(sscanf(ptr, "%3hho%n", &value, &count) == 1) {
                    ptr += count;
                } else {
                    if(debug) fprintf(debug, "Unrecognized escape sequence\n");
                    return NULL;
                }
        }
    } else {
        value = *ptr;
        ptr ++;
    }
    *dst = value;
    return ptr;
}