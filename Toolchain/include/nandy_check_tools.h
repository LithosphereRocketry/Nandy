#ifndef NANDY_CHECK_TOOLS_H
#define NANDY_CHECK_TOOLS_H

#include "nandy_tools.h"

typedef enum static_interrupt_status {
    STATIC_INTERRUPT_UNKNOWN,
    STATIC_INTERRUPT_ENABLED,
    STATIC_INTERRUPT_DISABLED
} static_interrupt_status_t;

static_interrupt_status_t checkStaticInterruptStatus();

#endif