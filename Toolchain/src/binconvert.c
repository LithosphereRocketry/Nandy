#include <stdio.h>
#include <string.h>
/**
 * Simple binary file converter for Logisim et al
 * 
 * Assumes 8-bit radix and converts to Logisim memory image format.
*/

int main(int argc, char** argv) {
	if(argc != 4) {
		printf("Usage: %s <format> <input-file> <output-file>\n", argv[0]);
		return 1;
	}

    FILE* in = fopen(argv[2], "rb");
    FILE* out = fopen(argv[3], "w");
    if(!strcmp(argv[1], "l")) { fprintf(out, "v2.0 raw\n"); }
    while(!feof(in)) {
        fprintf(out, "%hhx\n", fgetc(in));
    }
}