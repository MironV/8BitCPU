// Arduino ROM Programmer (extension of Ben Eater's programmer)

#define SHIFT_DATA 2
#define SHIFT_CLOCK 3
#define SHIFT_LATCH 4

#define EEPROM_D0 5
#define EEPROM_D7 12

#define WRITE_EN 13

#define MI  0x0100
#define RI  0x0200
#define II  0x0300
#define AI  0x0400
#define BI  0x0500
#define J   0x0600
#define SI  0x0700

#define RO  0x0001
#define CO  0x0002
#define AO  0x0003
#define EO  0x0004
#define SO  0x0005
#define BO  0x0006

#define SA  0b1000000000000000
#define SM  0b0100000000000000
#define SP  0b0010000000000000

#define HLT 0b0001000000000000
#define TR  0b0000100000000000

#define CE  0b0000000001000000
#define SU  0b0000000000100000
#define FI  0b0000000000010000
#define OI  0b0000000000001000

#define FLAGS_Z0C0 0
#define FLAGS_Z0C1 1
#define FLAGS_Z1C0 2
#define FLAGS_Z1C1 3

#define JC 0b0111
#define JZ 0b1000

#define CHIPNUM 1

const PROGMEM uint16_t UCODE_TEMPLATE[16][8] = {
  { MI|CO,  RO|II|CE,  TR,        0,          0,            0,             0,   0 },          // 0000 - NOP
  { MI|CO,  RO|II|CE,  MI|CO,     MI|RO|CE,     RO|AI,        TR,          0,   0 },          // 0001 - LDA
  { MI|CO,  RO|II|CE,  MI|CO,     MI|RO|CE,     RO|BI,        EO|AI|FI,    TR,  0 },         // 0010 - ADD
  { MI|CO,  RO|II|CE,  MI|CO,     MI|RO|CE,     RO|BI,        EO|AI|SU|FI, TR,  0 },         // 0011 - SUB
  { MI|CO,  RO|II|CE,  MI|CO,     MI|RO|CE,     AO|RI,        TR,          0,   0 },         // 0100 - STA
  { MI|CO,  RO|II|CE,  MI|CO,     RO|AI|CE,     TR,           0,           0,   0 },          // 0101 - LDI
  { MI|CO,  RO|II|CE,  MI|CO,     RO|J,         TR,           0,           0,   0 },          // 0110 - JMP
  { MI|CO,  RO|II|CE,  CE,        TR,           0,            0,           0,   0 },          // 0111 - JC
  { MI|CO,  RO|II|CE,  CE,        TR,           0,            0,           0,   0 },          // 1000 - JZ
  { MI|CO,  RO|II|CE,  MI|CO,     RO|BI|CE,     EO|AI|FI,     TR,          0,   0 },          // 1001 - ADI
  { MI|CO,  RO|II|CE,  MI|CO,     RO|BI|CE,     EO|AI|SU|FI,  TR,          0,   0 },          // 1010 - SUI
  { MI|CO,  RO|II|CE,  MI|SO|SA,  AO|RI|SP|SA,  TR,           0,           0,   0 },          // 1011 - PSH
  { MI|CO,  RO|II|CE,  SM,        MI|SO|SA,     RO|AI|SA,     TR,          0,   0 },          // 1100 - POP
  { MI|CO,  RO|II|CE,  MI|SO|SA,  CO|RI|SP|SA,  MI|CO,        RO|J,        TR,  0 },          // 1101 - JSR
  { MI|CO,  RO|II|CE,  AO|OI,     TR,           0,            0,           0,   0 },          // 1110 - OUT
  { MI|CO,  RO|II|CE,  HLT,       0,            0,            0,           0,   0 },          // 1111 - HLT   
};

uint16_t ucode[4][16][8];

