#include "Arduino.h"
#include "MCP4461.h"
#include <stdio.h>
#include <Wire.h>

/*
Library to control the MCP4461 Digital Potentiometer over I2C.
http://ww1.microchip.com/downloads/en/DeviceDoc/22265a.pdf
This library does not fully implement the functionality of
the MCP4461 just the basics of changing the wiper values.
The master joins the bus with the default address of 0

No warranty given or implied, use at your own risk.

Tony@think3dprint3d.com
GPL v3
*/

//ensure you call begin() before any other functions but note
//begin can only be called once for all MCP* objects as it initialises
//the local master through the Wire library
//if the MCP4461 does not have a default address, call set address before
//trying to communicate
MCP4461::MCP4461(uint8_t i2c_address) {
  _mcp4461_address = i2c_address;
}

//set the MCP4461 address
void MCP4461::setMCP4461Address(uint8_t mcp4461_addr) {
	_mcp4461_address = mcp4461_addr;
}

void MCP4461::setVolatileWiper(uint8_t wiper, uint16_t wiper_value){
  uint16_t value = wiper_value;
  if (value > 257) return; //max 257 taps allowed
  uint8_t d_byte = (uint8_t)value;
  uint8_t c_byte;
  if (value > 0xFF)c_byte = 0x1; //the 8th data bit is 1
  else c_byte =0;
  switch (wiper) {
      case 0:
        c_byte |= MCP4461_VW0;
        break;
      case 1:
        c_byte |= MCP4461_VW1;
        break;
      case 2:
        c_byte |= MCP4461_VW2;
        break;
      case 3:
        c_byte |= MCP4461_VW3;
        break;
      default: 
        break; //not a valid wiper
  } 
  c_byte |= MCP4461_WRITE;
  //send command byte
  Wire.beginTransmission(_mcp4461_address);
  Wire.write(c_byte);
  Wire.write(d_byte);
  Wire.endTransmission(); //do not release bus
}

void MCP4461::setNonVolatileWiper(uint8_t wiper, uint16_t wiper_value){
  uint16_t value = wiper_value;
  if (value > 257) return; //max 257 taps allowed
  uint8_t d_byte = (uint8_t)value;
  uint8_t c_byte;
  if (value > 0xFF)c_byte = 0x1; //the 8th data bit is 1
  else c_byte =0;
  switch (wiper) {
      case 0:
        c_byte |= MCP4461_NVW0;
        break;
      case 1:
        c_byte |= MCP4461_NVW1;
        break;
      case 2:
        c_byte |= MCP4461_NVW2;
        break;
      case 3:
        c_byte |= MCP4461_NVW3;
        break;
      default: 
        break; //not a valid wiper
  } 
  c_byte |= MCP4461_WRITE;
  //send command byte
  Wire.beginTransmission(_mcp4461_address);
  Wire.write(c_byte);
  Wire.write(d_byte);
  Wire.endTransmission(); //do not release bus
  while(getEEPRomWriteActive()) {
	  Serial.println("write active");
	  delayMicroseconds(1);
  }
}
  
//set all the wipers in one transmission, more verbose but quicker than multiple calls to
//setVolatileWiper(uint8_t wiper, uint16_t wiper_value)
void MCP4461::setVolatileWipers(uint16_t wiper_value){
  uint16_t value = wiper_value;
  if (value > 257) return; //max 257 taps allowed
  uint8_t d_byte = (uint8_t)value;
  uint8_t c_byte;
  if (value > 0xFF)c_byte = 0x1; //the 8th data bit is 1
  else c_byte =0;
  Wire.beginTransmission(_mcp4461_address);
  c_byte |= MCP4461_WRITE;
  c_byte |= MCP4461_VW0;
  Wire.write(c_byte);
  Wire.write(d_byte);
  if (value > 0xFF) c_byte = 0x1;
  else c_byte =0;
  c_byte |= MCP4461_WRITE;
  c_byte |= MCP4461_VW1;
  Wire.write(c_byte);
  Wire.write(d_byte);
  if (value > 0xFF) c_byte = 0x1;
  else c_byte =0;
  c_byte |= MCP4461_WRITE;
  c_byte |= MCP4461_VW2;
  Wire.write(c_byte);
  Wire.write(d_byte);
  if (value > 0xFF) c_byte = 0x1;
  else c_byte =0;
  c_byte |= MCP4461_WRITE;
  c_byte |= MCP4461_VW3;
  Wire.write(c_byte);
  Wire.write(d_byte);
  Wire.endTransmission();
}

