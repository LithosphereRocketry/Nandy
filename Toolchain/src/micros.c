#include "micros.h"

static unsigned long micros_at_init = 0;
void micros_init() {
    micros_at_init = micros();
}

#ifdef __unix__
#include <time.h>

long micros() {
    struct timespec tms;
    clock_gettime(CLOCK_MONOTONIC, &tms);
    return (tms.tv_sec * 1000000) + (tms.tv_nsec / 1000) - micros_at_init;
}

#else
#error "Only POSIX supported for timers at this time"
#endif