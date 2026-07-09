#ifndef THERMOCOUPLE_H
#define THERMOCOUPLE_H

#include "config.h"

/*
 * MAX31855 Thermocouple Driver
 * K-type thermocouple interface for reading espresso machine temperature
 */

void initThermocouple() {
  pinMode(MAX31855_CS_PIN, OUTPUT);
  digitalWrite(MAX31855_CS_PIN, HIGH);
  
  // SPI is initialized by display or other components
}

float readThermocouple() {
  digitalWrite(MAX31855_CS_PIN, LOW);
  delayMicroseconds(100);
  
  // Read 4 bytes from MAX31855
  uint32_t data = 0;
  for (int i = 0; i < 4; i++) {
    data = (data << 8) | spiRead();
  }
  
  digitalWrite(MAX31855_CS_PIN, HIGH);
  
  // Check for thermocouple fault
  if (data & 0x10000) {
    Serial.println("MAX31855 Error: Thermocouple fault detected");
    return -999.0; // Error code
  }
  
  // Extract temperature (bits 31-18, 14 bits, 0.25°C resolution)
  int16_t rawTemp = (data >> 18) & 0x3FFF;
  
  // Handle negative temperatures (two's complement)
  if (rawTemp & 0x2000) {
    rawTemp |= 0xC000;
  }
  
  float temperature = rawTemp * 0.25;
  temperature += TEMP_OFFSET;
  temperature *= TEMP_SCALE;
  
  return temperature;
}

// Helper function to read SPI byte
byte spiRead() {
  byte data = 0;
  for (int i = 7; i >= 0; i--) {
    digitalWrite(MAX31855_CLK_PIN, LOW);
    delayMicroseconds(1);
    
    if (digitalRead(MAX31855_MISO_PIN)) {
      data |= (1 << i);
    }
    
    digitalWrite(MAX31855_CLK_PIN, HIGH);
    delayMicroseconds(1);
  }
  return data;
}

#endif
