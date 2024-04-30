#include <string.h>

#define DEBUG

#include "candy-preprocessor.h"
#include "stringtools.h"

#define BUFFER_SIZE 1024
#define PADDING 32
char buffer[BUFFER_SIZE + 1];

FILE *file_in;
FILE *file_out;

char *help =
    "Usage:\n"
    "candy-preprocessor target\n"
    "\n"
    "target\t\tThe file to preprocess\n";

void print_help_and_exit(void) {
    printf("%s", help);
    exit(-1);
}

int read_into_buffer(int start) {
    int size = fread(buffer + start, 1, BUFFER_SIZE - start, file_in);
    buffer[start + size] = 0;
    return size;
}

int read_more(int *index, int should_write) {
    // Write what we have to the output
    if (should_write)
        fwrite(buffer, 1, *index, file_out);
    
    // Copy over what we want to keep
    int bytes_to_keep = BUFFER_SIZE - *index;
    if (*index) {
        memmove(buffer, buffer + *index, bytes_to_keep);
        *index = 0;
    }
    
    // Read in the rest
    int added = read_into_buffer(bytes_to_keep);
    return bytes_to_keep + added;
}

void tokenize_escape(int *i) {
    *i += 1;
    switch (buffer[*i]) {
        case '\\': fwrite("\\", sizeof(char), 1, file_out); *i += 1; break;
        case '\'': fwrite("\'", sizeof(char), 1, file_out); *i += 1; break;
        case '"':  fwrite("\"", sizeof(char), 1, file_out); *i += 1; break;
        case '?':  fwrite("\?", sizeof(char), 1, file_out); *i += 1; break;
        case 'a':  fwrite("\a", sizeof(char), 1, file_out); *i += 1; break;
        case 'b':  fwrite("\b", sizeof(char), 1, file_out); *i += 1; break;
        case 'f':  fwrite("\f", sizeof(char), 1, file_out); *i += 1; break;
        case 'n':  fwrite("\n", sizeof(char), 1, file_out); *i += 1; break;
        case 'r':  fwrite("\r", sizeof(char), 1, file_out); *i += 1; break;
        case 't':  fwrite("\t", sizeof(char), 1, file_out); *i += 1; break;
        case 'v':  fwrite("\v", sizeof(char), 1, file_out); *i += 1; break;
        
        //TODO:
        // Octal
        // case '0': break;
        // case '1': break;
        // case '2': break;
        // case '3': break;
        // case '4': break;
        // case '5': break;
        // case '6': break;
        // case '7': break;
        
        // Hexaadecimal
        // case 'x': break;
        
        default: assert(FALSE, "Unknown escape sequence '\\%c'\n", buffer[*i]);
    }
}

enum candy_preprocessor_state {
    IN_WHITESPACE = 1,
    IN_LINE_COMMENT,
    IN_BLOCK_COMMENT,
    IN_IDENTIFIER,
    IN_NUMBER,
    IN_CHARACTER,
    IN_STRING,
    IN_LOCAL_HEADER_NAME,
    IN_PATH_HEADER_NAME,
};

