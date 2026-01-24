#include "nandy_ios.h"

#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include "iotools.h"
#include "emu_settings.h"

#define SERIAL_SPEED B115200

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

enum cf_status {
    CF_DRQ = 0b00001000
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
static struct termios serial_settings;

static uint8_t error_register;

static bool feat_8bit;

static char* seek_ptr = NULL;
static size_t region_size = 0;

static void cf_abort() {
    registers[CF_STATUS_CMD] |= 0b1;
    error_register |= 0b101;
}

// place a 16 bit value in correct endianness into two 8-bit entries
// I can't find super clear documentation on what endianness CF IDENTIFY DRIVE
// uses so I'm doing it this way so it can be easily changed
static void put16(char* buf, uint16_t val) {
    // I *think* LE is correct
    buf[0] = val & 0xFF;
    buf[1] = val >> 8;
}

static char ident_buffer[512];
static void set_up_ident() {
    size_t n_sectors = disksize/512;
    if(n_sectors > 0xFFFFFFFF) {
        fprintf(stderr, "Warning: disk size of %zu sectors does not fit in 32-"
                "bit disk size parameter, size may be truncated\n", n_sectors);
    }

    memset(ident_buffer, 0, 512); // probably unnecessary but it doesn't hurt
    put16(ident_buffer + 2*0, 0x848A); // CF removable disk signature
    // TODO heads
    put16(ident_buffer + 2*5, 512); // bytes/sector
    // TODO sectors/track
    // TODO sectors/card
    memcpy(ident_buffer + 2*10, "SERIAL_NUMBER_123456", 20);
    put16(ident_buffer + 2*20, 2); // buffer type
    put16(ident_buffer + 2*21, 2); // buffer size
    put16(ident_buffer + 2*22, 4); // ECC bytes on R/W Long
    memcpy(ident_buffer + 2*23, "FW111111", 8);
    memcpy(ident_buffer + 2*27, "MODEL_NUMBER_1234567890_ABCDEFG_0000000", 40);
    put16(ident_buffer + 2*47, 1); // max 1 sector on R/W multiple
    put16(ident_buffer + 2*49, 0x0200); // LBA supported
    put16(ident_buffer + 2*51, 0x0200); // PIO mode 2
    put16(ident_buffer + 2*53, 3); // field validity (?)
    // TODO current cylinders
    // TODO current heads
    // TODO current sectors
    put16(ident_buffer + 2*57, n_sectors & 0xFFFF); // current LBA capacity
    put16(ident_buffer + 2*58, n_sectors >> 16);
    put16(ident_buffer + 2*59, 0x0100); // multiple sector setting is valid (?)
    put16(ident_buffer + 2*57, n_sectors & 0xFFFF); // total number of LBA sect.
    put16(ident_buffer + 2*58, n_sectors >> 16); // is this ever different?
    // I don't use PIO so these numbers are pretty irrelevant
    put16(ident_buffer + 2*64, 3); // advanced PIO supported (lies)
    put16(ident_buffer + 2*64, 78); // PIO cycle time w/o flow control
    put16(ident_buffer + 2*64, 78); // PIO cycle time w/ flow control
}

static void seek_to(char* buf, size_t size) {
    seek_ptr = buf;
    region_size = size;
    if(size) registers[CF_STATUS_CMD] |= CF_DRQ;
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
            break;
        case 0xEC: // identify drive
            seek_to(ident_buffer, 512);
            break;
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
            tcgetattr(serial_fd, &serial_settings);
            cfsetispeed(&serial_settings, SERIAL_SPEED);
            cfsetospeed(&serial_settings, SERIAL_SPEED);
            serial_settings.c_cflag &= ~(CSIZE | PARENB);
            serial_settings.c_cflag |= CS8 | CLOCAL | CREAD;
            serial_settings.c_lflag &= ~(ICANON | ECHO);
            serial_settings.c_cc[VMIN] = 1;
            serial_settings.c_cc[VTIME] = 0;
            tcsetattr(serial_fd, TCSANOW, &serial_settings);
            // Opening the serial port causes the arduino to reset, which takes
            // a shockingly long time, so wait for it to send us a byte before
            // we start blasting it

            uint8_t ack;
            read(serial_fd, &ack, 1);
            if(ack != 0xF0) {
                fprintf(stderr, "Unexpected serial acknowledge 0x%hhx\n", ack);
                exit(-1);
            }

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
            set_up_ident();
        }
        first_access = false;
    }

    if(active) {
        int reg_sel = cpu->y & 0b111;
        
        // Read behaviors
        if(cpu->io_rd) {
            if(arg_diskser.result.value) {
                uint8_t msg[] = {reg_sel};
                if(write(serial_fd, msg, 1) != 1) {
                    fprintf(stderr, "Serial didn't send command!\n");
                    exit(-1);
                }
                if(read(serial_fd, &cpu->acc, 1) != 1) {
                    fprintf(stderr, "Serial didn't get data!\n");
                    exit(-1);
                }
            } else if(disk) {
                switch(reg_sel) {
                case CF_ERROR_FEATURE:
                    cpu->acc = error_register;
                    break;
                case CF_DATA:
                    if(seek_ptr) {
                        cpu->acc = *seek_ptr;
                        if(cpu->io_rd) {
                            seek_ptr ++;
                            region_size --;
                            if(!region_size) {
                                registers[CF_STATUS_CMD] &= ~CF_DRQ;
                                seek_ptr = NULL;
                            }
                        }
                    }
                    break;
                default:
                    cpu->acc = registers[reg_sel];
                }            
            } else {
                cpu->acc = 0xFF;
            }
        }

        if(cpu->io_wr) {
            if(arg_diskser.result.value) {
                uint8_t msg[] = {0x80 | reg_sel, cpu->acc};
                if(write(serial_fd, msg, 2) != 2) {
                    fprintf(stderr, "Serial didn't send data!\n");
                    exit(-1);
                }
            } else if(disk) {
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