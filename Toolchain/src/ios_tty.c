#include "nandy_ios.h"
#include "stdin_avail.h"

FILE* tty_outstream;

#define COOLDOWN (1000000 / 1200)
bool io_step_tty(cpu_state_t* cpu, bool active) {
    static size_t lastIOcycle = 0;
    static bool interrupt = false;
    static word_t incoming = 0;
    if(active && cpu->io_wr) {
        putc(cpu->ioout, tty_outstream);
        fflush(tty_outstream);
    }
    if(stdinAvail() && cpu->elapsed - lastIOcycle > COOLDOWN) {
        incoming = getc(stdin);
        interrupt = true;
        lastIOcycle = cpu->elapsed;
    }
    if(active) {
        cpu->ioin |= incoming;
        if(cpu->io_rd) { interrupt = false; }
    }
    return interrupt;
}