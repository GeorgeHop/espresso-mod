#ifndef OUTPUT_CONTROL_H
#define OUTPUT_CONTROL_H

#include "config.h"

/*
 * Output Control Module
 * Manages:
 * - SSR (Solid State Relay) for heating element control
 * - PWM Pump controller (AC220V YYAC-3S) for pump speed/pressure
 * 
 * SSR control: PWM on Arduino Nano pin 9
 * Pump control: Analog 0-5V signal on pin A2
 */

volatile byte ssrValue = 0;
volatile byte ssrTarget = 0;
volatile byte pumpValue = 0;
volatile bool relayActive = false;  // Tracks relay state (HIGH = ON, LOW = OFF)
volatile float currentBrewPressure = PUMP_DEFAULT_BREW_PRESSURE;
volatile float currentSteamPressure = PUMP_DEFAULT_STEAM_PRESSURE;

void initOutputControl() {
  // SSR output (PWM capable - pin 9 on Nano)
  pinMode(SSR_PIN, OUTPUT);
  digitalWrite(SSR_PIN, LOW);
  
  // Pump PWM controller (0-5V analog signal)
  pinMode(PUMP_PWM_PIN, OUTPUT);
  digitalWrite(PUMP_PWM_PIN, LOW);
  
  // Relay control (HIGH = ON, LOW = OFF)
  pinMode(RELAY_CONTROL_PIN, OUTPUT);
  digitalWrite(RELAY_CONTROL_PIN, LOW);  // Start with relay OFF
  relayActive = false;
  
  currentBrewPressure = PUMP_DEFAULT_BREW_PRESSURE;
  currentSteamPressure = PUMP_DEFAULT_STEAM_PRESSURE;
  Serial.println(F("[Output Control] Initialized - SSR pin 9, Pump PWM pin A2, Relay pin A4"));
}

// ===== SSR HEATING CONTROL =====
void setSsrOutput(byte pwmValue) {
  // Clamp to 0-255 range
  pwmValue = constrain(pwmValue, SSR_MIN_VALUE, SSR_MAX_VALUE);
  
  ssrValue = pwmValue;
  analogWrite(SSR_PIN, ssrValue);
}

byte getSsrOutput() {
  return ssrValue;
}

void setSsrPercentage(byte percent) {
  percent = constrain(percent, 0, 100);
  byte pwmValue = (percent * 255) / 100;
  setSsrOutput(pwmValue);
}

// ===== PUMP CONTROL (PWM AC220V Controller) =====
// The YYAC-3S PWM controller receives 0-5V signal to control pump speed
// Higher voltage = higher pump speed = more pressure
void setPumpOutput(byte pwmValue) {
  // Clamp to 0-255 range
  pwmValue = constrain(pwmValue, PUMP_MIN_VALUE, PUMP_MAX_VALUE);
  
  pumpValue = pwmValue;
  analogWrite(PUMP_PWM_PIN, pwmValue);  // 0-255 maps to 0-5V on PWM output
}

byte getPumpOutput() {
  return pumpValue;
}

// Set pump speed based on desired brew pressure (bar)
// Maps pressure value to pump PWM output (0-255)
// Linear mapping: 2.0 bar -> PWM 50, 15.0 bar -> PWM 255
void setPumpBrewPressure(float pressureBar) {
  pressureBar = constrain(pressureBar, MIN_BREW_PRESSURE, MAX_BREW_PRESSURE);
  currentBrewPressure = pressureBar;
  
  byte pwmValue = (byte)(((pressureBar - MIN_BREW_PRESSURE) / (MAX_BREW_PRESSURE - MIN_BREW_PRESSURE)) * 255.0);
  setPumpOutput(pwmValue);
}

// Set pump speed based on desired steam pressure (bar)
// Maps pressure value to pump PWM output (0-255)
void setPumpSteamPressure(float pressureBar) {
  pressureBar = constrain(pressureBar, MIN_STEAM_PRESSURE, MAX_STEAM_PRESSURE);
  currentSteamPressure = pressureBar;
  
  byte pwmValue = (byte)(((pressureBar - MIN_STEAM_PRESSURE) / (MAX_STEAM_PRESSURE - MIN_STEAM_PRESSURE)) * 255.0);
  setPumpOutput(pwmValue);
}

// Legacy function - redirects to steam pressure for compatibility
void setPumpPressure(float pressureBar) {
  setPumpSteamPressure(pressureBar);
}

float getCurrentBrewPressure() {
  return currentBrewPressure;
}

float getCurrentSteamPressure() {
  return currentSteamPressure;
}

void setPumpPercentage(byte percent) {
  percent = constrain(percent, 0, 100);
  byte pwmValue = (percent * 255) / 100;
  setPumpOutput(pwmValue);
}

// ===== RELAY CONTROL (Power to machine) =====
// HIGH (5V) = relay energized = machine powered ON
// LOW (0V) = relay de-energized = machine powered OFF
void setRelayOutput(bool state) {
  relayActive = state;
  digitalWrite(RELAY_CONTROL_PIN, state ? HIGH : LOW);
  
  if (state) {
    Serial.println(F("[Relay] Machine powered ON"));
  } else {
    Serial.println(F("[Relay] Machine powered OFF"));
  }
}

bool getRelayState() {
  return relayActive;
}

// Emergency shutdown - turn off all outputs
void emergencyShutdown() {
  setSsrOutput(0);
  setPumpOutput(0);
  setRelayOutput(false);  // Cut power to machine
}

#endif
