#ifndef NANDY_CHECK_TOOLS_H
#define NANDY_CHECK_TOOLS_H

#include "nandy_tools.h"

typedef enum int_state {
    INT_STATE_UNKNOWN,
    INT_STATE_ENABLED,
    INT_STATE_DISABLED
} int_state_t;

int_state_t checkIntState();

#endif