#ifndef PRESSURE_SENSOR_H
#define PRESSURE_SENSOR_H

#include "config.h"

/*
 * Pressure Sensor Driver (5V G1/4 Water Pressure)
 * Typical analog 0-10 bar sensor with 0-5V output
 * Arduino ADC: 0-1023 (10-bit) for 0-5V input
 */

static int sampleBuffer[PRESSURE_SMOOTHING] = {0};
static int sampleIndex = 0;
static float lastPressure = 0.0;

void initPressureSensor() {
  pinMode(PRESSURE_SENSOR_PIN, INPUT);
  
  // Read a few samples during init for baseline
  for (int i = 0; i < PRESSURE_SMOOTHING; i++) {
    sampleBuffer[i] = analogRead(PRESSURE_SENSOR_PIN);
    delay(10);
  }
}

float readPressureSensor() {
  // Read current ADC value
  int adcValue = analogRead(PRESSURE_SENSOR_PIN);
  
  // Add to circular buffer
  sampleBuffer[sampleIndex] = adcValue;
  sampleIndex = (sampleIndex + 1) % PRESSURE_SMOOTHING;
  
  // Calculate average
  int sum = 0;
  for (int i = 0; i < PRESSURE_SMOOTHING; i++) {
    sum += sampleBuffer[i];
  }
  int avgAdc = sum / PRESSURE_SMOOTHING;
  
  // Convert ADC to pressure (bar)
  // Map from ADC range (PRESSURE_MIN_ADC to PRESSURE_MAX_ADC) to bar range
  float pressure = (float)(avgAdc - PRESSURE_MIN_ADC) / (float)(PRESSURE_MAX_ADC - PRESSURE_MIN_ADC);
  pressure = pressure * (PRESSURE_MAX_BAR - PRESSURE_MIN_BAR) + PRESSURE_MIN_BAR;
  
  // Clamp to valid range
  pressure = constrain(pressure, PRESSURE_MIN_BAR, PRESSURE_MAX_BAR);
  
  // Apply low-pass filter for stability
  lastPressure = (lastPressure * 0.7) + (pressure * 0.3);
  
  return lastPressure;
}

float getPressure() {
  return lastPressure;
}

// Calibration helper - call this to calibrate the 0-bar point
void calibratePressureZero() {
  // Take average of 10 readings with no pressure applied
  long sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += analogRead(PRESSURE_SENSOR_PIN);
    delay(20);
  }
  // This would be used to update PRESSURE_MIN_ADC in config.h
}

#endif
