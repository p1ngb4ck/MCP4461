#include "Arduino.h"
#include "MCP4461.h"
#include <stdio.h>
#include <Wire.h>

/*
Library to control the MCP4461 Digital Potentiometer over I2C.
http://ww1.microchip.com/downloads/en/DeviceDoc/22265a.pdf
This library implements all functionality required, except for HV cmds

MIT License

Copyright (c) 2025 Oliver Kleinecke

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

based on ideas from code by Tony@think3dprint3d.com
major/full code rewrite & extension by kleinecke.oliver@googlemail.com 2025
*/


MCP4461::MCP4461(uint8_t i2c_address) {
  _mcp4461_address = i2c_address;
}

void MCP4461::begin(TwoWire &wire) {
	_wire_bus = &wire;
}

void MCP4461::setMCP4461Address(uint8_t mcp4461_addr) {
	_mcp4461_address = mcp4461_addr;
}

uint16_t MCP4461::readAddress(uint8_t address) {
  uint16_t ret = 0;
  uint16_t cmdByte = 0;
  cmdByte |= address;
  cmdByte |= MCP4461_READ;
  _wire_bus->beginTransmission(_mcp4461_address);
  _wire_bus->write(cmdByte);
  _wire_bus->endTransmission(false);
  _wire_bus->requestFrom((uint8_t)_mcp4461_address,(uint8_t)2);
  int i = 0;
  while(_wire_bus->available()) 
  { 
    ret |= _wire_bus->read();
    if (i==0) ret = ret<<8;
    i++;
  }
  return ret;
}

void MCP4461::writeValue(uint8_t addressByte, uint16_t data) {
	uint8_t commandByte;
	uint8_t dataByte = (uint8_t)data;
	if (data > 0xFF) commandByte = 0x1;
	else commandByte = 0;
	commandByte |= addressByte;
	commandByte |= MCP4461_WRITE;
	_wire_bus->beginTransmission(_mcp4461_address);
	_wire_bus->write(commandByte);
	_wire_bus->write(dataByte);
	_wire_bus->endTransmission();
	while(getEEPRomWriteActive()) {
	  delayMicroseconds(1);
	}
}

void MCP4461::increment(uint8_t addressByte) {
	uint8_t commandByte = 0;
	commandByte |= addressByte;
	commandByte |= MCP4461_INCREMENT;
	_wire_bus->beginTransmission(_mcp4461_address);
	_wire_bus->write(commandByte);
	_wire_bus->endTransmission();
}

void MCP4461::decrement(uint8_t addressByte) {
	uint8_t commandByte = 0;
	commandByte |= addressByte;
	commandByte |= MCP4461_DECREMENT;
	_wire_bus->beginTransmission(_mcp4461_address);
	_wire_bus->write(commandByte);
	_wire_bus->endTransmission();
}

void MCP4461::decrementWiper(uint8_t wiper) {
	uint8_t addressByte;
	addressByte = getAddressByteForWiper(wiper);
	return decrement(addressByte);
}

void MCP4461::decrementWipers() {
  uint8_t i = 0;
  while (i < 4) {
	  decrementWiper(i);
	  i++;
  }
}

void MCP4461::incrementWiper(uint8_t wiper) {
	uint8_t addressByte;
	addressByte = getAddressByteForWiper(wiper);
	return increment(addressByte);
}

void MCP4461::incrementWipers() {
  uint8_t i = 0;
  while (i < 4) {
	  incrementWiper(i);
	  i++;
  }
}

uint16_t MCP4461::getWiper(uint8_t wiper, bool nonvolatile = false) {
	uint8_t addressByte;
	addressByte = getAddressByteForWiper(wiper, nonvolatile);
	return readAddress(addressByte);
}

void MCP4461::setWiper(uint8_t wiper, uint16_t wiper_value, bool nonvolatile = false){
  if (wiper_value > 257) return; //max 257 taps allowed
  uint8_t dataByte = (uint8_t)wiper_value;
  uint8_t addressByte;
  addressByte = getAddressByteForWiper(wiper, nonvolatile);
  writeValue(addressByte, dataByte);
}

