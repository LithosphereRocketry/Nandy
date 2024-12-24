#include <test_board.h>

static const uint8_t pin_sel1 = tb::pins::card_19;
static const uint8_t pin_sel2 = tb::pins::card_49;

bool prev;
void setup() {
  tb::init();
  tb::portMode(tb::ports::PORT_A, OUTPUT);
  tb::portMode(tb::ports::PORT_B, OUTPUT);
  pinMode(pin_sel1, OUTPUT);
  tb::portMode(tb::ports::PORT_C, INPUT);
  tb::portMode(tb::ports::PORT_D, OUTPUT);
  tb::portMode(tb::ports::PORT_E, OUTPUT);
  pinMode(pin_sel2, OUTPUT);
  tb::portMode(tb::ports::PORT_F, INPUT);

  tb::lcd.clear();
  tb::lcd.print("Running test...");
}

void puthex(uint8_t val) {
  tb::lcd.print(val >> 4, HEX);
  tb::lcd.print(val & 0xF, HEX);
}

void puttime(uint32_t value_sec) {
  uint8_t sec = value_sec % 60;
  value_sec /= 60;
  uint8_t min = value_sec % 60;
  value_sec /= 60;
  uint8_t hr = value_sec % 60;
  
  tb::lcd.print(hr/10);
  tb::lcd.print(hr%10);
  tb::lcd.print(":");
  tb::lcd.print(min/10);
  tb::lcd.print(min%10);
  tb::lcd.print(":");
  tb::lcd.print(sec/10);
  tb::lcd.print(sec%10);
}


void putsci(uint32_t value) {
  // Uses decimal prefixes (e.g. k = 1000 not 1024)
  // No more prefixes needed for int32
  static const char prefixes[] = {' ', 'k', 'M', 'G'};

  const char* pf = prefixes;
  while(value > 1000) {
    pf++;
    value /= 1000;
  }
  tb::lcd.print(value);
  tb::lcd.print(*pf);
}

bool run = true;

uint32_t lastDisplay = -1000;

uint32_t count = 0;

// We don't care much about the quality of our RNG, so implementing an xorshift
// PRNG is significantly faster than using the built-in random() (up to roughly
// 16K fuzz inputs per second as compared to 7K without)
// It's conceivable this could be even faster by just producing 16-bit PRNG
// values, but this is good enough and theoretically hits a few more cases
uint32_t xsrand() {
  static uint32_t sr = 1;
	/* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
  // Further stolen from Wikipedia
	sr ^= sr << 13;
	sr ^= sr >> 17;
	sr ^= sr << 5;
	return sr;
}

void loop() {
  if(run) {
    long rand_ch1 = xsrand();
    bool sel_ch1 = (rand_ch1 >> 16) & 0x1;
    uint8_t a_ch1 = (rand_ch1 >> 8) & 0xFF;
    uint8_t b_ch1 = rand_ch1 & 0xFF;
    uint8_t test_ch1 = sel_ch1 ? b_ch1 : a_ch1;

    long rand_ch2 = xsrand();
    bool sel_ch2 = (rand_ch2 >> 16) & 0x1;
    uint8_t a_ch2 = (rand_ch2 >> 8) & 0xFF;
    uint8_t b_ch2 = rand_ch2 & 0xFF;
    uint8_t test_ch2 = sel_ch2 ? b_ch2 : a_ch2;

    tb::portWrite(tb::ports::PORT_A, a_ch1);
    tb::portWrite(tb::ports::PORT_B, b_ch1);
    digitalWrite(pin_sel1, sel_ch1);
    asm volatile("nop");
    uint8_t q_ch1 = tb::portRead(tb::ports::PORT_C);

    tb::portWrite(tb::ports::PORT_D, a_ch2);
    tb::portWrite(tb::ports::PORT_E, b_ch2);
    digitalWrite(pin_sel2, sel_ch2);
    asm volatile("nop");
    uint8_t q_ch2 = tb::portRead(tb::ports::PORT_F);
    
    count++;

    if(millis() - lastDisplay > 1000) {
      tb::lcd.setCursor(0, 1);
      puttime(millis()/1000);
      tb::lcd.print("  C:");
      putsci(count);
      tb::lcd.print("  "); // clear rest of line
      lastDisplay += 1000;
    }

    if(test_ch1 != q_ch1) {
      tb::lcd.clear();
      tb::lcd.print("Error on ch. 1!");
      tb::lcd.setCursor(0, 1);
      puthex(a_ch1);
      tb::lcd.print(sel_ch1 ? "->" : "<-");
      puthex(b_ch1);
      tb::lcd.print(" = ");
      puthex(q_ch1);
      tb::lcd.print(" (");
      puthex(test_ch1);
      tb::lcd.print(")");
      run = false;
    } else if(test_ch2 != q_ch2) {
      tb::lcd.clear();
      tb::lcd.print("Error on ch. 2!");
      tb::lcd.setCursor(0, 1);
      puthex(a_ch2);
      tb::lcd.print(sel_ch2 ? "->" : "<-");
      puthex(b_ch2);
      tb::lcd.print(" = ");
      puthex(q_ch2);
      tb::lcd.print(" (");
      puthex(test_ch2);
      tb::lcd.print(")");
      run = false;
    }
  }


}