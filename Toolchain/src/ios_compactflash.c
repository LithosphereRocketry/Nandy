#include "nandy_ios.h"

#include <stdio.h>

#include "iotools.h"
#include "emu_settings.h"

bool io_step_compactflash(cpu_state_t* cpu, bool active) {
    static bool first_access = true;
    static size_t disksize = -1;
    static char* disk = NULL;
    static uint8_t registers[8] = {
        // disk data
        // err / features
        // sector ct
        // LBA 7-0
        // LBA 15-8
        // LBA 23-16
        // 1, LBA enable, 1, DRV, LBA 27-24
        // BUSY, RDY, WF, SC, DRQ, CORR, IDX(0), ERR / command
    };

    if(first_access) {
        if(arg_diskimg.result.value) {
            // To prevent annoying hidden state, don't actually write changes
            // back to disk (at least for now)
            FILE* diskfile = fopen(arg_diskimg.result.value, "rb");
            if(diskfile) {
                disksize = fsize(diskfile);
                disk = malloc(disksize);
                fread(disk, 1, disksize, diskfile);
            } else {
                fprintf(stderr, "Warning, could not open file '%s', defaulting to no disk", arg_diskimg.result.value);
            }
        }
        first_access = false;
    }

    if(active) {
        int reg_sel = cpu->y & 0b111;
        
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