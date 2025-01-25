#ifndef MCP4461_H
#define MCP4461_H

#include <inttypes.h>
#include <Wire.h>

#define DEFAULT_WIPER_VALUE 0x80  //Default to the wipers in midrange

//memory addresses (all shifted 4 bits left)
//For all the Wipers 0x100 = Full scale, 0x80 = mid scale, 0x0 = Zero scale
#define MCP4461_VW0 0x00
#define MCP4461_VW1 0x10
#define MCP4461_VW2 0x60
#define MCP4461_VW3 0x70
//STATUS: D8:D7: Reserved D6: WL3 D5:WL2 D4:EEWA D3:WL1 D2:WL0 D1:Reserved D0: WP
#define MCP4461_STATUS 0x50
//TCON0: D8: Reserved D7:R1HW D6: R1A D5:R1W D4:R1B D3:R0HW D2:R0A D1:R0W D0: R0B
#define MCP4461_TCON0 0x40
//TCON1: D8: Reserved D7:R3HW D6: R3A D5:R3W D4:R3B D3:R2HW D2:R2A D1:R2W D0: R2B
#define MCP4461_TCON1 0xA0
#define MCP4461_EEPROM_DATA_ADDRESS_1 0xB0

//control commands
#define MCP4461_WRITE 0x0
#define MCP4461_INCREMENT 0x4 //01 left shift by 2
#define MCP4461_DECREMENT 0x8 //10 left shift by 2
#define MCP4461_READ 0xC //11 left shift by 2

class MCP4461{
public:
  MCP4461(uint8_t i2c_address);
  void begin(TwoWire &wire);
  void setMCP4461Address(uint8_t mcp4461_addr);
  uint16_t getWiper(uint8_t wiper, bool nonvolatile = false);
  void setWiper(uint8_t wiper, uint16_t wiper_value, bool nonvolatile = false);
  void setWipers(uint16_t wiper_value, bool nonvolatile = false);
  void incrementWiper(uint8_t wiper);
  void incrementWipers();
  void decrementWiper(uint8_t wiper);
  void decrementWipers();
  uint16_t getStatus();
  bool getEEPRomWriteActive();
  uint8_t getTerminalRegister(uint8_t reg);
  void setTerminalRegister(uint8_t reg, uint8_t value);
  uint8_t getTerminalState(uint8_t wiper, char terminal);
  void setTerminalState(uint8_t wiper, char terminal, uint8_t state);
  void connectWiper(uint8_t wiper);
  void disconnectWiper(uint8_t wiper);
  void toggleWiper(uint8_t wiper);
  uint16_t getEEPRomGeneralPurposeData(uint8_t location);
  void setEEPRomGeneralPurposeData(uint8_t location, uint16_t value);

private:
  uint16_t readAddress(uint8_t address);
  void writeValue(uint8_t addressByte, uint16_t data);
  void increment(uint8_t addressByte);
  void decrement(uint8_t addressByte);
  uint8_t getAddressByteForWiper(uint8_t wiper, bool nonvolatile = false);
  uint8_t getTerminalStateBitNumber(uint8_t wiper, char terminal);
  TwoWire *_wire_bus;
  uint8_t _mcp4461_address;
  uint8_t _wiper;
  uint8_t _value;
};

#endif //MCP4461_H
