#include <stdio.h>
#include "n_malloc.h"

int main(int argc, char** argv) {
    n_malloc_init();
    int a = n_malloc(15);
    int b = n_malloc(47);
    int c = n_malloc(1234);
    int d = n_malloc(99999);
    printf("should be 0: %x\n", d);
    int e = n_malloc(100);
    n_free(c);
    int f = n_malloc(1279);
    int g = n_malloc(1214);
    n_free(b);
    n_free(g);
    n_free(e);
    int h = n_malloc(1300);
    int i = n_malloc(79);
    int j = n_malloc(0);
    n_free(a);
    n_free(f);
    n_free(h);
    n_free(i);
    n_free(j);
    int k = n_malloc(0x7EFD);
    int l = n_malloc(0);
    n_free(k);
    printf("should be 0: %x\n", l);
}

