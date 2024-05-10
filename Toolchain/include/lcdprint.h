#ifndef LCDPRINT_H
#define LCDPRINT_H

#include <stdint.h>
#include <stdbool.h>

// Assumes 1MHz I/O clock, near maximum for both HD44780 and NANDy

typedef struct lcd {
    // Pins
    uint8_t db;
    bool rw;
    bool rs;
    // Internal
    uint8_t ir;
    uint8_t ac;
    bool id, s, d, c, b, dl, n, f, bf, ramsel, cycle;
    uint8_t scroll;
    uint8_t ddram_b1[40];
    uint8_t ddram_b2[40];
    uint8_t cgram[64]; // Custom char printing not supported
    long cooldown;
} lcd_t;

void lcd_clock(lcd_t* lcd, bool active);
void lcd_print(const lcd_t* lcd);

#endif