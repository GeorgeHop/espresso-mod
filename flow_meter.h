#ifndef FLOW_METER_H
#define FLOW_METER_H

#include "config.h"

/*
 * YF-S201 Flow Meter Driver
 * Hall effect sensor for measuring water flow rate
 */

volatile unsigned long pulseCount = 0;
unsigned long lastFlowTime = 0;
float currentFlowRate = 0.0;

void initFlowMeter() {
  pinMode(FLOW_METER_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(FLOW_METER_PIN), flowInterrupt, RISING);
  lastFlowTime = millis();
}

// Interrupt handler for flow meter pulses
void flowInterrupt() {
  pulseCount++;
}

float getFlowRate() {
  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - lastFlowTime;
  
  if (elapsedTime >= 1000) {  // Calculate every second
    noInterrupts();
    unsigned long pulses = pulseCount;
    pulseCount = 0;
    interrupts();
    
    // Convert pulses to ml/s
    // YF-S201: 450 pulses per liter = 450 pulses per 1000 ml
    // So: (pulses / 450) liters = (pulses / 450 * 1000) ml
    float flowLitersPerSecond = (float)pulses / FLOW_METER_PPL;
    float flowMlPerSecond = flowLitersPerSecond * 1000.0;
    
    // Apply smoothing
    currentFlowRate = (currentFlowRate * FLOW_SMOOTHING_FACTOR) + 
                      (flowMlPerSecond * (1.0 - FLOW_SMOOTHING_FACTOR));
    
    lastFlowTime = currentTime;
  }
  
  return currentFlowRate;
}

// Get total volume dispensed in milliliters
float getTotalVolume() {
  // Formula: (total pulses / pulses per liter) * 1000 ml
  return ((float)pulseCount / FLOW_METER_PPL) * 1000.0;
}

// Reset flow meter
void resetFlowMeter() {
  noInterrupts();
  pulseCount = 0;
  interrupts();
  currentFlowRate = 0.0;
}

#endif
