const uint8_t ABUS_PINS[] = {A2, A1, A0};
const uint8_t DBUS_PINS[] = {9, 8, 7, 6, 5, 4, 3, 2};
const uint8_t P_IOW = 10;
const uint8_t P_IOR = 11;
const uint8_t P_CS0 = 12;
const uint8_t P_RST = A3;
const uint8_t P_LED = A4;

void write_reg(uint8_t addr, uint8_t value) {
  for(uint8_t i = 0; i < 3; i++) digitalWrite(ABUS_PINS[i], (addr & (1 << i)) ? HIGH : LOW);
  for(uint8_t i = 0; i < 8; i++) {
    pinMode(DBUS_PINS[i], OUTPUT);
    digitalWrite(DBUS_PINS[i], (value & (1 << i)) ? HIGH : LOW);
  }
  digitalWrite(P_CS0, LOW);
  delayMicroseconds(1);
  digitalWrite(P_IOW, LOW);
  delayMicroseconds(1);
  digitalWrite(P_IOW, HIGH);
  delayMicroseconds(1);
  digitalWrite(P_CS0, HIGH);
  for(uint8_t i = 0; i < 8; i++) pinMode(DBUS_PINS[i], INPUT_PULLUP);
}

uint8_t read_reg(uint8_t addr) {
  uint8_t result = 0;
  for(uint8_t i = 0; i < 3; i++) digitalWrite(ABUS_PINS[i], (addr & (1 << i)) ? HIGH : LOW);
  digitalWrite(P_CS0, LOW);
  delayMicroseconds(1);
  digitalWrite(P_IOR, LOW);
  delayMicroseconds(1);
  for(uint8_t i = 0; i < 8; i++) if(digitalRead(DBUS_PINS[i])) result |= 1 << i;
  digitalWrite(P_IOR, HIGH);
  delayMicroseconds(1);
  digitalWrite(P_CS0, HIGH);
  return result;
}

void setup() {
  for(uint8_t i = 0; i < 3; i++) pinMode(ABUS_PINS[i], OUTPUT);
  for(uint8_t i = 0; i < 8; i++) pinMode(DBUS_PINS[i], INPUT_PULLUP);

  pinMode(P_IOW, OUTPUT);
  digitalWrite(P_IOW, HIGH);
  pinMode(P_IOR, OUTPUT);
  digitalWrite(P_IOR, HIGH);
  pinMode(P_CS0, OUTPUT);
  digitalWrite(P_CS0, HIGH);
  pinMode(P_LED, OUTPUT);
  digitalWrite(P_LED, HIGH);
  pinMode(P_RST, OUTPUT);
  digitalWrite(P_RST, LOW);
  delayMicroseconds(1);
  digitalWrite(P_RST, HIGH);

  Serial.begin(115200);
  Serial.write(0xF0);
}

void loop() {
  while(!Serial.available());
  uint8_t command = Serial.read();
  switch(command & 0xF0) {
    uint8_t data;
    case 0x00:
      data = read_reg(command & 0x7);
      Serial.write(data);
      break;
    case 0x80:
      while(!Serial.available());
      data = Serial.read();
      write_reg(command & 0x7, data);
    default:
      break;
  }
}
