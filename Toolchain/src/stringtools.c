#include "stringtools.h"

inline int linebreak_length(char *c) {
    if (!c) return 0;
    if (c[0] == '\n') return 1;
    if (c[0] == '\r' && c[1] == '\n') return 2;
    return 0;
}

inline int is_whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f';
}

inline int is_letter(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

inline int is_number(char c) {
    return c >= '0' && c <= '9';
}

inline int is_alphanumeric(char c) {
    return is_letter(c) || is_number(c);
}