void tokenizer_pass(void) {
    int i = 0;
    read_into_buffer(0);
    
    //TODO: Unicode
    
    enum candy_preprocessor_state state = 0;
    candy_encoding_modifier_t encoding_modifier = CANDY_ENCODING_MODIFIER_NONE;
    candy_preprocessor_token_type_t header_name = 0;
    int next_is_header_name = -1;
    int header_test_index = -1;
    int was_newline = FALSE;
    char *c = buffer;
    while (c[i]) {
        if (i >= BUFFER_SIZE - PADDING) read_more(&i, FALSE);
        int linebreak = linebreak_length(c+i);
        was_newline = FALSE;
        
        // Remove escaped newlines
        if (c[i] == '\\') {
            assert(c[i+1], "Translation units cannot end with a backslash\n");
            i += 1;
            linebreak = linebreak_length(c+i);
            i += linebreak;
            continue;
        }
        
        switch (state) {
            case IN_WHITESPACE: {
                if (linebreak || !is_whitespace(c[i])) {
                    state = 0;
                } else {
                    i += 1;
                }
            } break;
            
            case IN_LINE_COMMENT:
            case IN_BLOCK_COMMENT: {
                if (state == IN_LINE_COMMENT && linebreak) {
                    state = 0;
                    fwrite(&(char){0}, sizeof(char), 1, file_out);
                } else if (state == IN_BLOCK_COMMENT && c[i] == '*' && c[i+1] == '/') {
                    state = 0;
                    fwrite(&(char){0}, sizeof(char), 1, file_out);
                    i += 2;
                } else {
                    fwrite(c+i, sizeof(char), 1, file_out);
                    i += 1;
                }
            } break;
            
            case IN_IDENTIFIER: {
                #define TEST_HEADER_PROMPT(index, name) \
                    if (header_test_index > (int) sizeof(name) || c[i] != name[header_test_index]) \
                        next_is_header_name &= ~(1 << index);
                #define VALIDATE_HEADER_PROMPT(index, name) \
                    if (header_test_index != (int) sizeof(name) - 1) \
                        next_is_header_name &= ~(1 << index);
                
                if (is_alphanumeric(c[i])) {
                    TEST_HEADER_PROMPT(0, "include");
                    TEST_HEADER_PROMPT(1, "embed");
                    TEST_HEADER_PROMPT(2, "__has_include");
                    TEST_HEADER_PROMPT(3, "__has_embed");
                    fwrite(c+i, sizeof(char), 1, file_out);
                    header_test_index++;
                    i += 1;
                } else {
                    VALIDATE_HEADER_PROMPT(0, "include");
                    VALIDATE_HEADER_PROMPT(1, "embed");
                    VALIDATE_HEADER_PROMPT(2, "__has_include");
                    VALIDATE_HEADER_PROMPT(3, "__has_embed");
                    next_is_header_name &= (1 << 4) - 1;
                    fwrite(&(char){0}, sizeof(char), 1, file_out);
                    state = 0;
                }
                
                #undef TEST_HEADER_PROMPT
                #undef VALIDATE_HEADER_PROMPT
            } break;
            
            case IN_NUMBER: {
                if (((c[i] == 'e' || c[i] == 'E' || c[i] == 'p' || c[i] == 'P') && (c[i+1] == '+' || c[i+1] == '-'))
                    || (c[i] == '\'' && is_alphanumeric(c[i+1]))) {
                    fwrite(c+i, sizeof(char), 2, file_out);
                    i += 2;
                } else if (is_alphanumeric(c[i]) || c[i] == '_' || c[i] == '.') {
                    fwrite(c+i, sizeof(char), 1, file_out);
                    i += 1;
                } else {
                    fwrite(&(char){0}, sizeof(char), 1, file_out);
                    state = 0;
                }
            } break;
            
            case IN_CHARACTER: {
                assert(!linebreak_length(c), "Cannot have a line break inside a character constant\n");
                if (c[i] == '\'') {
                    fwrite(&(char){0}, sizeof(char), 1, file_out);
                    state = 0;
                    encoding_modifier = CANDY_ENCODING_MODIFIER_NONE;
                    i += 1;
                } else if (c[i] == '\\') {
                    tokenize_escape(&i);
                } else {
                    fwrite(c+i, sizeof(char), 1, file_out);
                    i += 1;
                }
            } break;
            
            case IN_STRING: {
                assert(!linebreak_length(c), "Cannot have a line break inside a string\n");
                if (c[i] == '"') {
                    fwrite(&(char){0}, sizeof(char), 1, file_out);
                    state = 0;
                    encoding_modifier = CANDY_ENCODING_MODIFIER_NONE;
                    i += 1;
                } else if (c[i] == '\\') {
                    tokenize_escape(&i);
                } else {
                    fwrite(c+i, sizeof(char), 1, file_out);
                    i += 1;
                }
            } break;
            
            case IN_LOCAL_HEADER_NAME:
            case IN_PATH_HEADER_NAME: {
                if ((state == IN_LOCAL_HEADER_NAME && c[i] == '"')
                    || (state == IN_PATH_HEADER_NAME && c[i] == '>')) {
                    fwrite(&(char){0}, sizeof(char), 1, file_out);
                    next_is_header_name = FALSE;
                    state = 0;
                } else {
                    fwrite(c+i, sizeof(char), 1, file_out);
                }
                i += 1;
            } break;
            
            default: {
                switch (c[i]) {
                    // Handle newlines
                    case '\r':
                    case '\n': {
                        if (!linebreak) goto tokenize_whitespace;
                        was_newline = TRUE;
                        printf("Creating token type CANDY_PREPROCESSOR_NEWLINE\n");
                        candy_preprocessor_token_t t = {CANDY_PREPROCESSOR_NEWLINE};
                        fwrite(&t, sizeof(t), 1, file_out);
                        i += linebreak;
                    } break;
                    
                    // Start IN_WHITESPACE
                    case '\t':
                    case '\v':
                    case '\f':
                    case ' ': {
                        tokenize_whitespace:
                        printf("Creating token type CANDY_PREPROCESSOR_WHITESPACE\n");
                        candy_preprocessor_token_t t = {CANDY_PREPROCESSOR_WHITESPACE};
                        fwrite(&t, sizeof(t), 1, file_out);
                        state = IN_WHITESPACE;
                        i++;
                    } break;
                    
                    // Start IN_LINE_COMMENT / IN_BLOCK_COMMENT
                    case '/': {
                        if (c[i+1] == '/') {
                            state = IN_LINE_COMMENT;
                        } else if (c[i+1] == '*') {
                            state = IN_BLOCK_COMMENT;
                        } else goto tokenize_punctuator;
                        printf("Creating token type CANDY_PREPROCESSOR_COMMENT\n");
                        candy_preprocessor_token_t t = {CANDY_PREPROCESSOR_COMMENT};
                        fwrite(&t, sizeof(t), 1, file_out);
                        i += 2;
                    } break;
                    
                    // Start IN_NUMBER
                    case '.':
                        if (!is_number(c[i+1])) goto tokenize_punctuator;
                        // Fall through
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9': {
                        printf("Creating token type CANDY_PREPROCESSOR_NUMBER\n");
                        candy_preprocessor_token_t t = {CANDY_PREPROCESSOR_NUMBER};
                        fwrite(&t, sizeof(t), 1, file_out);
                        state = IN_NUMBER;
                    } break;
                    
                    // Start IN_CHARACTER
                    case '\'': {
                        printf("Creating token type CANDY_PREPROCESSOR_CHARACTER\n");
                        candy_preprocessor_character_t t = {CANDY_PREPROCESSOR_CHARACTER, encoding_modifier};
                        fwrite(&t, sizeof(t), 1, file_out);
                        state = IN_CHARACTER;
                        i += 1;
                    } break;
                    
                    // Start IN_STRING
                    case '"': {
                        if (next_is_header_name) goto tokenize_header_name;
                        printf("Creating token type CANDY_PREPROCESSOR_STRING\n");
                        candy_preprocessor_string_t t = {CANDY_PREPROCESSOR_STRING, encoding_modifier};
                        fwrite(&t, sizeof(t), 1, file_out);
                        state = IN_STRING;
                        i += 1;
                    } break;
                    
                    // Start IN_LOCAL_HEADER_NAME / IN_PATH_HEADER_NAME
                    case '<': {
                        if (!next_is_header_name) goto tokenize_punctuator;
                        tokenize_header_name:
                        if (c[i] == '"') {
                            state = IN_LOCAL_HEADER_NAME;
                            header_name = CANDY_PREPROCESSOR_LOCAL_HEADER_NAME;
                            printf("Creating token type CANDY_PREPROCESSOR_LOCAL_HEADER_NAME\n");
                        } else {
                            state = IN_PATH_HEADER_NAME;
                            header_name = CANDY_PREPROCESSOR_PATH_HEADER_NAME;
                            printf("Creating token type CANDY_PREPROCESSOR_PATH_HEADER_NAME\n");
                        }
                        candy_preprocessor_token_t t = {header_name};
                        fwrite(&t, sizeof(t), 1, file_out);
                        i += 1;
                    } break;
                    
                    // Handle punctuators
                    case '>':
                    case '[':
                    case ']':
                    case '(':
                    case ')':
                    case '{':
                    case '}':
                    case '&':
                    case '*':
                    case '+':
                    case '-':
                    case '~':
                    case '!':
                    case '%':
                    case '^':
                    case '|':
                    case '?':
                    case ':':
                    case ';':
                    case '=':
                    case ',':
                    case '#': {
                        tokenize_punctuator:
                        #define WRITE_OP(name, size) \
                            { \
                                printf("Creating token type CANDY_PREPROCESSOR_PUNCTUATOR\n"); \
                                candy_preprocessor_punctuator_t t = {CANDY_PREPROCESSOR_PUNCTUATOR, name}; \
                                fwrite(&t, sizeof(t), 1, file_out); \
                                i += size; \
                                break; \
                            }
                        #define MAYBE_OP4(c0, c1, c2, c3, name) \
                            if (c[i] == c0 && c[i+1] == c1 && c[i+2] == c2 && c[i+3] == c3) \
                                WRITE_OP(name, 4)
                        #define MAYBE_OP3(c0, c1, c2, name) \
                            if (c[i] == c0 && c[i+1] == c1 && c[i+2] == c2) \
                                WRITE_OP(name, 3)
                        #define MAYBE_OP2(c0, c1, name) \
                            if (c[i] == c0 && c[i+1] == c1) \
                                WRITE_OP(name, 2)
                        
                        MAYBE_OP4('%',':','%',':', CANDY_OPERATOR_CONCATENATE_DIGRAPH)
                        MAYBE_OP3('.','.','.', CANDY_OPERATOR_VARIADIC)
                        MAYBE_OP3('<','<','=', CANDY_OPERATOR_SHIFT_LEFT_ASSIGN)
                        MAYBE_OP3('>','>','=', CANDY_OPERATOR_SHIFT_RIGHT_ASSIGN)
                        MAYBE_OP2('-','>', CANDY_OPERATOR_POINTER_ACCESS)
                        MAYBE_OP2('+','+', CANDY_OPERATOR_INCREMENT)
                        MAYBE_OP2('-','-', CANDY_OPERATOR_DECREMENT)
                        MAYBE_OP2('<','<', CANDY_OPERATOR_SHIFT_LEFT)
                        MAYBE_OP2('>','>', CANDY_OPERATOR_SHIFT_RIGHT)
                        MAYBE_OP2('<','=', CANDY_OPERATOR_LESS_THAN_OR_EQUAL)
                        MAYBE_OP2('>','=', CANDY_OPERATOR_GREATER_THAN_OR_EQUAL)
                        MAYBE_OP2('=','=', CANDY_OPERATOR_EQUAL)
                        MAYBE_OP2('!','=', CANDY_OPERATOR_NOT_EQUAL)
                        MAYBE_OP2('&','&', CANDY_OPERATOR_LOGICAL_AND)
                        MAYBE_OP2('|','|', CANDY_OPERATOR_LOGICAL_OR)
                        MAYBE_OP2(':',':', CANDY_OPERATOR_PREFIX)
                        MAYBE_OP2('*','=', CANDY_OPERATOR_MULTIPLY_ASSIGN)
                        MAYBE_OP2('/','=', CANDY_OPERATOR_DIVIDE_ASSIGN)
                        MAYBE_OP2('%','=', CANDY_OPERATOR_REMAINDER_ASSIGN)
                        MAYBE_OP2('+','=', CANDY_OPERATOR_ADD_ASSIGN)
                        MAYBE_OP2('-','=', CANDY_OPERATOR_SUBTRACT_ASSIGN)
                        MAYBE_OP2('&','=', CANDY_OPERATOR_BITWISE_AND_ASSIGN)
                        MAYBE_OP2('^','=', CANDY_OPERATOR_BITWISE_XOR_ASSIGN)
                        MAYBE_OP2('|','=', CANDY_OPERATOR_BITWISE_OR_ASSIGN)
                        MAYBE_OP2('#','#', CANDY_OPERATOR_CONCATENATE)
                        MAYBE_OP2('<',':', CANDY_OPERATOR_ARRAY_OPEN_DIGRAPH)
                        MAYBE_OP2(':','>', CANDY_OPERATOR_ARRAY_CLOSE_DIGRAPH)
                        MAYBE_OP2('<','%', CANDY_OPERATOR_BLOCK_OPEN_DIGRAPH)
                        MAYBE_OP2('%','>', CANDY_OPERATOR_BLOCK_CLOSE_DIGRAPH)
                        MAYBE_OP2('%',':', CANDY_OPERATOR_PREPROCESS_DIGRAPH)
                        WRITE_OP(c[i], 1);
                        
                        #undef MAYBE_OP2
                        #undef MAYBE_OP3
                        #undef MAYBE_OP4
                        #undef WRITE_OP
                    } break;
                    
                    default: {
                        candy_encoding_modifier_t old_modifier = encoding_modifier;
                        if (c[i] == 'L' && (c[i+1] == '"' || c[i+1] == '\''))
                            i += 1, encoding_modifier = CANDY_ENCODING_MODIFIER_WIDE;
                        else if (c[i] == 'u' && c[i+1] == '8' && (c[i+2] == '"' || c[i+2] == '\''))
                            i += 2, encoding_modifier = CANDY_ENCODING_MODIFIER_UTF8;
                        else if (c[i] == 'u' && (c[i+1] == '"' || c[i+1] == '\''))
                            i += 1, encoding_modifier = CANDY_ENCODING_MODIFIER_UTF16;
                        else if (c[i] == 'U' && (c[i+1] == '"' || c[i+1] == '\''))
                            i += 1, encoding_modifier = CANDY_ENCODING_MODIFIER_UTF32;
                        else if (is_letter(c[i]) || c[i] == '_') {
                            printf("Creating token type CANDY_PREPROCESSOR_IDENTIFIER\n");
                            candy_preprocessor_token_t t = {CANDY_PREPROCESSOR_IDENTIFIER};
                            fwrite(&t, sizeof(t), 1, file_out);
                            next_is_header_name = -1;
                            header_test_index = 0;
                            state = IN_IDENTIFIER;
                        } else i++;
                        assert(old_modifier == CANDY_ENCODING_MODIFIER_NONE || old_modifier == encoding_modifier, "Concatenated string literals cannot have different modifiers\n");
                    }
                }
            }
        }
    }
    
    assert(was_newline, "Translation units must end with a newline\n");
    assert(!next_is_header_name, "Expected a header name, but encountered the end of the file\n");
    assert(state != IN_BLOCK_COMMENT, "Incomplete comment at the end of the translation unit\n");
    assert(state != IN_CHARACTER, "Incomplete character constant at the end of the translation unit\n");
    assert(state != IN_STRING, "Incomplete string at the end of the translation unit\n");
    assert(state != IN_PATH_HEADER_NAME && state != IN_LOCAL_HEADER_NAME, "Incomplete header name at the end of the translation unit\n");
}

