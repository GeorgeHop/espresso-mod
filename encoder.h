#ifndef ENCODER_H
#define ENCODER_H

#include "config.h"

/*
 * Rotary Encoder Driver
 * For menu navigation and parameter adjustment
 */

volatile int encoderValue = 0;
volatile boolean encoderButtonPressed = false;
int lastEncoderValue = 0;

void initEncoder() {
  pinMode(ENCODER_CLK_PIN, INPUT_PULLUP);
  pinMode(ENCODER_DT_PIN, INPUT_PULLUP);
  pinMode(ENCODER_SW_PIN, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(ENCODER_CLK_PIN), encoderISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_SW_PIN), buttonISR, FALLING);
}

void encoderISR() {
  static unsigned long lastTime = 0;
  unsigned long currentTime = millis();
  
  // Debounce
  if (currentTime - lastTime < 5) {
    return;
  }
  lastTime = currentTime;
  
  if (digitalRead(ENCODER_CLK_PIN) == digitalRead(ENCODER_DT_PIN)) {
    encoderValue++;
  } else {
    encoderValue--;
  }
}

void buttonISR() {
  static unsigned long lastTime = 0;
  unsigned long currentTime = millis();
  
  // Debounce
  if (currentTime - lastTime < 50) {
    return;
  }
  lastTime = currentTime;
  
  encoderButtonPressed = true;
}

int getEncoderDelta() {
  int delta = encoderValue - lastEncoderValue;
  lastEncoderValue = encoderValue;
  return delta;
}

boolean isEncoderButtonPressed() {
  if (encoderButtonPressed) {
    encoderButtonPressed = false;
    return true;
  }
  return false;
}

void handleEncoderInput() {
  // Encoder input handling - can be extended in main sketch
  int delta = getEncoderDelta();
  
  if (delta != 0) {
    Serial.print("Encoder: ");
    Serial.println(delta);
  }
  
  if (isEncoderButtonPressed()) {
    Serial.println("Encoder button pressed");
  }
}

#endif
