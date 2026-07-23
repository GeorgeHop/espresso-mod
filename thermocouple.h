#ifndef THERMOCOUPLE_H
#define THERMOCOUPLE_H

#include "config.h"
#include <SPI.h>

/*
 * MAX6675 Thermocouple Driver
 * K-type thermocouple interface (up to 800°C)
 * SPI-based reading: 16-bit data with MSB first
 * 
 * Data format:
 * Bit 15-5: Temperature (11 bits, 0.25°C resolution)
 * Bit 4: N/C
 * Bit 3: OC (open circuit fault)
 * Bit 2: GND (ground fault)  
 * Bit 1: Vcc (power fault)
 * Bit 0: Always 0
 */

float tempFiltered = 0.0;
const float TEMP_FILTER_ALPHA = 0.3; // Low-pass filter constant

void initThermocouple() {
  pinMode(MAX6675_CS_PIN, OUTPUT);
  digitalWrite(MAX6675_CS_PIN, HIGH);
  
  // SPI is initialized by display setup
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV4);  // ~4MHz for MAX6675
  SPI.setDataMode(SPI_MODE0);           // CPOL=0, CPHA=0
  
  Serial.print(F("[Thermocouple] MAX6675 initialized on CS="));
  Serial.println(MAX6675_CS_PIN);
}

float readThermocouple() {
  digitalWrite(MAX6675_CS_PIN, LOW);
  delayMicroseconds(100);
  
  // Read 16 bits from MAX6675
  uint16_t data = 0;
  data = (SPI.transfer(0x00) << 8) | SPI.transfer(0x00);
  
  digitalWrite(MAX6675_CS_PIN, HIGH);
  
  // Check for faults
  if (data & 0x04) {
    Serial.println(F("[Thermocouple] Error: Ground fault detected"));
    return -999.0;
  }
  if (data & 0x02) {
    Serial.println(F("[Thermocouple] Error: Vcc fault detected"));
    return -999.0;
  }
  if (data & 0x01) {
    Serial.println(F("[Thermocouple] Error: Open circuit detected"));
    return -999.0;
  }
  
  // Extract temperature (bits 15-5, 11 bits, 0.25°C resolution)
  int16_t rawTemp = (data >> 3) & 0x0FFF;
  float temperature = rawTemp * 0.25;
  
  // Apply calibration
  temperature += TEMP_OFFSET;
  temperature *= TEMP_SCALE;
  
  // Apply low-pass filter for smoothing
  tempFiltered = (tempFiltered * (1.0 - TEMP_FILTER_ALPHA)) + (temperature * TEMP_FILTER_ALPHA);
  
  return tempFiltered;
}



#endif
