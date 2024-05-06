#include "lcdprint.h"
#include <stdio.h>

void lcd_clock(lcd_t* lcd, bool active) {
    lcd->ir = lcd->db;
    if(lcd->rs) {
        if(lcd->rw) { // read RAM
            if(lcd->ramsel) {
                if(lcd->ac < 64) {
                    lcd->ir = lcd->cgram[lcd->ac];
                } else {
                    fprintf(stderr, "CGRAM out of bounds\n");
                }
            } else {
                if(lcd->ac < 0x40) {
                    if(lcd->ac < 40) {
                        lcd->ir = lcd->ddram_b1[lcd->ac];
                    } else {
                        fprintf(stderr, "DDRAM out of bounds\n");
                    }
                } else {
                    if(lcd->ac < 0x40 + 40) {
                        lcd->ir = lcd->ddram_b1[lcd->ac - 0x40];
                    } else {
                        fprintf(stderr, "DDRAM out of bounds\n");
                    }
                }
            }
        } else { // write RAM
            if(lcd->ramsel) {
                if(lcd->ac < 64) {
                    lcd->cgram[lcd->ac] = lcd->ir;
                } else {
                    fprintf(stderr, "CGRAM out of bounds\n");
                }
            } else {
                if(lcd->ac < 0x40) {
                    if(lcd->ac < 40) {
                        lcd->ddram_b1[lcd->ac] = lcd->ir;
                    } else {
                        fprintf(stderr, "DDRAM out of bounds\n");
                    }
                } else {
                    if(lcd->ac < 0x40 + 40) {
                        lcd->ddram_b1[lcd->ac - 0x40] = lcd->ir;
                    } else {
                        fprintf(stderr, "DDRAM out of bounds\n");
                    }
                }
            }
        }
    } else if(lcd->rw) { // read BF/AC

    } else if(lcd->ir & 0b10000000) { // Set DDRAM address
        lcd->ramsel = false;
        lcd->ac = lcd->ir & 0b1111111;
    } else if(lcd->ir & 0b01000000) { // Set CGRAM address
        lcd->ramsel = true;
        lcd->ac = lcd->ir & 0b111111;
    } else if(lcd->ir & 0b00100000) { // Function set
        lcd->dl = lcd->ir & 0b10000;
        lcd->n = lcd->ir & 0b1000;
        lcd->f = lcd->ir & 0b100;
    } else if(lcd->ir & 0b00010000) { // Cursor or display shift
        int dir = (lcd->ir & 0b100) ? 1 : -1;
        if(lcd->ir & 0b1000) { // shift
            lcd->scroll += dir;
        } else { // cursor
            lcd->ac += dir;
        }
        // TODO: what happens if you shift off the end of a line
    } else if(lcd->ir & 0b00001000) { // Display on/off control
        lcd->d = lcd->ir & 0b100;
        lcd->c = lcd->ir & 0b10;
        lcd->b = lcd->ir & 0b1;
    } else if(lcd->ir & 0b00000100) { // Entry mode set
        lcd->id = lcd->ir & 0b10;
        lcd->s = lcd->ir & 0b1;
    } else if(lcd->ir & 0b00000010) { // Return home
        lcd->ramsel = false;
        lcd->ac = 0;
        lcd->scroll = 0;
    } else if(lcd->ir & 0b00000001) { // Clear display
        for(int i = 0; i < 40; i++) {
            lcd->ddram_b1[i] = 0x20;
            lcd->ddram_b2[i] = 0x20;
        }
        lcd->ac = 0;
        lcd->ramsel = false;
        lcd->id = true;
        lcd->scroll = 0;
    }
}

void lcd_print(const lcd_t* lcd) {

}