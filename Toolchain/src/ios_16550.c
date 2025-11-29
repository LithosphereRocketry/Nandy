#include "nandy_ios.h"

#include <stdio.h>

#include "stdin_avail.h"

static size_t REFCLK = 1843200;

bool io_step_16550(cpu_state_t* cpu, bool active) {
    static uint8_t registers[8] = {

    };
    // it's easier to make a circular fifo with one space always free, even if
    // mod is a little slower than bitand
    static uint16_t fifo[17];

    if(active) {
        int reg_sel = cpu->ioaddr & 0b111;
        
        // Read behaviors
        switch(reg_sel) {
            default:
                cpu->ioin = registers[reg_sel];
        }

        if(cpu->io_wr) {
            switch(reg_sel) {
                default:
                    registers[reg_sel] = cpu->ioout;
            }
        }
    }
    return false;
}