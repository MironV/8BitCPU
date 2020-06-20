// ROM Programmer for 8-Bit CPU

#include <stdio.h>
#include <string.h>

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

#define JC 0b00111
#define JZ 0b01000

const unsigned short UCODE_TEMPLATE[32][8] = {
  { MI|CO,  RO|II|CE,  TR,        0,          0,            0,             0,   0 },          // 00000 - NOP
  { MI|CO,  RO|II|CE,  MI|CO,     MI|RO|CE,     RO|AI,        TR,          0,   0 },          // 00001 - LDA
  { MI|CO,  RO|II|CE,  MI|CO,     MI|RO|CE,     RO|BI,        EO|AI|FI,    TR,  0 },          // 00010 - ADD
  { MI|CO,  RO|II|CE,  MI|CO,     MI|RO|CE,     RO|BI,        EO|AI|SU|FI, TR,  0 },          // 00011 - SUB
  { MI|CO,  RO|II|CE,  MI|CO,     MI|RO|CE,     AO|RI,        TR,          0,   0 },          // 00100 - STA
  { MI|CO,  RO|II|CE,  MI|CO,     RO|AI|CE,     TR,           0,           0,   0 },          // 00101 - LDI
  { MI|CO,  RO|II|CE,  MI|CO,     RO|J,         TR,           0,           0,   0 },          // 00110 - JMP
  { MI|CO,  RO|II|CE,  CE,        TR,           0,            0,           0,   0 },          // 00111 - JC
  { MI|CO,  RO|II|CE,  CE,        TR,           0,            0,           0,   0 },          // 01000 - JZ
  { MI|CO,  RO|II|CE,  MI|CO,     RO|BI|CE,     EO|AI|FI,     TR,          0,   0 },          // 01001 - ADI
  { MI|CO,  RO|II|CE,  MI|CO,     RO|BI|CE,     EO|AI|SU|FI,  TR,          0,   0 },          // 01010 - SUI
  { MI|CO,  RO|II|CE,  TR,        0,            0,            0,           0,   0 },          // 01011 - NOP
  { MI|CO,  RO|II|CE,  TR,        0,            0,            0,           0,   0 },          // 01100 - NOP   
  { MI|CO,  RO|II|CE,  TR,        0,            0,            0,           0,   0 },          // 01101 - NOP  
  { MI|CO,  RO|II|CE,  AO|OI,     TR,           0,            0,           0,   0 },          // 01110 - OUT
  { MI|CO,  RO|II|CE,  HLT,       0,            0,            0,           0,   0 },          // 01111 - HLT 
  { MI|CO,  RO|II|CE,  MI|SO|SA,  AO|RI|SP|SA,  TR,           0,           0,   0 },          // 10000 - PSH
  { MI|CO,  RO|II|CE,  SM,        MI|SO|SA,     RO|AI|SA,     TR,          0,   0 },          // 10001 - POP
  { MI|CO,  RO|II|CE,  MI|SO|SA,  CO|RI|SP|SA,  MI|CO,        RO|J,        TR,  0 },          // 10010 - JSR 
  { MI|CO,  RO|II|CE,  SM,        MI|SO|SA,     RO|J|SA,      CE,          TR,  0 },          // 10011 - RTS   
  { MI|CO,  RO|II|CE,  TR,        0,            0,            0,           0,   0 },          // 10100 - NOP   
  { MI|CO,  RO|II|CE,  TR,        0,            0,            0,           0,   0 },          // 10101 - NOP   
  { MI|CO,  RO|II|CE,  TR,        0,            0,            0,           0,   0 },          // 10110 - NOP   
  { MI|CO,  RO|II|CE,  TR,        0,            0,            0,           0,   0 },          // 10111 - NOP   
  { MI|CO,  RO|II|CE,  TR,        0,            0,            0,           0,   0 },          // 11000 - NOP   
  { MI|CO,  RO|II|CE,  TR,        0,            0,            0,           0,   0 },          // 11001 - NOP   
  { MI|CO,  RO|II|CE,  TR,        0,            0,            0,           0,   0 },          // 11010 - NOP   
  { MI|CO,  RO|II|CE,  TR,        0,            0,            0,           0,   0 },          // 11011 - NOP   
  { MI|CO,  RO|II|CE,  TR,        0,            0,            0,           0,   0 },          // 11100 - NOP   
  { MI|CO,  RO|II|CE,  TR,        0,            0,            0,           0,   0 },          // 11101 - NOP   
  { MI|CO,  RO|II|CE,  TR,        0,            0,            0,           0,   0 },          // 11110 - NOP   
  { MI|CO,  RO|II|CE,  TR,        0,            0,            0,           0,   0 },          // 11111 - NOP        
};

unsigned short ucode[4][32][8];

void initUCode() {
  // ZF = 0, CF = 0
  memcpy(ucode[FLAGS_Z0C0], UCODE_TEMPLATE, sizeof(UCODE_TEMPLATE));

  // ZF = 0, CF = 1
  memcpy(ucode[FLAGS_Z0C1], UCODE_TEMPLATE, sizeof(UCODE_TEMPLATE));
  ucode[FLAGS_Z0C1][JC][2] = MI|CO;
  ucode[FLAGS_Z0C1][JC][3] = RO|J;
  ucode[FLAGS_Z0C1][JC][4] = TR;

  // ZF = 1, CF = 0
  memcpy(ucode[FLAGS_Z1C0], UCODE_TEMPLATE, sizeof(UCODE_TEMPLATE));
  ucode[FLAGS_Z1C0][JZ][2] = MI|CO;
  ucode[FLAGS_Z1C0][JZ][3] = RO|J;
  ucode[FLAGS_Z1C0][JZ][4] = TR;

  // ZF = 1, CF = 1
  memcpy(ucode[FLAGS_Z1C1], UCODE_TEMPLATE, sizeof(UCODE_TEMPLATE));
  ucode[FLAGS_Z1C1][JC][2] = MI|CO;
  ucode[FLAGS_Z1C1][JC][3] = RO|J;
  ucode[FLAGS_Z1C1][JC][4] = TR;
  ucode[FLAGS_Z1C1][JZ][2] = MI|CO;
  ucode[FLAGS_Z1C1][JZ][3] = RO|J;
  ucode[FLAGS_Z1C1][JZ][4] = TR;
}

int main (void)
{
  initUCode();

  char ucode_upper[2048];
  char ucode_lower[2048];

  for (int address = 0; address < 2048; address++) {
    int flags = (address & 0b01100000000) >> 8;
    int instruction = (address & 0b00011111000) >> 3;
    int step =  (address & 0b00000000111);

    ucode_lower[address] = ucode[flags][instruction][step];
    ucode_upper[address] = ucode[flags][instruction][step] >> 8;
  }

  FILE *f = fopen("rom_upper.hex", "wb");
  fwrite(ucode_upper, sizeof(char), sizeof(ucode_upper), f);
  fclose(f);

  f = fopen("rom_lower.hex", "wb");
  fwrite(ucode_lower, sizeof(char), sizeof(ucode_lower), f);
  fclose(f);

  return 0;
}