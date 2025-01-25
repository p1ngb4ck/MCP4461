#include <Arduino.h>
#include <Wire.h>
#include <MCP4461.h>

// 0x2C A0 and A1 to VSS)
// 0x2D (A0 floating/VDD, A1 VSS)
// 0x2E (A0 VSS, A1 floating/VDD)
// 0x2F (both address pins floating/VDD)
uint8_t i2c_address = 0x2C;

MCP4461 digipot(i2c_address);
uint16_t status_register;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  digipot.begin(Wire);
}

void loop() {
  //get && understand status register =)
  status_register = digipot.getStatus();
  Serial.println("Status-Register: ");
  //(1) means, bit is locked to value 1
  // Bit 0 is WP status (=>pin)
  // Bit 1 is named "R-1"-pin in datasheet an declared "reserved" and forced to 1
  // Bit 2 is WiperLock-Status resistor-network 0
  // Bit 3 is WiperLock-Status resistor-network 1
  // Bit 4 is EEPROM-Write-Active-Status bit
  // Bit 5 is WiperLock-Status resistor-network 2
  // Bit 6 is WiperLock-Status resistor-network 3
  // Bit 7+8 are referenced in datasheet as D7 + D8 and both locked to 1
  // Default status register reading should be 0x182h or 386 decimal
  // "Default" means  without any WiperLocks or WriteProtection enabled and EEPRom not active writing
  uint8_t bitNum = 9;
  Serial.println("D8(1) : D7(1) : WL3 : WL2 : EEWA : WL1 : WL0 : R-1(1) : WP");
  while(bitNum >= 1) {
    bool b = bitRead(status_register, (bitNum - 1));
    Serial.print(b);
    if(bitNum > 1) Serial.print(" : ");
    bitNum--;
  }
  Serial.println("");
  Serial.print("As Int: ");
  Serial.println(status_register);
  
  //get current value of wiper {0-3}, 2nd param is nonvolatile=false per default, set to true to get nonvolatile wiper value
  uint16_t wiper0Val;
  wiper0Val = digipot.getWiper(0, true);

  // max value (full scale) value example
  uint16_t wiper0Val = 0x100 // 257 in dec
  
  //set current value of wiper {0-3}, 2nd param is 9-bits(!) with max value of 257 (257 taps available per wiper), 3rd param is nonvolatile=false per default, set to true to get nonvolatile wiper value
  digipot.setWiper(0, wiper0Val, true);

  //set all wipers to same value
  digipot.setWipers(wiper0Val, true);

  //increment wiper 0 - increment/decrement is allowed for volatile wiper only !
  digipot.incrementWiper(0);

  //decrement wiper 0 - increment/decrement is allowed for volatile wiper only !
  digipot.decrementWiper(0);

  //increment all wipers - increment/decrement is allowed for volatile wiper only !
  digipot.incrementWipers();

  //decrement all wipers - increment/decrement is allowed for volatile wiper only !
  digipot.decrementWipers();
  
  // get 1byte status register - see datasheet
  uint8_t statusByte = digipot.getStatus();
  printLSBByteAsMSB(statusByte);

  // get 1byte terminal-status registers {0,1} - see datasheet
  uint8_t terminal0StatusByte;
  uint8_t terminal1StatusByte;
  terminal0StatusByte = digipot.getTerminalRegister(0);
  terminal1StatusByte = digipot.getTerminalRegister(1);
  printLSBByteAsMSB(terminal0StatusByte);
  printLSBByteAsMSB(terminal1StatusByte);

  //change specific bit of result, then set
  uint8_t bitNum = digipot.getTerminalStateBitNumber(0, 'A');
  bitSet(terminal0StatusBye, bitNum);
  digipot.setTerminalRegister(0, terminal0StatusByte);

  // connect wiper {0-3} to terminal (using HW-Pin)
  digipot.connectWiper(0);

  // disconnect wiper {0-3} from terminal (using HW-Pin)
  digipot.disconnectWiper(0);

  // toggle connection of wiper to terminal
  digipot.toggleWiper(0);

  // get specific terminal A state of wiper 0 ( {0-3}, {'A','W','B','H'} )
  bool terminalStateA = (bool)digipot.getTerminalState(0, 'A');

  // set terminal A state of wiper 0 to disconnected ( {0-3}, {'A','W','B','H'}, {0, 1} )
  bool terminalStateA = (bool)digipot.getTerminalState(0, 'A', 0);

  //MCP4461 provides 5x 9bits of eeprom to use freely as eeprom-storage
  // can be used eg. as single bool (bit) + 1byte, or as 9-bits int (max 511)
  // param is location index {0-4} (5x 9 bits)
  uint16_t eepromData1;
  eeepromData1 = digipot.getEEPRomGeneralPurposeData(0);

  // set first eeprom data location to max int value
  eepromData1 = 511;
  digipot.setEEPRomGeneralPurposeData(0, eepromData1);
}

void printLSBByteAsMSB(byte inByte)
{
  for (int b = 7; b >= 0; b--)
  {
    Serial.print(bitRead(inByte, b));
  }
}
