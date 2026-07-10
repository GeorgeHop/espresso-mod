/*
 * Espresso Machine Mod Controller
 * Arduino Nano with integrated temperature, pressure, and flow control
 * 
 * Hardware:
 * - MAX31855 Thermocouple
 * - HX710B Pressure Sensor
 * - YF-S201 Flow Meter
 * - Rotary Encoder for navigation
 * - TFT Display for UI
 * - SSR for heating element control
 * - MCP4131 digital pot for pump pressure control
 */

#include "config.h"
#include "thermocouple.h"
#include "pressure_sensor.h"
#include "flow_meter.h"
#include "encoder.h"
#include "display.h"
#include "output_control.h"
#include "menu.h"

// Global variables
float currentTemp = 0.0;
float targetTemp = 92.0;
float currentPressure = 0.0;
float targetPressure = 9.0;
float flowRate = 0.0;
float currentVolume = 0.0;
float targetVolume = DEFAULT_TARGET_VOLUME;
unsigned long targetTime = DEFAULT_TARGET_TIME;
unsigned long preInfusionTime = PREINFUSION_TIME;
byte preInfusionEnabled = PREINFUSION_ENABLED;
unsigned long brewStartTime = 0;
unsigned long elapsedBrewTime = 0;
unsigned long preInfusionStartTime = 0;
bool steamMode = false;

// State machine
enum State {
  STATE_IDLE,
  STATE_HEATING,
  STATE_PRE_INFUSION,
  STATE_BREWING,
  STATE_STEAMING,
  STATE_MENU
};
State currentState = STATE_IDLE;

// Timing
unsigned long lastTempUpdate = 0;
unsigned long lastPressureUpdate = 0;
unsigned long lastFlowUpdate = 0;
unsigned long lastDisplayUpdate = 0;
unsigned long preInfusionStartTime = 0;

void setup() {
  Serial.begin(115200);
  
  // Initialize all components
  initDisplay();
  initThermocouple();
  initPressureSensor();
  initFlowMeter();
  initEncoder();
  initOutputControl();
  
  Serial.println("Espresso Controller Initialized");
  displayInit();
}

void loop() {
  unsigned long currentMillis = millis();
  
  // Update sensor readings at different intervals
  if (currentMillis - lastTempUpdate >= TEMP_UPDATE_INTERVAL) {
    currentTemp = readThermocouple();
    lastTempUpdate = currentMillis;
  }
  
  if (currentMillis - lastPressureUpdate >= PRESSURE_UPDATE_INTERVAL) {
    currentPressure = readPressureSensor();
    lastPressureUpdate = currentMillis;
  }
  
  if (currentMillis - lastFlowUpdate >= FLOW_UPDATE_INTERVAL) {
    flowRate = getFlowRate();
    lastFlowUpdate = currentMillis;
  }
  
  // Check switch state
  steamMode = readSteamSwitch();
  
  // Handle encoder input
  handleEncoderInput();
  
  // Handle menu with encoder
  processMenuInput();
  
  // State machine
  updateState();
  
  // Control outputs
  updateOutputs();
  
  // Update display
  if (currentMillis - lastDisplayUpdate >= DISPLAY_UPDATE_INTERVAL) {
    unsigned long preInfusionElapsed = 0;
    if (currentState == STATE_PRE_INFUSION) {
      preInfusionElapsed = (millis() - preInfusionStartTime) / 1000;
    }
    updateDisplay(currentTemp, targetTemp, currentPressure, targetPressure, 
                  flowRate, currentVolume, targetVolume, elapsedBrewTime, 
                  targetTime, currentState, preInfusionElapsed);
    lastDisplayUpdate = currentMillis;
  }
}