//set all the wipers in one transmission, more verbose but quicker than multiple calls to
//setNonVolatileWiper(uint8_t wiper, uint16_t wiper_value)
void MCP4461::setNonVolatileWipers(uint16_t wiper_value){
 uint16_t value = wiper_value;
  if (value > 257) return; //max 257 taps allowed
  uint8_t d_byte = (uint8_t)value;
  uint8_t c_byte;
  if (value > 0xFF)c_byte = 0x1; //the 8th data bit is 1
  else c_byte =0;
  Wire.beginTransmission(_mcp4461_address);
  c_byte |= MCP4461_WRITE;
  c_byte |= MCP4461_NVW0;
  Wire.write(c_byte);
  Wire.write(d_byte);
  while(getEEPRomWriteActive()) {
	  delayMicroseconds(1);
  }
  if (value > 0xFF) c_byte = 0x1;
  else c_byte =0;
  c_byte |= MCP4461_WRITE;
  c_byte |= MCP4461_NVW1;
  Wire.write(c_byte);
  Wire.write(d_byte);
  while(getEEPRomWriteActive()) {
	  delayMicroseconds(1);
  }
  if (value > 0xFF) c_byte = 0x1;
  else c_byte =0;
  c_byte |= MCP4461_WRITE;
  c_byte |= MCP4461_NVW2;
  Wire.write(c_byte);
  Wire.write(d_byte);
  while(getEEPRomWriteActive()) {
	  delayMicroseconds(1);
  }
  if (value > 0xFF) c_byte = 0x1;
  else c_byte =0;
  c_byte |= MCP4461_WRITE;
  c_byte |= MCP4461_NVW3;
  Wire.write(c_byte);
  Wire.write(d_byte);
  Wire.endTransmission();
  while(getEEPRomWriteActive()) {
	  delayMicroseconds(1);
  }
}

//return the value for a specific wiper
uint16_t MCP4461::getNonVolatileWiper(uint8_t wiper) const{
  uint16_t ret = 0;
  uint16_t c_byte =0;
  switch (wiper) {
      case 0:
        c_byte |= MCP4461_NVW0;
        break;
      case 1:
        c_byte |= MCP4461_NVW1;
        break;
      case 2:
        c_byte |= MCP4461_NVW2;
        break;
      case 3:
        c_byte |= MCP4461_NVW3;
        break;
      default: 
        return 0; //not a valid wiper
  } 
  c_byte |= MCP4461_READ; 
  //send command byte
  Wire.beginTransmission(_mcp4461_address);
  Wire.write(c_byte);
  Wire.endTransmission(false); //do not release bus
  Wire.requestFrom((uint8_t)_mcp4461_address,(uint8_t)2);
  //read the register
  int i = 0;
  while(Wire.available()) 
  { 
    ret |= Wire.read();
    if (i==0) ret = ret<<8;
    i++;
  }
  return ret;
}
  
//return the volatile value for a specific wiper
uint16_t MCP4461::getVolatileWiper(uint8_t wiper){
  uint16_t ret = 0;
  uint16_t c_byte =0;
  switch (wiper) {
      case 0:
        c_byte |= MCP4461_VW0;
        break;
      case 1:
        c_byte |= MCP4461_VW1;
        break;
      case 2:
        c_byte |= MCP4461_VW2;
        break;
      case 3:
        c_byte |= MCP4461_VW3;
        break;
      default: 
        return 0; //not a valid wiper
  } 
  c_byte |= MCP4461_READ; 
  //send command byte
  Wire.beginTransmission(_mcp4461_address);
  Wire.write(c_byte);
  Wire.endTransmission(false); //do not release bus
  Wire.requestFrom((uint8_t)_mcp4461_address,(uint8_t)2);
  //read the register
  int i = 0;
  while(Wire.available()) 
  { 
    ret |= Wire.read();
    if (i==0) ret = ret<<8;
    i++;
  }
  return ret;
} 


//return the status register
uint8_t MCP4461::getStatus(){
  uint16_t ret = 0;
  uint16_t c_byte =0;
  c_byte |= MCP4461_STATUS;
  c_byte |= MCP4461_READ; 
  //send command byte
  Wire.beginTransmission(_mcp4461_address);
  Wire.write(c_byte);
  Wire.endTransmission(false); //do not release bus
  Wire.requestFrom((uint8_t)_mcp4461_address, (uint8_t)2);
  //read the register
  int i = 0;
  while(Wire.available()) 
  { 
    ret |= Wire.read();
    if (i==0) ret = ret<<8;
    i++;
  }
  return (ret & 0x00ff);
}

bool MCP4461::getEEPRomWriteActive() {
  return bitRead(getStatus(), 4);
}

uint16_t MCP4461::getEEPRomGeneralPurposeData(uint8_t location){
  uint16_t ret = 0;
  uint16_t c_byte = 0;
  switch (location) {
      case 0:
        c_byte |= MCP4461_EEPROM_DATA_ADDRESS_1;
        break;
      case 1:
        c_byte |= MCP4461_EEPROM_DATA_ADDRESS_2;
        break;
      case 2:
        c_byte |= MCP4461_EEPROM_DATA_ADDRESS_3;
        break;
      case 3:
        c_byte |= MCP4461_EEPROM_DATA_ADDRESS_4;
        break;
	  case 4:
        c_byte |= MCP4461_EEPROM_DATA_ADDRESS_5;
        break;
      default: 
        break; //not a valid location of eeprom general purpose data
  }
  c_byte |= MCP4461_READ;
  //send command byte
  Wire.beginTransmission(_mcp4461_address);
  Wire.write(c_byte);
  Wire.endTransmission(false); //do not release bus
  Wire.requestFrom((uint8_t)_mcp4461_address, (uint8_t)2);
  //read the register
  int i = 0;
  while(Wire.available()) 
  { 
    ret |= Wire.read();
    if (i==0) ret = ret << 8;
    i++;
  }
  return ret;
}

