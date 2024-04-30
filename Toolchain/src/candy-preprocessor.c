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

void tokenizer_pass(void) {
    int i = 0;
    read_into_buffer(0);
    
    #define END_WHITESPACE() \
        do { \
            if (in_whitespace) { \
                printf("Creating token type %d\n", CANDY_PREPROCESSOR_WHITESPACE); \
                candy_preprocessor_token_t t = {CANDY_PREPROCESSOR_WHITESPACE}; \
                fwrite(&t, sizeof(t), 1, file_out); \
                in_whitespace = FALSE; \
            } \
        } while (0)
    
    #define END_STRING() \
        do { \
            if (was_string) {\
                encoding_modifier = CANDY_ENCODING_MODIFIER_NONE; \
                was_string = FALSE; \
                fwrite(&(char){0}, sizeof(char), 1, file_out); \
            } \
        } while (0)
    
    int in_whitespace = FALSE;
    int in_string = FALSE;
    int in_character = FALSE;
    int was_string = FALSE;
    int was_newline = FALSE;
    candy_encoding_modifier_t encoding_modifier = CANDY_ENCODING_MODIFIER_NONE;
    int comment = 0;
    int header_name = 0;
    char *c = buffer;
    while (c[i]) {
        if (i >= BUFFER_SIZE - PADDING) read_more(&i, FALSE);
        int linebreak = linebreak_length(c+i);
        
        // Remove escaped newlines
        if (c[i] == '\\') {
            assert(c[i+1], "Translation units cannot end with a backslash\n");
            i += 1;
            linebreak = linebreak_length(c+i);
            i += linebreak;
            continue;
        }
        
        // Header names
        if (header_name > 1) {
            if ((header_name == CANDY_PREPROCESSOR_INCLUDE_HEADER_NAME && c[i] == '>')
                || (header_name == CANDY_PREPROCESSOR_LOCAL_HEADER_NAME && c[i] == '"')) {
                fwrite(&(char){0}, sizeof(char), 1, file_out);
                header_name = 0;
            } else {
                fwrite(c+i, 1, 1, file_out);
            }
            i += 1;
            continue;
        }
        
        // Characters
        if (in_character) {
            assert(!linebreak_length(c), "Cannot have a line break inside a character constant\n");
            if (c[i] == '\'') {
                fwrite(&(char){0}, sizeof(char), 1, file_out);
                in_character = FALSE;
                encoding_modifier = CANDY_ENCODING_MODIFIER_NONE;
                i += 1;
                continue;
            }
            else if (c[i] == '\\') {
                
            }
            fwrite(c+i, sizeof(char), 1, file_out);
            i += 1;
            continue;
        }
        
        // Strings
        if (in_string) {
            assert(!linebreak_length(c), "Cannot have a line break inside a string\n");
            if (c[i] == '"') {
                in_string = FALSE;
                was_string = TRUE;
                i += 1;
                continue;
            }
            if (c[i] == '\\') {
                
            }
            fwrite(c+i, sizeof(char), 1, file_out);
            i += 1;
            continue;
        }
        
        // Comments
        if (!comment && c[i] == '/') {
            if (c[i+1] == '/') comment = 1;
            else if (c[i+1] == '*') comment = 2;
            if (comment) {
                printf("Creating token type CANDY_PREPROCESSOR_COMMENT\n");
                candy_preprocessor_token_t t = {CANDY_PREPROCESSOR_COMMENT};
                fwrite(&t, sizeof(t), 1, file_out);
                i += 2;
                continue;
            }
        }
        if (comment) {
            if (comment == 1 && linebreak) {
                comment = 0;
                fwrite(&(char){0}, sizeof(char), 1, file_out);
                continue;
            } else if (comment == 2 && c[i] == '*' && c[i+1] == '/') {
                comment = 0;
                fwrite(&(char){0}, sizeof(char), 1, file_out);
                i += 2;
            } else {
                fwrite(c+i, sizeof(char), 1, file_out);
                i += 1;
            }
            continue;
        }
        
        // Keep newlines
        if (linebreak) {
            END_STRING();
            END_WHITESPACE();
            was_newline = TRUE;
            assert(!header_name, "Cannot have a newline in a header name\n");
            printf("Creating token type CANDY_PREPROCESSOR_NEWLINE\n");
            candy_preprocessor_token_t t = {CANDY_PREPROCESSOR_NEWLINE};
            fwrite(&t, sizeof(t), 1, file_out);
            i += linebreak;
            continue;
        }
        
        was_newline = FALSE;
        switch (c[i]) {
            // Flatten whitespace into a single space
            case '\r':
            case '\t':
            case '\v':
            case '\f':
            case ' ': {
                in_whitespace = TRUE;
                i++;
            } break;
            
            case '\'':
                END_WHITESPACE();
                printf("Creating token type CANDY_PREPROCESSOR_CHARACTER\n");
                candy_preprocessor_character_t t = {CANDY_PREPROCESSOR_CHARACTER, encoding_modifier};
                fwrite(&t, sizeof(t), 1, file_out);
                in_character = TRUE;
                i += 1;
                break;
            
            case '"':
                if (!header_name) {
                    if (!was_string) {
                        END_WHITESPACE();
                        printf("Creating token type CANDY_PREPROCESSOR_STRING\n");
                        candy_preprocessor_string_t t = {CANDY_PREPROCESSOR_STRING, encoding_modifier};
                        fwrite(&t, sizeof(t), 1, file_out);
                    }
                    in_string = TRUE;
                    i += 1;
                    break;
                }
                // Fall through
            case '<':
                if (header_name == 1) {
                    was_string = FALSE;
                    END_WHITESPACE();
                    header_name = c[i] == '"'
                        ? CANDY_PREPROCESSOR_LOCAL_HEADER_NAME
                        : CANDY_PREPROCESSOR_INCLUDE_HEADER_NAME;
                    if (c[i] == '"') printf("Creating token type CANDY_PREPROCESSOR_LOCAL_HEADER_NAME\n");
                    else printf("Creating token type CANDY_PREPROCESSOR_INCLUDE_HEADER_NAME\n");
                    candy_preprocessor_token_t t = {header_name};
                    fwrite(&t, sizeof(t), 1, file_out);
                    i += 1;
                    break;
                }
                // Fall through
            case '>':
            case '[':
            case ']':
            case '(':
            case ')':
            case '{':
            case '}':
            case '.':
            case '&':
            case '*':
            case '+':
            case '-':
            case '~':
            case '!':
            case '/':
            case '%':
            case '^':
            case '|':
            case '?':
            case ':':
            case ';':
            case '=':
            case ',':
            case '#': {
                was_string = FALSE;
                END_WHITESPACE();
                END_STRING();
                assert(header_name != 1, "Encountered '%c' instead of a header name\n", c[i]);
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
            
            default:
                //TODO: Unicode
                was_string = FALSE;
                END_WHITESPACE();
                assert(header_name != 1, "Encountered '%c' instead of a header name", c[i]);
                
                candy_encoding_modifier_t old_modifier = encoding_modifier;
                if (c[i] == 'L' && (c[i+1] == '"' || c[i+1] == '\''))
                    i += 1, encoding_modifier = CANDY_ENCODING_MODIFIER_WIDE;
                else if (c[i] == 'u' && c[i+1] == '8' && (c[i+2] == '"' || c[i+2] == '\''))
                    i += 2, encoding_modifier = CANDY_ENCODING_MODIFIER_UTF8;
                else if (c[i] == 'u' && (c[i+1] == '"' || c[i+1] == '\''))
                    i += 1, encoding_modifier = CANDY_ENCODING_MODIFIER_UTF16;
                else if (c[i] == 'U' && (c[i+1] == '"' || c[i+1] == '\''))
                    i += 1, encoding_modifier = CANDY_ENCODING_MODIFIER_UTF32;
                else {
                    END_STRING();
                    i++;
                }
                assert(old_modifier == CANDY_ENCODING_MODIFIER_NONE || old_modifier == encoding_modifier, "Concatenated string literals cannot have different modifiers\n");
        }
    }
    
    assert(was_newline, "Translation units must end with a newline\n");
    assert(!in_character, "Incomplete character constant at the end of the translation unit\n");
    assert(!in_string, "Incomplete string at the end of the translation unit\n");
    assert(!comment, "Incomplete comment at the end of the translation unit\n");
    assert(!header_name, "Incomplete header name at the end of the translation unit\n");
    
    END_WHITESPACE();
    END_STRING();
    
    #undef END_STRING
    #undef END_WHITESPACE
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
        printf("Reading token type %d\n", token->type);
        switch (token->type) {
            case CANDY_PREPROCESSOR_NONE: break;
            
            case CANDY_PREPROCESSOR_INCLUDE_HEADER_NAME:
                i += sizeof(*token);
                fwrite("<", 1, 1, file_out);
                WRITE_STRING();
                fwrite(">", 1, 1, file_out);
                break;
            
            case CANDY_PREPROCESSOR_LOCAL_HEADER_NAME:
                i += sizeof(*token);
                fwrite("\"", 1, 1, file_out);
                WRITE_STRING();
                fwrite("\"", 1, 1, file_out);
                break;
            
            case CANDY_PREPROCESSOR_IDENTIFIER:
                i += sizeof(*token);
                break;
            
            case CANDY_PREPROCESSOR_NUMBER:
                i += sizeof(*token);
                break;
            
            case CANDY_PREPROCESSOR_CHARACTER:
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
            
            case CANDY_PREPROCESSOR_PLACEMARKER:
                i += sizeof(*token);
                break;
            
            case CANDY_PREPROCESSOR_WHITESPACE:
                i += sizeof(*token);
                fwrite(" ", 1, 1, file_out);
                break;
            
            case CANDY_PREPROCESSOR_COMMENT:
                i += sizeof(*token);
                fwrite("/*", 1, 2, file_out);
                WRITE_STRING();
                fwrite("*/", 1, 2, file_out);
                break;
            
            case CANDY_PREPROCESSOR_NEWLINE:
                i += sizeof(*token);
                fwrite("\r\n", 1, 2, file_out);
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