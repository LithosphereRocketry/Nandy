#ifndef N_MALLOC_H
#define N_MALLOC_H

void n_malloc_init();
int n_malloc(int sz);
void n_free(int addr);

#endif