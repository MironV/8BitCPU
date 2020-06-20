// Arduino 2560 Programmer for 8-Bit CPU

const char DATA_LINES[] = {38, 40, 42, 44, 46, 48, 50, 52};
const char ADDRESS_LINES[] = {39, 41, 43, 45, 47, 49, 51, 53};
#define RAM_WRITE 3

#define NOP 0x00
#define LDA 0x01
#define ADD 0x02
#define SUB 0x03
#define STA 0x04
#define LDI 0x05
#define JMP 0x06
#define JC  0x07
#define JZ  0x08
#define ADI 0x09
#define SUI 0x0a
#define PSH 0x0b
#define POP 0x0c
#define OUT 0x0e
#define HLT 0x0f

const unsigned char vars[] = {
 // 0xf0, 1
};

const unsigned char data[] = {
  /* 0x00 */ 0x05, 0x05, 0x12, 0x0c, 0x0e, 0x12, 0x0f, 0x0e, 0x12, 0x0c, 0x0e, 0x0f, 0x09, 0x05, 0x13, 0x0a, 
  /* 0x10 */ 0x02, 0x13
};

void enableWritingData() {
  for(int n = 0; n < 8; n += 1) {
    pinMode(DATA_LINES[n], OUTPUT);
  }
}

void enableWritingAddress() {
  for(int n = 0; n < 8; n += 1) {
    pinMode(ADDRESS_LINES[n], OUTPUT);
  }
}

void disableWritingAddress() {
  for(int n = 0; n < 8; n += 1) {
    pinMode(ADDRESS_LINES[n], INPUT);
  }
}

void disableWritingData() {  
  for(int n = 0; n < 8; n += 1) {
    pinMode(DATA_LINES[n], INPUT);
  }
}

void writeData(byte writeAddress, byte writeData) {
  // Set address register
  Serial.print("Addr: ");
  Serial.print(writeAddress, HEX);
  for (int n = 0; n < 8; n += 1) {
      digitalWrite(ADDRESS_LINES[n], (writeAddress & 0x01) ? HIGH : LOW);
      writeAddress = writeAddress >> 1;
  }

  // Write data
  Serial.print(" Data: ");
  Serial.print(writeData, HEX);
  Serial.println();
  for (int n = 0; n < 8; n += 1) {
      digitalWrite(DATA_LINES[n], (writeData & 0x01) ? HIGH : LOW);
      writeData = writeData >> 1;
  }

  digitalWrite(RAM_WRITE, LOW);
  delay(1);
  digitalWrite(RAM_WRITE, HIGH);
  delay(1);
}

void readData(byte readAddress) {
  enableWritingAddress();

  for (int n = 0; n < 8; n += 1) {
      digitalWrite(ADDRESS_LINES[n], (readAddress & 0x01) ? HIGH : LOW);
      readAddress = readAddress >> 1;
  }

  delay(1000);

  disableWritingAddress();
}

void loadProgram() {
   enableWritingAddress();
   enableWritingData();
  
  // Load the data
  Serial.println("Loading data...");
  for (byte i = 0; i < sizeof(vars); i += 2) {
    writeData(vars[i], vars[i+1]);
  }
  
  // Load the program
  Serial.println("Loading program...");  
  for (byte i = 0; i < sizeof(data); i += 1) {
    writeData(i, data[i]);
  }

  disableWritingData();
  disableWritingAddress();
}

String splitString(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void setup() {
  Serial.begin(57600);

  pinMode(RAM_WRITE, OUTPUT);

  loadProgram();
  
  Serial.println("READY");
}

void loop() {
   if (Serial.available() > 0) {
    String command = "";
    command = Serial.readString();
    Serial.print(command);

    if(command.startsWith("r")) {
      String hexAddr = splitString(command, ' ', 1);
      int addr = strtol(hexAddr.c_str(), NULL, 16);
      
      readData(addr);
    } else if (command.startsWith("w")) {
      String hexAddr = splitString(command, ' ', 1);
      int addr = strtol(hexAddr.c_str(), NULL, 16);
      
      int inputData = splitString(command, ' ', 2).toInt();

      enableWritingAddress();
      enableWritingData();
      
      writeData(addr, inputData);
      
      disableWritingData();
      disableWritingAddress();
    } else if (command.startsWith("p")) {
      loadProgram();
    } else {
      Serial.println("Unknown command!");
    }
  }
}