uint8_t MCP4461::getAddressByteForWiper(uint8_t wiper, bool nonvolatile = false) {
	uint8_t addressByte;
	switch (wiper) {
      case 0:
		addressByte = MCP4461_VW0;
        break;
      case 1:
        addressByte = MCP4461_VW1;
        break;
      case 2:
        addressByte = MCP4461_VW2;
        break;
      case 3:
        addressByte = MCP4461_VW3;
        break;
      default: 
        return;
  }
  if(nonvolatile) addressByte = addressByte + 0x20;
  return addressByte;
}

void MCP4461::setWipers(uint16_t wiper_value, bool nonvolatile = false ) {
  uint16_t value = wiper_value;
  uint8_t i = 0;
  while (i < 4) {
	  setWiper(i, wiper_value, nonvolatile);
	  i++;
  }
}

uint8_t MCP4461::getStatus() {
  uint16_t ret;
  ret = readAddress(MCP4461_STATUS);
  return (ret & 0x00ff);
}

bool MCP4461::getEEPRomWriteActive() {
  return bitRead(getStatus(), 4);
}

uint8_t MCP4461::getTerminalRegister(uint8_t reg) {
  if (reg == 0) {
    return (readAddress(MCP4461_TCON0) & 0x00ff);
  }
  else {
    return (readAddress(MCP4461_TCON1) & 0x00ff);
  }
}

void MCP4461::setTerminalRegister(uint8_t reg, uint8_t value) {
  uint8_t addressByte = 0;
  if (reg == 0) {
    addressByte |= MCP4461_TCON0;
  }
  else {
	addressByte |= MCP4461_TCON1;
  }
  addressByte |= MCP4461_WRITE;
  writeValue(addressByte, value);
}

uint8_t MCP4461::getTerminalState(uint8_t wiper, char terminal) {
  uint8_t reg = 0;
  if (wiper >= 2) {
    reg++;
  }
  uint8_t bitNum;
  bitNum = getTerminalStateBitNumber(wiper, terminal);
  uint8_t tcon;
  tcon = getTerminalRegister(reg);
  return bitRead(tcon, bitNum);
  
}

void MCP4461::setTerminalState(uint8_t wiper, char terminal, uint8_t state) {
	uint8_t reg = 0;
	uint8_t tcon = 0;
	uint8_t bitNum;
	if (wiper >= 2) reg++;
	bitNum = getTerminalStateBitNumber(wiper, terminal);
	tcon = getTerminalRegister(reg);
	if(state == 0) bitClear(tcon, bitNum);
	else bitSet(tcon, bitNum);
	setTerminalRegister(reg, tcon);
}

uint8_t MCP4461::getTerminalStateBitNumber(uint8_t wiper, char terminal) {
	uint8_t bitNum;
	switch(terminal) {
		case 'B':
			bitNum = 0;
			break;
		case 'W':
			bitNum = 1;
			break;
		case 'A':
			bitNum = 2;
			break;
		case 'H':
			bitNum = 3;
			break;
	}
	if (wiper == 1 || wiper == 3) bitNum = bitNum + 4;
	return bitNum;
}

void MCP4461::connectWiper(uint8_t wiper) {
	setTerminalState(wiper, 'H', 1);
}

void MCP4461::disconnectWiper(uint8_t wiper) {
	setTerminalState(wiper, 'H', 0);
}

void MCP4461::toggleWiper(uint8_t wiper) {
	bool state;
    state = getTerminalState(wiper, 'H');
	if(state) {
		disconnectWiper(wiper);
	}
	else {
		connectWiper(wiper);
	}
}

uint16_t MCP4461::getEEPRomGeneralPurposeData(uint8_t location){
  if (location > 4) return 0;
  uint16_t addressByte = 1;
  addressByte |= (MCP4461_EEPROM_DATA_ADDRESS_1 + (location * 0x10));
  uint16_t ret;
  ret = readAddress(addressByte);
  byte retVal[2];
  retVal[0] = (ret >> 8);
  retVal[1] = (ret & 0x00ff);
  uint16_t returnInt;
  returnInt = (retVal[0]*256 + retVal[1]);
  return returnInt;
}

void MCP4461::setEEPRomGeneralPurposeData(uint8_t location, uint16_t value){
	uint8_t addressByte = 0;
	if (value > 511)return;
	addressByte |= (MCP4461_EEPROM_DATA_ADDRESS_1 + (location * 0x10));
	Serial.println(addressByte, HEX);
	writeValue(addressByte, value);
}
