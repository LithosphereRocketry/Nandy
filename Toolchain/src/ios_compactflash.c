#include "nandy_ios.h"

#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

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

static int serial_fd;
static struct termios serial_settings = {
    .c_cflag = B115200 | CS8 | CLOCAL | CREAD,
    .c_iflag = IGNPAR,
    .c_lflag = 0,
    .c_cc[VMIN] = 1
};

static uint8_t error_register;

static bool feat_8bit;

static void cf_abort() {
    registers[CF_STATUS_CMD] |= 0b1;
    error_register |= 0b101;
}

static void cf_command(uint8_t cmd) {
    error_register = 0;
    switch(cmd) {
        case 0xEF: // set feature
            switch(registers[CF_ERROR_FEATURE]) {
                case 0x01: // enable 8-bit mode
                    feat_8bit = true;
                    break;
                case 0x81: // disable 8-bit mode
                    feat_8bit = false;
                    break;
                default:
                    fprintf(stderr, "Tried to set an unknown feature 0x%hhx",
                        registers[CF_ERROR_FEATURE]);
                    cf_abort();
            }
        default:
            fprintf(stderr, "Received an unknown CF command 0x%hhx", cmd);
                    cf_abort();
    }
}

bool io_step_compactflash(cpu_state_t* cpu, bool active) {

    if(first_access) {
        if(arg_diskser.result.value) {
            serial_fd = open(arg_diskser.result.value, O_RDWR | O_NOCTTY);
            tcflush(serial_fd, TCIFLUSH);
            tcsetattr(serial_fd, TCSANOW, &serial_settings);

        } else if(arg_diskimg.result.value) {
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
        if(cpu->io_rd) {
            if(arg_diskser.result.value) {
                uint8_t msg[] = {reg_sel};
                write(serial_fd, msg, 1);
                read(serial_fd, &cpu->acc, 1);
            } else {
                switch(reg_sel) {
                    case CF_ERROR_FEATURE:
                        cpu->acc = error_register;
                        break;
                    default:
                        cpu->acc = registers[reg_sel];
                }
            }            
        }

        if(cpu->io_wr) {
            if(arg_diskser.result.value) {
                uint8_t msg[] = {0x80 | reg_sel, cpu->acc};
                write(serial_fd, msg, 2);
            } else {
                // write behaviors
                switch(reg_sel) {
                    case CF_STATUS_CMD:
                        cf_command(cpu->acc);
                        break;
                    default:
                        registers[reg_sel] = cpu->acc;
                }
            }
        }
    }
    return false;
}