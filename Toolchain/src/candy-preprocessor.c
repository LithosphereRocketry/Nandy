#include <string.h>

#define DEBUG

#include "candy-preprocessor.h"
#include "stringtools.h"

#define BUFFER_SIZE 512
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

int main(int argc, char **argv) {
    if (argc != 2) print_help_and_exit();
    
    file_in = fopen(argv[1], "r");
    assert(file_in, "File %s was not found\n", argv[1]);
    
    int i = 0;
    for (; i < BUFFER_SIZE && argv[1][i]; i++) buffer[i] = argv[1][i];
    buffer[i] = 0;
    for (int j = i - 1; j >= 0; j--) {
        if (buffer[j] == '.' && j <= BUFFER_SIZE-2) {
            buffer[j+1] = 'i';
            buffer[j+2] = 0;
            break;
        }
    }
    file_out = fopen(buffer, "w+");
    assert(file_out, "File %s was not found\n", buffer);
    
    i = 0;
    read_into_buffer(0);
    
    int in_whitespace = FALSE;
    int was_newline = FALSE;
    char *c = buffer;
    while (c[i]) {
        if (i >= BUFFER_SIZE - 32) read_more(&i, !in_whitespace);
        
        was_newline = FALSE;
        switch (c[i]) {
            // Remove escaped newlines
            case '\\':
                assert(c[i+1], "Translation units cannot end with a backslash\n");
                i += 1;
                int linebreak = linebreak_length(c+i);
                if (in_whitespace) {
                    i += linebreak;
                } else if (linebreak) {
                    if (i > 1) fwrite(buffer, 1, i-1, file_out);
                    i += linebreak;
                    read_more(&i, FALSE);
                }
                break;
            
            // Keep newlines, flatten whitespace into a single space
            case '\n':
                if (in_whitespace) {
                    read_more(&i, FALSE);
                    in_whitespace = FALSE;
                }
                was_newline = TRUE;
                i++;
                break;
            case '\r':
                if (c[i+1] == '\n') {
                    if (in_whitespace) {
                        read_more(&i, FALSE);
                        in_whitespace = FALSE;
                    }
                    was_newline = TRUE;
                    i += 2;
                    break;
                }
                // Needed because of GCC warnings, yay...:
                // Fall through
            case '\t':
            case '\v':
            case '\f':
                if (!in_whitespace) c[i] = ' ';
                // Fall through
            case ' ':
                i++;
                if (!in_whitespace) {
                    fwrite(buffer, 1, i, file_out);
                    in_whitespace = TRUE;
                }
                break;
            
            default:
                //TODO: Unicode
                if (in_whitespace) {
                    read_more(&i, FALSE);
                    in_whitespace = FALSE;
                }
                i++;
        }
    }
    
    assert(was_newline, "Translation units must end with a newline\n");
    
    fwrite(buffer, 1, i, file_out);
    
    fclose(file_in);
    fclose(file_out);
    return 0;
}