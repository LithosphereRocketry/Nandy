#ifndef STDIN_AVAIL_H
#define STDIN_AVAIL_H

#include <stdbool.h>

// Returns whether characters are available to be read on stdin. For this
// function to work correctly, stdin must be *unbuffered*.
bool stdinAvail();

#endif