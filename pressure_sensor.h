#ifndef PRESSURE_SENSOR_H
#define PRESSURE_SENSOR_H

#include "config.h"

/*
 * HX710B Load Cell Amplifier Driver
 * Reads pressure from a pressure transducer via HX710B ADC
 */

static long pressureOffset = 0;
static float lastPressure = 0.0;
static int sampleBuffer[PRESSURE_SMOOTHING];
static int sampleIndex = 0;

void initPressureSensor() {
  pinMode(HX710B_DOUT_PIN, INPUT);
  pinMode(HX710B_CLK_PIN, OUTPUT);
  digitalWrite(HX710B_CLK_PIN, LOW);
  
  // Wait for sensor startup
  delay(100);
  
  // Calibrate zero point (assuming no pressure at startup)
  pressureOffset = readRawPressure();
  
  Serial.println("Pressure sensor calibrated");
}

float readPressureSensor() {
  // Read multiple samples and average
  long sum = 0;
  for (int i = 0; i < 3; i++) {
    sum += readRawPressure();
    delay(10);
  }
  long rawValue = sum / 3;
  
  // Convert to pressure
  long difference = rawValue - pressureOffset;
  float pressure = difference * PRESSURE_SCALE;
  pressure += PRESSURE_OFFSET;
  
  // Apply exponential moving average for smoothing
  lastPressure = (lastPressure * FLOW_SMOOTHING_FACTOR) + (pressure * (1.0 - FLOW_SMOOTHING_FACTOR));
  
  return lastPressure;
}

long readRawPressure() {
  // Wait for data ready
  int count = 0;
  while (digitalRead(HX710B_DOUT_PIN) == HIGH) {
    count++;
    if (count > 100000) {
      Serial.println("HX710B: Timeout waiting for data");
      return pressureOffset;
    }
  }
  
  // Read 24-bit value
  long value = 0;
  for (int i = 0; i < 24; i++) {
    digitalWrite(HX710B_CLK_PIN, HIGH);
    delayMicroseconds(1);
    digitalWrite(HX710B_CLK_PIN, LOW);
    delayMicroseconds(1);
    
    if (digitalRead(HX710B_DOUT_PIN) == HIGH) {
      value |= (1L << (23 - i));
    }
  }
  
  // 25th pulse for next conversion
  digitalWrite(HX710B_CLK_PIN, HIGH);
  delayMicroseconds(1);
  digitalWrite(HX710B_CLK_PIN, LOW);
  delayMicroseconds(1);
  
  // Convert from 24-bit two's complement
  if (value & 0x800000) {
    value |= 0xFF000000;
  }
  
  return value;
}

void calibratePressure(long zeroValue) {
  pressureOffset = zeroValue;
}

#endif
