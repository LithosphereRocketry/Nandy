#ifndef EMU_SETTINGS_H
#define EMU_SETTINGS_H

#include "argparse.h"

extern argument_t arg_debug, arg_forcedebug, arg_out, arg_timeacc, arg_diskimg;

extern argument_t* args[];
extern const size_t n_args;

#endif