#ifndef TEST_BOARD_H
#define TEST_BOARD_H

#include <Arduino.h>
#include <LiquidCrystal.h>


namespace tb {

enum ports {
    PORT_A,
    PORT_B,
    PORT_C,
    PORT_D,
    PORT_E,
    PORT_F
};

extern LiquidCrystal lcd;

namespace pins {
    const uint8_t lcd_d[] = {-1, -1, -1, -1, 14, 15, 16, 17};
    const uint8_t lcd_rs = 38;
    const uint8_t lcd_e = 39;
    const uint8_t lcd_bl = 9;

    const uint8_t sw1 = 41;
    const uint8_t sw2 = 40;
    
    const uint8_t gnd_det = 8;

    const uint8_t card_19 = 3;
    const uint8_t card_29 = 4;
    const uint8_t card_30 = 5;
    const uint8_t card_49 = 6;
    const uint8_t card_51 = 2;
    const uint8_t card_52 = 7;
}

void init();

static inline bool present() { return !digitalRead(pins::gnd_det); }

inline volatile uint8_t* const ddr[] = {
    [ports::PORT_A] = &DDRL,
    [ports::PORT_B] = &DDRC,
    [ports::PORT_C] = &DDRK,
    [ports::PORT_D] = &DDRA,
    [ports::PORT_E] = &DDRF,
    [ports::PORT_F] = &DDRB
};

inline const volatile uint8_t* const pin[] = {
    [ports::PORT_A] = &PINL,
    [ports::PORT_B] = &PINC,
    [ports::PORT_C] = &PINK,
    [ports::PORT_D] = &PINA,
    [ports::PORT_E] = &PINF,
    [ports::PORT_F] = &PINB
};

inline volatile uint8_t* const port[] = {
    [ports::PORT_A] = &PORTL,
    [ports::PORT_B] = &PORTC,
    [ports::PORT_C] = &PORTK,
    [ports::PORT_D] = &PORTA,
    [ports::PORT_E] = &PORTF,
    [ports::PORT_F] = &PORTB
};

static inline void portModeMask(ports p, uint8_t modeMask) {
    *ddr[p] = modeMask;
}

// Pullup not yet supported
static inline void portMode(ports p, uint8_t mode) {
    if(mode == OUTPUT) {
        portModeMask(p, 0xFF);
    } else {
        portModeMask(p, 0);
    }
}

static inline void portWrite(ports p, uint8_t val) {
    *port[p] = val;
}

static inline uint8_t portRead(ports p) {
    return *pin[p];
}

} // namespace tb

#endif