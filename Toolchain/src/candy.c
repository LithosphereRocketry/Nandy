#define DEBUG

// CREDIT: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3096.pdf

#include "iotools.h"
#include "candy.h"

// static void consume_whitespace(char **text) {
//     while (is_whitespace(**text)) (*text)++;
// }

// static string_t next_token(char **text) {
//     consume_whitespace(text);
//     string_t result = {*text, 0};
//     while (result.text) result.text++, result.count++;
//     return result;
// }

// static string_t apply_tokenizer_pass(char *text) {
//     string_t t = {text, 0};
    
//     while (*text) {
//         //TODO: Actually implement this
//         t.count++;
//     }
    
//     return t;
// }

// static void apply_preprocessor_pass(char *text) {
//     while (**text) {
//         string_t token = next_token(text);
        
        
//     }
// }

int main(int argc, char **argv) {
    assert(argc == 2, "Candy needs exactly one argument, the filename to compile, but was given %d arguments\n", argc - 1);
    
    FILE* f = fopen(argv[1], "r");
    assert(f, "File %s was not found\n", argv[1]);
    char* text = fread_dup(f);
    fclose(f);
    assert(text, "Out of memory\n");
    
    // printf("Text: %s\n", text);
    // string_t text = apply_unicode_translation_pass(text);
    
    // apply_preprocessor_pass(text);
    
    free(text);
    return 0;
}