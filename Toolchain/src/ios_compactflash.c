#include "nandy_ios.h"

#include <stdio.h>
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
                if(write(serial_fd, msg, 2) != 2) {
                    fprintf(stderr, "Serial didn't send data!\n");
                    exit(-1);
                }
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