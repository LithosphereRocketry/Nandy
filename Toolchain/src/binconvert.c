#include <stdio.h>
/**
 * Simple binary file converter for Logisim et al
 * 
 * Assumes 8-bit radix and converts to Logisim memory image format.
*/

int main(int argc, char** argv) {
	if(argc != 3) {
		printf("Usage: %s <input-file> <output-file>\n", argv[0]);
		return 1;
	}

    FILE* in = fopen(argv[1], "rb");
    FILE* out = fopen(argv[2], "w");
    fprintf(out, "v2.0 raw\n");
    while(!feof(in)) {
        fprintf(out, "%hhx\n", fgetc(in));
    }
}