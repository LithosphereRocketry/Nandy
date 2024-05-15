#include "n_malloc.h"

#include <stdio.h>

#define MAX_MEM 0xFF00
#define FREE_MEM 0x8000
#define MIN_ALLOC 1
char mem[MAX_MEM];

void n_malloc_init() {
    mem[FREE_MEM] = MAX_MEM & 0xFF;
    mem[FREE_MEM + 1] = (MAX_MEM >> 8) & 0x7F;
}

int n_malloc(int sz) {
    int index = FREE_MEM;
    while(index + 1 < MAX_MEM) {
        int next = 0x8000 | ((mem[index + 1] & 0x7F) << 8) | mem[index];
        if(!(mem[index + 1] & 0x80)) {
            // Attempt to coalesce all following allocations into a single block
            while(next + 1 < MAX_MEM && !(mem[next + 1] & 0x80)) {
                next = 0x8000 | ((mem[next + 1] & 0x7F) << 8) | mem[next];
            }
            if(sz + 2 < next - index) {
                // This is our block
                if(sz + 4 + MIN_ALLOC < next - index) {
                    // There's enough space for another block after this
                    int nextaddr = index + sz + 2;
                    // Our block
                    mem[index + 1] = 0x80 | ((nextaddr >> 8) & 0x7F);
                    mem[index] = nextaddr & 0xFF;
                    // Leftover block
                    mem[nextaddr + 1] = ((next >> 8) & 0x7F);
                    mem[nextaddr] = next & 0xFF;
                } else {
                    // Not enough space, just claim the current block
                    mem[index + 1] = 0x80 | ((next >> 8) & 0x7F);
                    mem[index] = next & 0xFF;
                }
                printf("Allocated %i bytes at %x\n", sz, index + 2);
                return index + 2;
            } else {
                // This isn't big enough, keep looking but save our coalesced
                // size
                mem[index + 1] = ((next >> 8) & 0x7F);
                mem[index] = next & 0xFF;
            }
        }
        index = next;
    }
    printf("Failed to allocate %i bytes\n", sz);
    return 0;
}

void n_free(int addr) {
    printf("Freed allocation at %x\n", addr);
    mem[addr - 1] &= 0x7F;
}