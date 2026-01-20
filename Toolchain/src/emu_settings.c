#include "argparse.h"

argument_t arg_debug = { .abbr = 'g', .name = "debug", .hasval = false,
    .help="Open debugger when a BRK instruction is encountered"};
argument_t arg_forcedebug = { .abbr = 'G', .name = "force-debug", .hasval = false,
    .help="Open debugger immediately on startup (implies -g)"};
argument_t arg_out = { .abbr = 'o', .name = "out", .hasval = true };
argument_t arg_timeacc = { .abbr = 't', .name = "time-accurate", .hasval = false,
    .help="Attempt to emulate actual execution speed"};
argument_t arg_diskimg = { .abbr = 'd', .name = "disk-image", .hasval = true,
    .help="Emulate CompactFlash card using specified disk image"};
argument_t arg_diskser = { .abbr = 'S', .name = "cf-serial", .hasval = true,
    .help="Emulate CompactFlash card using physical card attached via serial"};

argument_t* args[] = {
    &arg_debug,
    &arg_forcedebug,
    &arg_out,
    &arg_timeacc,
    &arg_diskimg,
    &arg_diskser
};
const size_t n_args = sizeof(args) / sizeof(argument_t*);