void MCP4461::setEEPRomGeneralPurposeData(uint8_t location, uint16_t eeprom_value){
  uint16_t value = eeprom_value;
  if (value > 511)return; //highest value is 511
  uint8_t d_byte = (uint8_t)value;
  uint8_t c_byte;
  if (value > 0xFF)c_byte = 0x1; //the 8th data bit is 1
  else c_byte = 0;
  switch (location) {
      case 0:
        c_byte |= MCP4461_EEPROM_DATA_ADDRESS_1;
        break;
      case 1:
        c_byte |= MCP4461_EEPROM_DATA_ADDRESS_2;
        break;
      case 2:
        c_byte |= MCP4461_EEPROM_DATA_ADDRESS_3;
        break;
      case 3:
        c_byte |= MCP4461_EEPROM_DATA_ADDRESS_4;
        break;
	  case 4:
        c_byte |= MCP4461_EEPROM_DATA_ADDRESS_5;
        break;
      default: 
        break; //not a valid location
  } 
  c_byte |= MCP4461_WRITE;
  //send command byte
  Wire.beginTransmission(_mcp4461_address);
  Wire.write(c_byte);
  Wire.write(d_byte);
  Wire.endTransmission(); //do not release bus
  while(getEEPRomWriteActive()) {
	  //Serial.println("write active");
	  delayMicroseconds(1);
  }
}

uint8_t MCP4461::getTerminalRegister(uint8_t reg) {
  uint16_t tcon = 0;
  uint16_t c_byte = 0;
  if (reg == 0) {
    c_byte |= MCP4461_TCON0;
  }
  else {
    c_byte |= MCP4461_TCON1;  
  }
  c_byte |= MCP4461_READ;
  Wire.beginTransmission(_mcp4461_address);
  Wire.write(c_byte);
  Wire.endTransmission(false);
  Wire.requestFrom((uint8_t)_mcp4461_address,(uint8_t)2);
  uint8_t i = 0;
  while(Wire.available()) 
  { 
    tcon |= Wire.read();
    if (i==0) tcon = tcon<<8;
    i++;
  }
  tcon = (tcon & 0x00ff);
  return tcon;
}

void MCP4461::setTerminalRegister(uint8_t reg, uint8_t value) {
  uint8_t c_byte = 0;
  if (reg == 0) {
    c_byte |= MCP4461_TCON0;
  }
  else {
	c_byte |= MCP4461_TCON1;
  }
  c_byte |= MCP4461_WRITE;
  Wire.beginTransmission(_mcp4461_address);
  Wire.write(c_byte);
  Wire.write(value);
  Wire.endTransmission();
  while(getEEPRomWriteActive()) {
	  delayMicroseconds(1);
  }
}

void MCP4461::setTerminalState(uint8_t wiper, char terminal, uint8_t state) {
  uint8_t tcon;
  uint8_t bitNum;
  if (wiper <= 1) {
    tcon = getTerminalRegister(0);
  }
  else {
    tcon = getTerminalRegister(1);
  }
  if (wiper == 0 || wiper == 2) {
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
  }
  else {
	switch(terminal) {
		case 'B':
			bitNum = 4;
			break;
		case 'W':
			bitNum = 5;
			break;
		case 'A':
			bitNum = 6;
			break;
		case 'H':
			bitNum = 7;
			break;
	}
  }
  if(state == 0) bitClear(tcon, bitNum);
  else bitSet(tcon, bitNum);
  if (wiper <= 1) {
    setTerminalRegister(0, tcon);
  }
  else {
    setTerminalRegister(1, tcon);
  }
}

//toggle a specific pot channel on and off
bool MCP4461::getTerminalState(uint8_t wiper, char terminal) {
  uint8_t tcon;
  if (wiper <= 1) {
    tcon = getTerminalRegister(0);
  }
  else {
    tcon = getTerminalRegister(1);
  }
  bool state;
  if (wiper == 0 || wiper == 2) {
    switch(terminal) {
		case 'B':
			state = bitRead(tcon, 0);
			break;
		case 'W':
			state = bitRead(tcon, 1);
			break;
		case 'A':
			state = bitRead(tcon, 2);
			break;
		case 'H':
			state = bitRead(tcon, 3);
			break;
	}
  }
  else {
	switch(terminal) {
		case 'B':
			state = bitRead(tcon, 4);
			break;
		case 'W':
			state = bitRead(tcon, 5);
			break;
		case 'A':
			state = bitRead(tcon, 6);
			break;
		case 'H':
			state = bitRead(tcon, 7);
			break;
	}
  }
  return state;
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