void preprocessor_pass(void) {
    int i = 0;
    read_into_buffer(0);
    
    #define WRITE_STRING() \
        do { \
            while (c[i]) { \
                if (i >= BUFFER_SIZE - PADDING) read_more(&i, FALSE); \
                fwrite(c+i, sizeof(char), 1, file_out); \
                i += 1; \
            } \
            i += 1; \
        } while (0)
    
    char *c = buffer;
    while (c[i]) {
        if (i >= BUFFER_SIZE - PADDING) read_more(&i, FALSE);
        
        candy_preprocessor_token_t *token = (void*) (c + i);
        switch (token->type) {
            case CANDY_PREPROCESSOR_NONE: break;
            
            case CANDY_PREPROCESSOR_PATH_HEADER_NAME:
                printf("Reading token type CANDY_PREPROCESSOR_PATH_HEADER_NAME\n");
                i += sizeof(*token);
                fwrite("<", 1, 1, file_out);
                WRITE_STRING();
                fwrite(">", 1, 1, file_out);
                break;
            
            case CANDY_PREPROCESSOR_LOCAL_HEADER_NAME:
                printf("Reading token type CANDY_PREPROCESSOR_LOCAL_HEADER_NAME\n");
                i += sizeof(*token);
                fwrite("\"", 1, 1, file_out);
                WRITE_STRING();
                fwrite("\"", 1, 1, file_out);
                break;
            
            case CANDY_PREPROCESSOR_IDENTIFIER:
                printf("Reading token type CANDY_PREPROCESSOR_IDENTIFIER\n");
                i += sizeof(*token);
                WRITE_STRING();
                break;
            
            case CANDY_PREPROCESSOR_NUMBER:
                printf("Reading token type CANDY_PREPROCESSOR_NUMBER\n");
                i += sizeof(*token);
                WRITE_STRING();
                break;
            
            case CANDY_PREPROCESSOR_CHARACTER:
                printf("Reading token type CANDY_PREPROCESSOR_CHARACTER\n");
                candy_preprocessor_character_t *t = (void*) token;
                i += sizeof(*t);
                switch (t->modifier) {
                    case CANDY_ENCODING_MODIFIER_NONE: break;
                    case CANDY_ENCODING_MODIFIER_WIDE: fwrite("L", 1, 1, file_out); break;
                    case CANDY_ENCODING_MODIFIER_UTF8: fwrite("u8", 1, 2, file_out); break;
                    case CANDY_ENCODING_MODIFIER_UTF16: fwrite("u", 1, 1, file_out); break;
                    case CANDY_ENCODING_MODIFIER_UTF32: fwrite("U", 1, 1, file_out); break;
                    default: assert(FALSE, "Unknown encoding modifier type %d\n", t->modifier);
                }
                fwrite("'", 1, 1, file_out);
                WRITE_STRING();
                fwrite("'", 1, 1, file_out);
                break;
            
            case CANDY_PREPROCESSOR_STRING: {
                printf("Reading token type CANDY_PREPROCESSOR_STRING\n");
                candy_preprocessor_string_t *t = (void*) token;
                i += sizeof(*t);
                switch (t->modifier) {
                    case CANDY_ENCODING_MODIFIER_NONE: break;
                    case CANDY_ENCODING_MODIFIER_WIDE: fwrite("L", 1, 1, file_out); break;
                    case CANDY_ENCODING_MODIFIER_UTF8: fwrite("u8", 1, 2, file_out); break;
                    case CANDY_ENCODING_MODIFIER_UTF16: fwrite("u", 1, 1, file_out); break;
                    case CANDY_ENCODING_MODIFIER_UTF32: fwrite("U", 1, 1, file_out); break;
                    default: assert(FALSE, "Unknown encoding modifier type %d\n", t->modifier);
                }
                fwrite("\"", 1, 1, file_out);
                WRITE_STRING();
                fwrite("\"", 1, 1, file_out);
            } break;
            
            case CANDY_PREPROCESSOR_PUNCTUATOR: {
                printf("Reading token type CANDY_PREPROCESSOR_PUNCTUATOR\n");
                candy_preprocessor_punctuator_t *t = (void*) token;
                i += sizeof(*t);
                switch(t->op) {
                    case CANDY_OPERATOR_NONE: break;
                    case CANDY_OPERATOR_CONCATENATE_DIGRAPH:   fwrite("%:%:", 1, 4, file_out); break;
                    case CANDY_OPERATOR_VARIADIC:              fwrite("...", 1, 3, file_out); break;
                    case CANDY_OPERATOR_SHIFT_LEFT_ASSIGN:     fwrite("<<=", 1, 3, file_out); break;
                    case CANDY_OPERATOR_SHIFT_RIGHT_ASSIGN:    fwrite(">>=", 1, 3, file_out); break;
                    case CANDY_OPERATOR_POINTER_ACCESS:        fwrite("->", 1, 2, file_out); break;
                    case CANDY_OPERATOR_INCREMENT:             fwrite("++", 1, 2, file_out); break;
                    case CANDY_OPERATOR_DECREMENT:             fwrite("--", 1, 2, file_out); break;
                    case CANDY_OPERATOR_SHIFT_LEFT:            fwrite("<<", 1, 2, file_out); break;
                    case CANDY_OPERATOR_SHIFT_RIGHT:           fwrite(">>", 1, 2, file_out); break;
                    case CANDY_OPERATOR_LESS_THAN_OR_EQUAL:    fwrite("<=", 1, 2, file_out); break;
                    case CANDY_OPERATOR_GREATER_THAN_OR_EQUAL: fwrite(">=", 1, 2, file_out); break;
                    case CANDY_OPERATOR_EQUAL:                 fwrite("==", 1, 2, file_out); break;
                    case CANDY_OPERATOR_NOT_EQUAL:             fwrite("!=", 1, 2, file_out); break;
                    case CANDY_OPERATOR_LOGICAL_AND:           fwrite("&&", 1, 2, file_out); break;
                    case CANDY_OPERATOR_LOGICAL_OR:            fwrite("||", 1, 2, file_out); break;
                    case CANDY_OPERATOR_PREFIX:                fwrite("::", 1, 2, file_out); break;
                    case CANDY_OPERATOR_MULTIPLY_ASSIGN:       fwrite("*=", 1, 2, file_out); break;
                    case CANDY_OPERATOR_DIVIDE_ASSIGN:         fwrite("/=", 1, 2, file_out); break;
                    case CANDY_OPERATOR_REMAINDER_ASSIGN:      fwrite("%=", 1, 2, file_out); break;
                    case CANDY_OPERATOR_ADD_ASSIGN:            fwrite("+=", 1, 2, file_out); break;
                    case CANDY_OPERATOR_SUBTRACT_ASSIGN:       fwrite("-=", 1, 2, file_out); break;
                    case CANDY_OPERATOR_BITWISE_AND_ASSIGN:    fwrite("&=", 1, 2, file_out); break;
                    case CANDY_OPERATOR_BITWISE_XOR_ASSIGN:    fwrite("^=", 1, 2, file_out); break;
                    case CANDY_OPERATOR_BITWISE_OR_ASSIGN:     fwrite("|=", 1, 2, file_out); break;
                    case CANDY_OPERATOR_CONCATENATE:           fwrite("##", 1, 2, file_out); break;
                    case CANDY_OPERATOR_ARRAY_OPEN_DIGRAPH:    fwrite("<:", 1, 2, file_out); break;
                    case CANDY_OPERATOR_ARRAY_CLOSE_DIGRAPH:   fwrite(":>", 1, 2, file_out); break;
                    case CANDY_OPERATOR_BLOCK_OPEN_DIGRAPH:    fwrite("<%", 1, 2, file_out); break;
                    case CANDY_OPERATOR_BLOCK_CLOSE_DIGRAPH:   fwrite("%>", 1, 2, file_out); break;
                    case CANDY_OPERATOR_PREPROCESS_DIGRAPH:    fwrite("%:", 1, 2, file_out); break;
                    case CANDY_OPERATOR_ARRAY_OPEN:            fwrite("[", 1, 1, file_out); break;
                    case CANDY_OPERATOR_ARRAY_CLOSE:           fwrite("]", 1, 1, file_out); break;
                    case CANDY_OPERATOR_GROUP_OPEN:            fwrite("(", 1, 1, file_out); break;
                    case CANDY_OPERATOR_GROUP_CLOSE:           fwrite(")", 1, 1, file_out); break;
                    case CANDY_OPERATOR_BLOCK_OPEN:            fwrite("{", 1, 1, file_out); break;
                    case CANDY_OPERATOR_BLOCK_CLOSE:           fwrite("}", 1, 1, file_out); break;
                    case CANDY_OPERATOR_STRUCT_ACCESS:         fwrite(".", 1, 1, file_out); break;
                    case CANDY_OPERATOR_BITWISE_AND:           fwrite("&", 1, 1, file_out); break;
                    case CANDY_OPERATOR_MULTIPLY:              fwrite("*", 1, 1, file_out); break;
                    case CANDY_OPERATOR_ADD:                   fwrite("+", 1, 1, file_out); break;
                    case CANDY_OPERATOR_SUBTRACT:              fwrite("-", 1, 1, file_out); break;
                    case CANDY_OPERATOR_BITWISE_NOT:           fwrite("~", 1, 1, file_out); break;
                    case CANDY_OPERATOR_LOGICAL_NOT:           fwrite("!", 1, 1, file_out); break;
                    case CANDY_OPERATOR_DIVIDE:                fwrite("/", 1, 1, file_out); break;
                    case CANDY_OPERATOR_REMAINDER:             fwrite("%", 1, 1, file_out); break;
                    case CANDY_OPERATOR_LESS_THAN:             fwrite("<", 1, 1, file_out); break;
                    case CANDY_OPERATOR_GREATER_THAN:          fwrite(">", 1, 1, file_out); break;
                    case CANDY_OPERATOR_BITWISE_XOR:           fwrite("^", 1, 1, file_out); break;
                    case CANDY_OPERATOR_BITWISE_OR:            fwrite("|", 1, 1, file_out); break;
                    case CANDY_OPERATOR_TERNARY_THEN:          fwrite("?", 1, 1, file_out); break;
                    case CANDY_OPERATOR_TERNARY_ELSE:          fwrite(":", 1, 1, file_out); break;
                    case CANDY_OPERATOR_TERMINATE:             fwrite(";", 1, 1, file_out); break;
                    case CANDY_OPERATOR_ASSIGN:                fwrite("=", 1, 1, file_out); break;
                    case CANDY_OPERATOR_SEPARATE:              fwrite(",", 1, 1, file_out); break;
                    case CANDY_OPERATOR_PREPROCESS:            fwrite("#", 1, 1, file_out); break;
                    default: assert(FALSE, "Unknown operator type %d\n", t->op);
                }
            } break;
            
            case CANDY_PREPROCESSOR_WHITESPACE:
                printf("Reading token type CANDY_PREPROCESSOR_WHITESPACE\n");
                i += sizeof(*token);
                fwrite(" ", 1, 1, file_out);
                break;
            
            case CANDY_PREPROCESSOR_COMMENT:
                printf("Reading token type CANDY_PREPROCESSOR_COMMENT\n");
                i += sizeof(*token);
                fwrite("/*", 1, 2, file_out);
                WRITE_STRING();
                fwrite("*/", 1, 2, file_out);
                break;
            
            case CANDY_PREPROCESSOR_NEWLINE:
                printf("Reading token type CANDY_PREPROCESSOR_NEWLINE\n");
                i += sizeof(*token);
                fwrite("\r\n", 1, 2, file_out);
                break;
            
            case CANDY_PREPROCESSOR_PLACEMARKER:
                printf("Reading token type CANDY_PREPROCESSOR_PLACEMARKER\n");
                i += sizeof(*token);
                break;
            
            default: assert(FALSE, "Unknown preprocessor token type %d\n", token->type);
        }
    }
    
    #undef WRITE_STRING
}

