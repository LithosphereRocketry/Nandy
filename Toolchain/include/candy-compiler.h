#ifndef CANDY_COMPILER_H
#define CANDY_COMPILER_H

#include "candy.h"

typedef enum candy_ast_type {
    CANDY_AST_NONE = 0,
    CANDY_AST_SCOPE,
    CANDY_AST_STATEMENT,
    CANDY_AST_EXPRESSION,
    CANDY_AST_IDENTIFIER,
} candy_ast_type_t;

typedef struct candy_ast {
    candy_ast_type_t type;
} candy_ast_t;

typedef struct candy_ast_scope {
    candy_ast_type_t type;
    candy_ast_t **children;
    int child_count;
} candy_ast_scope_t;

typedef struct candy_ast_statement {
    candy_ast_type_t type;
} candy_ast_statement_t;

typedef struct candy_ast_expression {
    candy_ast_type_t type;
} candy_ast_expression_t;

typedef struct candy_ast_identifier {
    candy_ast_type_t type;
    string_t identifier;
} candy_ast_identifier_t;

#endif