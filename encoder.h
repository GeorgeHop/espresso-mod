#ifndef ENCODER_H
#define ENCODER_H

#include "config.h"

/*
 * Rotary Encoder Driver
 * Handles rotation detection and button press
 * Pins configured for Arduino Nano INT0/INT1
 * Also tracks user activity for auto-shutoff timer
 */

volatile int encoderValue = 0;
volatile boolean encoderButtonPressed = false;
int lastEncoderValue = 0;

// Activity tracking for auto-shutoff
unsigned long lastActivityTime = 0;

// Forward declarations
void encoderISR_CLK();
void encoderISR_DT();

void initEncoder() {
  // Encoder pins with pullups
  pinMode(ENCODER_CLK_PIN, INPUT_PULLUP);    // Pin 2 (INT0)
  pinMode(ENCODER_DT_PIN, INPUT_PULLUP);     // Pin 3 (INT1)
  pinMode(ENCODER_SW_PIN, INPUT_PULLUP);     // Pin 4 (button)
  
  // Attach interrupts for encoder rotation
  attachInterrupt(digitalPinToInterrupt(ENCODER_CLK_PIN), encoderISR_CLK, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_DT_PIN), encoderISR_DT, CHANGE);
  
  // Pin change interrupt for button (no dedicated interrupt on pin 4)
  // We'll handle button debouncing in the main loop via checkSwitches
  
  lastActivityTime = millis();  // Initialize activity timer
  
  Serial.println(F("[Encoder] Initialized on CLK=2, DT=3, SW=4"));
}

// Interrupt handlers for encoder rotation
void encoderISR_CLK() {
  static unsigned long lastTime = 0;
  unsigned long currentTime = millis();
  
  if (currentTime - lastTime < 5) {
    return;  // Debounce
  }
  lastTime = currentTime;
  lastActivityTime = currentTime;  // Track activity
  
  if (digitalRead(ENCODER_CLK_PIN) == digitalRead(ENCODER_DT_PIN)) {
    encoderValue++;
  } else {
    encoderValue--;
  }
}

void encoderISR_DT() {
  static unsigned long lastTime = 0;
  unsigned long currentTime = millis();
  
  if (currentTime - lastTime < 5) {
    return;  // Debounce
  }
  lastTime = currentTime;
  lastActivityTime = currentTime;  // Track activity
  
  if (digitalRead(ENCODER_CLK_PIN) == digitalRead(ENCODER_DT_PIN)) {
    encoderValue--;
  } else {
    encoderValue++;
  }
}

// Get encoder rotation since last call
int getEncoderDelta() {
  int delta = encoderValue - lastEncoderValue;
  lastEncoderValue = encoderValue;
  return delta;
}

// Check if encoder button was pressed (debounced in main loop)
boolean isEncoderButtonPressed() {
  if (encoderButtonPressed) {
    encoderButtonPressed = false;
    return true;
  }
  return false;
}

// Set encoder button pressed flag (called from main loop debounce)
void setEncoderButtonPressed() {
  encoderButtonPressed = true;
  lastActivityTime = millis();  // Track button press as activity
}

// Update activity time - called whenever user interacts
void updateActivityTime() {
  lastActivityTime = millis();
}

// Get time since last activity (milliseconds)
unsigned long getTimeSinceLastActivity() {
  return millis() - lastActivityTime;
}

#endif
