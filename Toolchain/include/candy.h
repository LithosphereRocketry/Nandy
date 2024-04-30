#ifndef CANDY_H
#define CANDY_H

#include "stdio.h"
#include "stdlib.h"

#define TRUE  1
#define FALSE 0

// All of the following forms are valid for this:
// assert(condition)
// assert(condition, message)
// assert(condition, message, ...)
#ifdef DEBUG
    #define assert(condition, ...) do { \
        if (!(condition)) { \
            _Pragma("GCC diagnostic ignored \"-Wformat-zero-length\""); \
            printf("" __VA_ARGS__); \
            _Pragma("GCC diagnostic warning \"-Wformat-zero-length\""); \
            exit(-1); \
        } \
    } while (0)
#else
    #define assert(condition, ...) ((void)0)
#endif

typedef enum candy_encoding_modifier {
    CANDY_ENCODING_MODIFIER_NONE = 0,
    CANDY_ENCODING_MODIFIER_WIDE,
    CANDY_ENCODING_MODIFIER_UTF8,
    CANDY_ENCODING_MODIFIER_UTF16,
    CANDY_ENCODING_MODIFIER_UTF32,
} candy_encoding_modifier_t;

typedef enum candy_operator {
    CANDY_OPERATOR_NONE = 0,
    
    CANDY_OPERATOR_POINTER_ACCESS,        // ->
    CANDY_OPERATOR_INCREMENT,             // ++
    CANDY_OPERATOR_DECREMENT,             // --
    CANDY_OPERATOR_SHIFT_LEFT,            // <<
    CANDY_OPERATOR_SHIFT_RIGHT,           // >>
    CANDY_OPERATOR_LESS_THAN_OR_EQUAL,    // <=
    CANDY_OPERATOR_GREATER_THAN_OR_EQUAL, // >=
    CANDY_OPERATOR_EQUAL,                 // ==
    CANDY_OPERATOR_NOT_EQUAL,             // !=
    CANDY_OPERATOR_LOGICAL_AND,           // &&
    CANDY_OPERATOR_LOGICAL_OR,            // ||
    CANDY_OPERATOR_PREFIX,                // ::
    CANDY_OPERATOR_VARIADIC,              // ...
    CANDY_OPERATOR_MULTIPLY_ASSIGN,       // *=
    CANDY_OPERATOR_DIVIDE_ASSIGN,         // /=
    CANDY_OPERATOR_REMAINDER_ASSIGN,      // %=
    CANDY_OPERATOR_ADD_ASSIGN,            // +=
    CANDY_OPERATOR_SUBTRACT_ASSIGN,       // -=
    CANDY_OPERATOR_SHIFT_LEFT_ASSIGN,     // <<=
    CANDY_OPERATOR_SHIFT_RIGHT_ASSIGN,    // >>=
    CANDY_OPERATOR_BITWISE_AND_ASSIGN,    // &=
    CANDY_OPERATOR_BITWISE_XOR_ASSIGN,    // ^=
    CANDY_OPERATOR_BITWISE_OR_ASSIGN,     // |=
    CANDY_OPERATOR_CONCATENATE,           // ##
    CANDY_OPERATOR_ARRAY_OPEN_DIGRAPH,    // <:
    CANDY_OPERATOR_ARRAY_CLOSE_DIGRAPH,   // :>
    CANDY_OPERATOR_BLOCK_OPEN_DIGRAPH,    // <%
    CANDY_OPERATOR_BLOCK_CLOSE_DIGRAPH,   // %>
    CANDY_OPERATOR_PREPROCESS_DIGRAPH,    // %:
    CANDY_OPERATOR_CONCATENATE_DIGRAPH,   // %:%:
    
    CANDY_OPERATOR_ARRAY_OPEN   = '[',
    CANDY_OPERATOR_ARRAY_CLOSE  = ']',
    CANDY_OPERATOR_GROUP_OPEN   = '(',
    CANDY_OPERATOR_GROUP_CLOSE  = ')',
    CANDY_OPERATOR_BLOCK_OPEN   = '{',
    CANDY_OPERATOR_BLOCK_CLOSE  = '}',
    CANDY_OPERATOR_STRUCT_ACCESS= '.',
    CANDY_OPERATOR_BITWISE_AND  = '&',
    CANDY_OPERATOR_MULTIPLY     = '*',
    CANDY_OPERATOR_ADD          = '+',
    CANDY_OPERATOR_SUBTRACT     = '-',
    CANDY_OPERATOR_BITWISE_NOT  = '~',
    CANDY_OPERATOR_LOGICAL_NOT  = '!',
    CANDY_OPERATOR_DIVIDE       = '/',
    CANDY_OPERATOR_REMAINDER    = '%',
    CANDY_OPERATOR_LESS_THAN    = '<',
    CANDY_OPERATOR_HEADER_OPEN  = '<',
    CANDY_OPERATOR_GREATER_THAN = '>',
    CANDY_OPERATOR_HEADER_CLOSE = '>',
    CANDY_OPERATOR_BITWISE_XOR  = '^',
    CANDY_OPERATOR_BITWISE_OR   = '|',
    CANDY_OPERATOR_TERNARY_THEN = '?',
    CANDY_OPERATOR_TERNARY_ELSE = ':',
    CANDY_OPERATOR_LABEL        = ':',
    CANDY_OPERATOR_TERMINATE    = ';',
    CANDY_OPERATOR_ASSIGN       = '=',
    CANDY_OPERATOR_SEPARATE     = ',',
    CANDY_OPERATOR_PREPROCESS   = '#',
    CANDY_OPERATOR_STRINGIFY    = '#',
} candy_operator_t;

typedef struct string {
    char *text;
    long count;
} string_t;

#endif