int main(int argc, char **argv) {
    if (argc != 2) print_help_and_exit();
    
    int i, j;
    
    #define COPY_FILENAME() \
        do { \
            for (i = 0; i < BUFFER_SIZE && argv[1][i]; i++) buffer[i] = argv[1][i]; \
            buffer[i] = 0; \
            for (j = i - 1; j >= 0 && buffer[j] != '.'; j--); \
            assert(j <= BUFFER_SIZE - 2, "File name too large\n"); \
        } while (0)
    
    COPY_FILENAME();
    file_in = fopen(buffer, "r");
    assert(file_in, "Source file %s could not be opened\n", buffer);
    buffer[j+1] = 'i';
    buffer[j+2] = 't';
    buffer[j+3] = 0;
    file_out = fopen(buffer, "w");
    assert(file_out, "Token file %s could not be opened\n", buffer);
    tokenizer_pass();
    fclose(file_in);
    fclose(file_out);
    
    COPY_FILENAME();
    buffer[j+1] = 'i';
    buffer[j+2] = 't';
    buffer[j+3] = 0;
    file_in = fopen(buffer, "r");
    assert(file_in, "Token file %s could not be opened\n", buffer);
    buffer[j+2] = 0;
    file_out = fopen(buffer, "w");
    assert(file_out, "Output file %s could not be opened\n", buffer);
    preprocessor_pass();
    fclose(file_in);
    fclose(file_out);
    
    #undef COPY_FILENAME
    
    return 0;
}