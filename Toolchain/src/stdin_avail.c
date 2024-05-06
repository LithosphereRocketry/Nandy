#include "stdin_avail.h"

#include <stdio.h>

#ifdef __unix__
#include <poll.h>

bool stdinAvail() {
    struct pollfd pfd = {
        .fd = fileno(stdin),
        .events = POLLIN
    };
    return poll(&pfd, 1, 0) > 0 && pfd.revents & POLLIN;
}

#else
#error "Only POSIX supported for input polling at this time"
#endif