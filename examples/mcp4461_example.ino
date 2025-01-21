#include <Arduino.h>
#include <Wire.h>
#include <MCP4461.h>

// 0x2C A0 and A1 to VSS)
// 0x2D (A0 floating/VDD, A1 VSS)
// 0x2E (A0 VSS, A1 floating/VDD)
// 0x2F (both address pins floating/VDD)
uint8_t i2c_address = 0x2C;

MCP4461 digipot(i2c_address);

void setup() {
  Serial.begin(115200);
  Wire.begin();
  digipot.begin(Wire);
}

void loop() {
  //get current value of wiper {0-3}, 2nd param is nonvolatile=false per default, set to true to get nonvolatile wiper value
  uint16_t wiper0Val;
  wiper0Val = digipot.getWiper(0, true);

  // max value (full scale) value example
  uint16_t wiper0Val = 0x100 // 257 in dec
  
  //set current value of wiper {0-3}, 2nd param is 9-bits(!) with max value of 257 (257 taps available per wiper), 3rd param is nonvolatile=false per default, set to true to get nonvolatile wiper value
  digipot.setWiper(0, wiper0Val, true);

  //set all wipers to same value
  digipot.setWipers(wiper0Val, true)

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
