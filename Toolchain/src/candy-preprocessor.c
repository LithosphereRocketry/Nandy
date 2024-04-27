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

int read_more(int *index) {
    // Write what we have to the output
    fwrite(buffer, 1, *index, file_out);
    
    // Copy over what we want to keep
    int bytes_to_keep = BUFFER_SIZE - *index;
    memmove(buffer, buffer + *index, bytes_to_keep);
    
    // Read in the rest
    *index = 0;
    return read_into_buffer(bytes_to_keep);
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
    
    char *c = buffer;
    while (c[i]) {
        if (i >= BUFFER_SIZE - 32) read_more(&i);
        
        if (is_whitespace(c[i])) {
            i++;
            continue;
        }
        
        //TODO: Unicode
        
        // Handle escaped newlines
        if (c[i] == '\\') {
            assert(c[i+1], "Translation units cannot end with a backslash\n");
            c[i] = ' ';
            if (c[i+1] == '\r' && c[i+2] == '\n')
                c[i+1] = ' ', c[i+2] = ' ', i += 3;
            if (c[i+1] == '\n')
                c[i+1] = ' ', i += 2;
            continue;
        }
        
        i++;
    }
    
    fwrite(buffer, 1, i, file_out);
    
    fclose(file_in);
    fclose(file_out);
    return 0;
}