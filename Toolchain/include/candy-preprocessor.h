#ifndef CANDY_PREPROCESSOR_H
#define CANDY_PREPROCESSOR_H

#include "candy.h"

typedef enum candy_preprocessor_token_type {
    CANDY_PREPROCESSOR_NONE = 0,
    
    CANDY_PREPROCESSOR_LOCAL_HEADER_NAME,
    CANDY_PREPROCESSOR_PATH_HEADER_NAME,
    CANDY_PREPROCESSOR_IDENTIFIER,
    CANDY_PREPROCESSOR_NUMBER,
    CANDY_PREPROCESSOR_CHARACTER,
    CANDY_PREPROCESSOR_STRING,
    CANDY_PREPROCESSOR_PUNCTUATOR,
    CANDY_PREPROCESSOR_WHITESPACE,
    CANDY_PREPROCESSOR_COMMENT,
    CANDY_PREPROCESSOR_NEWLINE,
    
    CANDY_PREPROCESSOR_PLACEMARKER,
} candy_preprocessor_token_type_t;

typedef struct candy_preprocessor_token {
    candy_preprocessor_token_type_t type;
} candy_preprocessor_token_t;

typedef struct candy_preprocessor_character {
    candy_preprocessor_token_type_t type;
    candy_encoding_modifier_t modifier;
} candy_preprocessor_character_t;

typedef struct candy_preprocessor_string {
    candy_preprocessor_token_type_t type;
    candy_encoding_modifier_t modifier;
} candy_preprocessor_string_t;

typedef struct candy_preprocessor_punctuator {
    candy_preprocessor_token_type_t type;
    candy_operator_t op;
} candy_preprocessor_punctuator_t;


#endif