void initUCode() {
  // ZF = 0, CF = 0
  memcpy_P(ucode[FLAGS_Z0C0], UCODE_TEMPLATE, sizeof(UCODE_TEMPLATE));

  // ZF = 0, CF = 1
  memcpy_P(ucode[FLAGS_Z0C1], UCODE_TEMPLATE, sizeof(UCODE_TEMPLATE));
  ucode[FLAGS_Z0C1][JC][2] = MI|CO;
  ucode[FLAGS_Z0C1][JC][3] = RO|J;
  ucode[FLAGS_Z0C1][JC][4] = TR;

  // ZF = 1, CF = 0
  memcpy_P(ucode[FLAGS_Z1C0], UCODE_TEMPLATE, sizeof(UCODE_TEMPLATE));
  ucode[FLAGS_Z1C0][JZ][2] = MI|CO;
  ucode[FLAGS_Z1C0][JZ][3] = RO|J;
  ucode[FLAGS_Z1C0][JZ][4] = TR;

  // ZF = 1, CF = 1
  memcpy_P(ucode[FLAGS_Z1C1], UCODE_TEMPLATE, sizeof(UCODE_TEMPLATE));
  ucode[FLAGS_Z1C1][JC][2] = MI|CO;
  ucode[FLAGS_Z1C1][JC][3] = RO|J;
  ucode[FLAGS_Z1C1][JC][4] = TR;
  ucode[FLAGS_Z1C1][JZ][2] = MI|CO;
  ucode[FLAGS_Z1C1][JZ][3] = RO|J;
  ucode[FLAGS_Z1C1][JZ][4] = TR;
}

void setAddress(int address, bool outputEnable) {
  shiftOut(SHIFT_DATA, SHIFT_CLOCK, MSBFIRST, (address >> 8) | (outputEnable ? 0x00 : 0x80));
  shiftOut(SHIFT_DATA, SHIFT_CLOCK, MSBFIRST, address);

  digitalWrite(SHIFT_LATCH, LOW);
  digitalWrite(SHIFT_LATCH, HIGH);
  digitalWrite(SHIFT_LATCH, LOW);
}

byte readEEPROM(int address) {
  for(int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    pinMode(pin, INPUT);
  }
  
  setAddress(address, true);

  byte data = 0;
  for(int pin = EEPROM_D7; pin >= EEPROM_D0; pin -= 1) {
    data = (data << 1) + digitalRead(pin);
  }

  return data;
}

void writeEEPROM(int address, byte data){
  for(int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    pinMode(pin, OUTPUT);
  }
  
  setAddress(address, false);

  for(int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    digitalWrite(pin, data & 1);
    data = data >> 1;
  }

  digitalWrite(WRITE_EN, LOW);
  delayMicroseconds(1);
  digitalWrite(WRITE_EN, HIGH);
  delay(5);
}

void printContents(int start, int length) {
  for (int base = start; base < length; base += 16) {
    byte data[16];
    for (int offset = 0; offset <= 15; offset += 1) {
      data[offset] = readEEPROM(base + offset);
    }

    char buf[80];
    sprintf(buf, "%03x: %02x %02x %02x %02x %02x %02x %02x %02x    %02x %02x %02x %02x %02x %02x %02x %02x",
    base, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8],
    data[9], data[10], data[11], data[12], data[13], data[14], data[15]);

    Serial.println(buf);
  }
}

void setup() {
  initUCode();
  
  // put your setup code here, to run once:
  pinMode(SHIFT_DATA, OUTPUT);
  pinMode(SHIFT_CLOCK, OUTPUT);
  pinMode(SHIFT_LATCH, OUTPUT);

  digitalWrite(WRITE_EN, HIGH);
  pinMode(WRITE_EN, OUTPUT);

  Serial.begin(57600);

  Serial.println();

  Serial.print("Erasing EEPROM");
  for (int base = 0; base <= 255; base += 16) {
    Serial.print(".");
    for (int offset = 0; offset <= 15; offset += 1) {
      writeEEPROM(base + offset, 0x00);
    }
  }
  Serial.println();

  Serial.print("Writing EEPROM");
  for (int address = 0; address < 1024; address += 1) {
    int flags = (address & 0b1100000000) >> 8;
    int byte_sel = (address & 0b0010000000) >> 7;
    int instruction = (address & 0b0001111000) >> 3;
    int step =  (address & 0b0000000111);

    writeEEPROM(address, ucode[flags][instruction][step] >> (8 * CHIPNUM));

    if(address % 64 == 0) {
      Serial.print(".");
    }
  }
  Serial.println(" DONE");

  printContents(0, 1024);
}

void loop() {

  // put your main code here, to run repeatedly:
}
