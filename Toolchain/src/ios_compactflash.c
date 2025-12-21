#include "nandy_ios.h"

#include <stdio.h>

#include "iotools.h"
#include "emu_settings.h"

enum cf_registers {
    CF_DATA = 0,
    CF_ERROR_FEATURE = 1,
    CF_SECTOR_COUNT = 2,
    CF_LBA_7_0 = 3,
    CF_LBA_15_8 = 4,
    CF_LBA_23_16 = 5,
    CF_LBA_27_24_MODE = 6,
    CF_STATUS_CMD = 7
};

static bool first_access = true;
static size_t disksize = -1;
static char* disk = NULL;
static uint8_t registers[8] = {
    [CF_DATA] = 0,
    [CF_ERROR_FEATURE] = 0,
    [CF_SECTOR_COUNT] = 0,
    [CF_LBA_7_0] = 0,
    [CF_LBA_15_8] = 0,
    [CF_LBA_23_16] = 0,
    [CF_LBA_27_24_MODE] = 0b10100000,// 1, LBA enable, 1, DRV, LBA 27-24
    [CF_STATUS_CMD] = 0 // BUSY, RDY, WF, SC, DRQ, CORR, IDX(0), ERR / command
};

void cf_command(uint8_t cmd) {
    switch(cmd) {
        default:
            fprintf(stderr, "Received an unknown CF command 0x%hhx", cmd);
    }
}

bool io_step_compactflash(cpu_state_t* cpu, bool active) {

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
                case CF_STATUS_CMD:
                    
                    break;
                default:
                    registers[reg_sel] = cpu->ioout;
            }
        }
    }
    return false;
}