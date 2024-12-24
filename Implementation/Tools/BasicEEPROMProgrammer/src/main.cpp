#include <Arduino.h>

const uint32_t P_WLED = PA8;
const uint32_t P_RLED = PC13;

const uint32_t P_OE = PA10;
const uint32_t P_WE = PA9;

const size_t addr_width = 15;
const uint32_t addr_pins[] = {
    PB0,
    PB1,
    PB2,
    PB3,
    PB4,
    PB5,
    PB6,
    PB7,
    PB8,
    PB9,
    PB10,
    PB12,
    PB13,
    PB14,
    PB15
};
static_assert(addr_width == sizeof(addr_pins)/sizeof(addr_pins[0]));

const uint32_t data_pins[] = {
    PA0,
    PA1,
    PA2,
    PA3,
    PA4,
    PA5,
    PA6,
    PA7
};
static_assert(8 == sizeof(data_pins)/sizeof(data_pins[0]));

void setReadMode() {
    for(size_t i = 0; i < 8; i++) {
        pinMode(data_pins[i], INPUT);
    }
    digitalWrite(P_OE, LOW); // active low
}

void setWriteMode() {
    digitalWrite(P_OE, HIGH);
    for(size_t i = 0; i < 8; i++) {
        pinMode(data_pins[i], OUTPUT);
    }
}

void seekAddr(size_t addr) {
    for(size_t i = 0; i < addr_width; i++) {
        digitalWrite(addr_pins[i], (addr & (1 << i)) ? HIGH : LOW);
    }
}

// Assumes write mode already set
void writeROM(size_t addr, uint8_t data) {
    seekAddr(addr);
    for(size_t i = 0; i < 8; i++) {
        digitalWrite(data_pins[i], (data & (1 << i)) ? HIGH : LOW);
    }
    digitalWrite(P_WE, LOW);
    delayMicroseconds(1); // only need 100ns
    digitalWrite(P_WE, HIGH);
    delay(10); // maximum expected time - I'm too lazy to do this the right way
}

void writeROM64(size_t baseaddr, const uint8_t* buf) {
    for(size_t i = 0; i < 64; i++) {
        seekAddr(baseaddr + i);
        for(size_t j = 0; j < 8; j++) {
            digitalWrite(data_pins[j], (buf[i] & (1 << j)) ? HIGH : LOW);
        }
        digitalWrite(P_WE, LOW);
        delayMicroseconds(1); // only need 100ns
        digitalWrite(P_WE, HIGH);
        delayMicroseconds(1);
    }
    delay(10); // maximum expected time - I'm too lazy to do this the right way
}

// Assumes read mode already set
uint8_t readROM(size_t addr) {
    seekAddr(addr);
    delayMicroseconds(1);
    uint8_t data = 0;
    for(size_t i = 0; i < 8; i++) {
        data |= (digitalRead(data_pins[i]) ? 1 : 0) << i;
    }
    return data;
}

void setup() {
    for(size_t i = 0; i < addr_width; i++) {
        pinMode(addr_pins[i], OUTPUT);
    }

    digitalWrite(P_OE, HIGH);
    pinMode(P_OE, OUTPUT);
    digitalWrite(P_WE, HIGH);
    pinMode(P_WE, OUTPUT);

    pinMode(P_WLED, OUTPUT);
    digitalWrite(P_WLED, HIGH);
    pinMode(P_RLED, OUTPUT);
    digitalWrite(P_RLED, LOW);


    Serial.begin(4000000);
    while(!Serial);

    digitalWrite(P_WLED, LOW);
    digitalWrite(P_RLED, HIGH);
}

void loop() {
    setWriteMode();

    for(size_t i = 0; i < 1 << addr_width; i += 64) {
        digitalWrite(P_WLED, HIGH);
        uint8_t buf[64];
        for(size_t j = 0; j < 64; j++) {
            while(!Serial.available());
            buf[j] = Serial.read();
        }
        Serial.write('A'); // acknowledge
        digitalWrite(P_WLED, LOW);
        // Serial.println(i);
        // Serial.write(buf, 64);
        // Serial.println();
        writeROM64(i, buf);
    }

    setReadMode();
    digitalWrite(P_RLED, LOW);
    for(size_t i = 0; i < 1 << addr_width; i++) {
        Serial.write(readROM(i));
    }
    digitalWrite(P_RLED, HIGH);
}