void updateState() {
  // Check steam switch first (priority)
  if (steamMode) {
    if (currentState != STATE_STEAMING) {
      currentState = STATE_STEAMING;
      targetTemp = STEAM_TARGET_TEMP;
      Serial.println("Entering STEAM state");
    }
  } else {
    // Not in steam mode
    switch (currentState) {
      case STATE_IDLE:
        currentVolume = 0.0;
        elapsedBrewTime = 0;
        if (readBrewSwitch()) {
          currentState = STATE_HEATING;
          targetTemp = DEFAULT_TARGET_TEMP;  // Reset to espresso temp
          Serial.println("Entering HEATING state");
        }
        break;
        
      case STATE_HEATING:
        if (currentTemp >= targetTemp - TEMP_HYSTERESIS) {
          currentState = STATE_BREWING;
          brewStartTime = millis();
          currentVolume = 0.0;
          resetFlowMeter();
          Serial.println("Entering BREWING state");
        }
        break;
        
      case STATE_BREWING:
        elapsedBrewTime = (millis() - brewStartTime) / 1000;
        currentVolume = getTotalVolume();
        
        // Check if target time or volume reached
        if (elapsedBrewTime >= targetTime || currentVolume >= targetVolume) {
          if (!readBrewSwitch()) {
            currentState = STATE_IDLE;
            Serial.print("Shot complete - Time: ");
            Serial.print(elapsedBrewTime);
            Serial.print("s, Volume: ");
            Serial.print(currentVolume, 1);
            Serial.println("ml");
          }
        } else if (!readBrewSwitch()) {
          currentState = STATE_IDLE;
          Serial.println("Shot interrupted");
        }
        break;
        
      case STATE_STEAMING:
        currentState = STATE_IDLE;
        Serial.println("Returning to IDLE state");
        break;
        
      case STATE_MENU:
        // Handled by encoder
        break;
    }
  }
}

void updateOutputs() {
  if (currentState == STATE_HEATING || currentState == STATE_BREWING || currentState == STATE_STEAMING || currentState == STATE_PRE_INFUSION) {
    // PID controlled heating
    setSsrOutput(controlHeating(currentTemp, targetTemp));
  } else {
    setSsrOutput(0);
  }
  
  if (currentState == STATE_PRE_INFUSION) {
    // Low pressure pre-infusion pump
    setPumpPressure(PREINFUSION_POT_VALUE);
  } else if (currentState == STATE_BREWING) {
    // Control pump pressure with MCP4131 for espresso
    setPumpPressure(controlPressure(currentPressure, targetPressure));
  } else if (currentState == STATE_STEAMING) {
    // Full pump pressure for steaming (15 bars = 255)
    setPumpPressure(255);
  } else {
    setPumpPressure(0);
  }
}

// Temperature control using PID
float controlHeating(float current, float target) {
  static float lastError = 0;
  static float integral = 0;
  
  float error = target - current;
  integral += error * (millis() / 1000.0);
  integral = constrain(integral, 0, 255);
  
  float derivative = error - lastError;
  lastError = error;
  
  float output = (KP * error) + (KI * integral) + (KD * derivative);
  return constrain(output, 0, 255);
}

// Pressure control
float controlPressure(float current, float target) {
  float error = target - current;
  float output = map(constrain(error + target, 0, 15), 0, 15, 0, 255);
  return output;
}

// Menu/Settings adjustment helpers
void adjustTargetTemp(int delta) {
  targetTemp = constrain(targetTemp + delta * 0.5, MIN_TEMP, MAX_TEMP);
  Serial.print("Target Temp: ");
  Serial.print(targetTemp, 1);
  Serial.println("°C");
}

void adjustTargetPressure(int delta) {
  targetPressure = constrain(targetPressure + delta * 0.1, MIN_PRESSURE, MAX_PRESSURE);
  Serial.print("Target Pressure: ");
  Serial.print(targetPressure, 1);
  Serial.println("bar");
}

void adjustTargetTime(int delta) {
  targetTime = constrain((long)targetTime + delta, MIN_TIME, MAX_TIME);
  Serial.print("Target Time: ");
  Serial.print(targetTime);
  Serial.println("s");
}

void adjustTargetVolume(int delta) {
  targetVolume = constrain(targetVolume + delta, MIN_VOLUME, MAX_VOLUME);
  Serial.print("Target Volume: ");
  Serial.print(targetVolume, 1);
  Serial.println("ml");
}

// Pre-infusion adjustment helpers
void togglePreInfusion() {
  preInfusionEnabled = preInfusionEnabled ? 0 : 1;
  Serial.print("Pre-Infusion: ");
  Serial.println(preInfusionEnabled ? "ON" : "OFF");
}

void adjustPreInfusionTime(int delta) {
  // Adjust by 1 second increments, range 2-10 seconds
  preInfusionTime = constrain(preInfusionTime + delta, 2, 10);
  Serial.print("Pre-Infusion Time: ");
  Serial.print(preInfusionTime);
  Serial.println("s");
}

// Menu input processing
void processMenuInput() {
  int delta = getEncoderDelta();
  
  if (delta != 0) {
    if (isMenuActive()) {
      adjustMenuValue(delta);
    }
  }
  
  if (isEncoderButtonPressed()) {
    if (isMenuActive()) {
      cycleMenu();
    } else {
      // Long press or another trigger could enter menu
      // For now, just print for debugging
    }
  }
}
