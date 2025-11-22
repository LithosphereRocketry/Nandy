#include "nandy_ios.h"

#include <stdio.h>

#include "stdin_avail.h"

// roughly 9600 8N1
static const size_t SERIAL_COOLDOWN = 1e6 / 9600 / 10;

bool io_step_tty(cpu_state_t* cpu, bool active) {
    static bool input_available = false;
    static word_t incoming_input;

    static size_t cooldown = 0;
    
    if(cooldown) cooldown--;

    if(!input_available && !cooldown && stdinAvail()) {
        incoming_input = getc(stdin);
        input_available = true;
        cooldown = SERIAL_COOLDOWN;
    }
    
    if(active) {
        cpu->ioin = incoming_input;
        if(cpu->io_rd) {
            input_available = false;
        }
        if(cpu->io_wr) {
            putc(cpu->ioout, stdout);
        }
    }
    
    return input_available;
}