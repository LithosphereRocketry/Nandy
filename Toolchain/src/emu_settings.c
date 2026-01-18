#include "argparse.h"

argument_t arg_debug = { .abbr = 'g', .name = "debug", .hasval = false };
argument_t arg_forcedebug = { .abbr = 'G', .name = "force-debug", .hasval = false };
argument_t arg_out = { .abbr = 'o', .name = "out", .hasval = true };
argument_t arg_timeacc = { .abbr = 't', .name = "time-accurate", .hasval = false };
argument_t arg_diskimg = { .abbr = 'd', .name = "disk-image", .hasval = true };
argument_t arg_diskser = { .abbr = 'S', .name = "cf-serial", .hasval = true };

argument_t* args[] = {
    &arg_debug,
    &arg_forcedebug,
    &arg_out,
    &arg_timeacc,
    &arg_diskimg,
    &arg_diskser
};
const size_t n_args = sizeof(args) / sizeof(argument_t*);
