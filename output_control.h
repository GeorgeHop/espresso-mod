#ifndef OUTPUT_CONTROL_H
#define OUTPUT_CONTROL_H

#include "config.h"

/*
 * Output Control Module
 * Manages SSR (heating element) and MCP4131 (pump pressure)
 */

// MCP4131 digital potentiometer commands
#define MCP4131_WRITE_DATA      0x00
#define MCP4131_READ_DATA       0x0C

// Current wiper positions
byte ssrValue = 0;
byte pumpPressureValue = 0;

void initOutputControl() {
  // SSR output (PWM capable)
  pinMode(SSR_PIN, OUTPUT);
  digitalWrite(SSR_PIN, LOW);
  
  // MCP4131 control pins
  pinMode(MCP4131_CS_PIN, OUTPUT);
  pinMode(MCP4131_CLK_PIN, OUTPUT);
  pinMode(MCP4131_MOSI_PIN, OUTPUT);
  digitalWrite(MCP4131_CS_PIN, HIGH);
  
  Serial.println("Output control initialized");
}

void setSsrOutput(byte pwmValue) {
  // Clamp to 0-255 range
  pwmValue = constrain(pwmValue, 0, 255);
  
  if (pwmValue != ssrValue) {
    ssrValue = pwmValue;
    analogWrite(SSR_PIN, ssrValue);
    
    Serial.print("SSR PWM: ");
    Serial.println(ssrValue);
  }
}

void setPumpPressure(byte potValue) {
  // Clamp to 0-255 range (MCP4131 has 256 positions)
  potValue = constrain(potValue, 0, 255);
  
  if (potValue != pumpPressureValue) {
    pumpPressureValue = potValue;
    writeMcp4131(potValue);
    
    Serial.print("Pump pressure POT: ");
    Serial.println(potValue);
  }
}

void writeMcp4131(byte value) {
  digitalWrite(MCP4131_CS_PIN, LOW);
  delayMicroseconds(10);
  
  // Send write command to wiper 0
  // Format: 0 0 ADDRESS(2 bits) | DATA(8 bits)
  // For wiper 0: 0x00 = write to wiper 0
  byte command = MCP4131_WRITE_DATA;
  
  // Shift out command byte
  shiftOut(MCP4131_MOSI_PIN, MCP4131_CLK_PIN, MSBFIRST, command);
  
  // Shift out data byte
  shiftOut(MCP4131_MOSI_PIN, MCP4131_CLK_PIN, MSBFIRST, value);
  
  delayMicroseconds(10);
  digitalWrite(MCP4131_CS_PIN, HIGH);
  
  delay(1); // Allow time for MCP4131 to process
}

byte readMcp4131() {
  digitalWrite(MCP4131_CS_PIN, LOW);
  delayMicroseconds(10);
  
  // Send read command to wiper 0
  byte command = MCP4131_READ_DATA;
  shiftOut(MCP4131_MOSI_PIN, MCP4131_CLK_PIN, MSBFIRST, command);
  
  // Read data byte
  byte value = shiftIn(MCP4131_MOSI_PIN, MCP4131_CLK_PIN, MSBFIRST);
  
  delayMicroseconds(10);
  digitalWrite(MCP4131_CS_PIN, HIGH);
  
  return value;
}

// Safety functions
void emergencyShutdown() {
  setSsrOutput(0);
  setPumpPressure(0);
  Serial.println("EMERGENCY SHUTDOWN - All outputs disabled");
}

bool readBrewSwitch() {
  return digitalRead(BREW_SWITCH_PIN) == LOW;
}

bool readSteamSwitch() {
  return digitalRead(STEAM_SWITCH_PIN) == LOW;
}

#endif
