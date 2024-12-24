#include "test_board.h"

namespace tb {

LiquidCrystal lcd(pins::lcd_rs, pins::lcd_e,
                  pins::lcd_d[4],
                  pins::lcd_d[5],
                  pins::lcd_d[6],
                  pins::lcd_d[7]);

void init() {
    pinMode(pins::lcd_bl, OUTPUT);
    digitalWrite(pins::lcd_bl, HIGH);
    lcd.begin(16, 2);

    pinMode(pins::gnd_det, INPUT);
}

} // namespace tb