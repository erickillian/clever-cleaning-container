
#include <EEPROM.h>

#define SELECTED_MOTOR_ADDR   0
#define AMOUNTS_ADDR          8

void setup() {
  Serial.begin(9600);
  writeIntIntoEEPROM(SELECTED_MOTOR_ADDR, 500);
  Serial.println(readIntFromEEPROM(SELECTED_MOTOR_ADDR));
//  Serial.println(EEPROM.read(SELECTED_MOTOR_ADDR+1));
//  Serial.println(EEPROM.read(SELECTED_MOTOR_ADDR+2));
}

void loop() {
}



void writeIntIntoEEPROM(int address, unsigned int number) { 
  EEPROM.write(address, (byte)(number >> 8));
  EEPROM.write(address + 1, (byte)((number & 0xFF)));
}

unsigned int readIntFromEEPROM(int address) {
  unsigned int byte1 = EEPROM.read(address);
  unsigned int byte2 = EEPROM.read(address + 1);
  return ((byte1 << 8) + byte2);
}
