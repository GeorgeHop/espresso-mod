/*
 * Espresso Machine Mod Controller - Machine-Driven Control
 * Arduino Nano with integrated temperature and pressure control
 * 
 * Control Logic:
 * - BREW SWITCH: Controls pump directly (physical switch on machine)
 * - STEAM SWITCH: Activates steam mode (physical switch on machine)
 * - ENCODER: Navigation and menu control, adjustable steam pressure
 * - AUTO-SHUTOFF: 20 minutes inactivity timeout with manual reactivation
 * 
 * Hardware:
 * - MAX6675 Thermocouple (K-type, up to 800°C)
 * - 5V G1/4 Water Pressure Sensor (0-10 bar)
 * - Rotary Encoder with button for menu navigation and settings
 * - TFT IPS 1.69" ST7789V Display (240x280)
 * - SSR 40A DC-AC for heating element
 * - PWM AC220V Controller (YYAC-3S) for pump speed/pressure control
 * - 2 switches: Espresso (Ulka pump) and Steam (thermoblock)
 * - Manual power switch for machine on/off
 */

#include "config.h"
#include "thermocouple.h"
#include "pressure_sensor.h"
#include "encoder.h"
#include "display.h"
#include "output_control.h"
#include "menu.h"

// ===== STATE MACHINE =====
enum State {
  STATE_HOME = 0,                   // Home/idle screen
  STATE_PORTAFILTER_PREHEAT = 1,    // Preheating portafilter with 5sec flush
  STATE_BREWING = 2,                // Brew switch pressed, extracting
  STATE_STEAMING = 3,               // Steam switch pressed
  STATE_BREWING_MODE = 4,           // Brewing mode selection (manual/auto-stop)
  STATE_SHUTDOWN = 5                // Machine in low-power shutdown (inactivity timeout)
};

State currentState = STATE_HOME;

// ===== SETTINGS =====
float brewTemp = DEFAULT_TARGET_TEMP;
byte brewTime = DEFAULT_TARGET_TIME;
float brewPressure = PUMP_DEFAULT_BREW_PRESSURE;
float steamTemp = STEAM_TARGET_TEMP;
float steamPressure = PUMP_DEFAULT_STEAM_PRESSURE;
bool pumpStopOnTimeout = PUMP_STOP_ON_TIMEOUT;

// ===== SENSOR READINGS =====
float currentTemp = 0.0;
float currentPressure = 0.0;
float targetTemp;  // Set by state machine based on mode

// ===== BREW TRACKING =====
unsigned long brewStartTime = 0;
unsigned int brewElapsedTime = 0;
bool brewTimerActive = false;
float brewEstimatedVolume = 0.0;  // Estimated volume (ml) based on flow rate and time

// ===== PORTAFILTER PREHEAT =====
bool preheatDoneThisSession = false;
unsigned long preheatStartTime = 0;
bool preheatActive = false;
bool waterDetected = false;              // Flag to track if water pressure detected during preheat
unsigned long waterCheckStartTime = 0;  // Time when preheat started (for water pressure check)
bool preheatError = false;              // Flag to display preheat error message
unsigned long preheatErrorTime = 0;     // Time when error occurred (to auto-clear after 5 seconds)

// ===== SWITCH STATES =====
bool powerSwitchOn = false;     // Power switch position (UP=ON, DOWN=OFF)
bool lastPowerState = false;    // Track power switch state changes
bool brewSwitchOn = false;      // TODEA latching switch (D5)
bool steamSwitchOn = false;     // SW312 click button on steam wand (D6)
bool lastBrewState = false;
bool lastSteamState = false;

// ===== AUTO-SHUTOFF & ACTIVITY TRACKING =====
bool machineShutdown = false;  // Flag for machine in shutdown state
unsigned long lastActivityCheck = 0;

// ===== WAKE-UP TIMER (counts down from boot time) =====
bool wakeupTimerEnabled = false;
unsigned long wakeupTimeMillis = 0;  // Wake-up time in milliseconds from boot
unsigned long bootTimeMillis = 0;    // Time when Arduino started

// ===== ENCODER STATES =====
unsigned long lastMenuActivity = 0;  // For auto-close timer

// ===== TIMING =====
unsigned long lastTempUpdate = 0;
unsigned long lastPressureUpdate = 0;
unsigned long lastSwitchCheck = 0;
unsigned long lastPidUpdate = 0;

// ===== PID CONTROL =====
float pidError = 0.0;
float pidIntegral = 0.0;
float pidDerivative = 0.0;
float lastError = 0.0;

// ===== FORWARD DECLARATIONS =====
void updateStateMachine();
void checkSwitches();
void handleEncoderInput();
void updateHeatingControl();
void renderDisplay();
void checkInactivityTimeout();
void reactivateMachine();

void setup() {
  delay(500);
  
  // Initialize all components
  initDisplay();
  delay(100);
  
  initThermocouple();
  delay(100);
  
  initPressureSensor();
  delay(100);
  
  initEncoder();
  delay(100);
  
  // Enable internal pull-ups for brew and steam switches
  pinMode(BREW_SWITCH_PIN, INPUT_PULLUP);
  pinMode(STEAM_SWITCH_PIN, INPUT_PULLUP);
  delay(100);
  
  initOutputControl();
  delay(100);
  
  initMenu();
  delay(100);
  
  delay(100);
  
  targetTemp = brewTemp;
  lastPidUpdate = millis();
  lastActivityTime = millis();  // Initialize activity timer
  lastActivityCheck = millis();
  bootTimeMillis = millis();    // Record boot time for wake-up timer
  
  // Set initial pump pressure for both modes
  setPumpBrewPressure(brewPressure);
  setPumpSteamPressure(steamPressure);
  
  Serial.println(F("[Setup] Espresso Controller initialized - 20min auto-shutoff enabled"));
  Serial.print(F("[Setup] Brew Pressure: "));
  Serial.print(brewPressure);
  Serial.print(F(" bar, Steam Pressure: "));
  Serial.print(steamPressure);
  Serial.println(F(" bar"));
  Serial.println(F("[Setup] Wake-up timer available - use menu to set"));
}

void loop() {
  unsigned long currentMillis = millis();
  
  // Update sensor readings at intervals
  if (currentMillis - lastTempUpdate >= TEMP_UPDATE_INTERVAL) {
    currentTemp = readThermocouple();
    lastTempUpdate = currentMillis;
  }
  
  if (currentMillis - lastPressureUpdate >= PRESSURE_UPDATE_INTERVAL) {
    currentPressure = readPressureSensor();
    lastPressureUpdate = currentMillis;
  }
  
  // Check switches
  checkSwitches();
  
  // Handle encoder input
  handleEncoderInput();
  
  // Check for inactivity timeout
  checkInactivityTimeout();
  
  // Check wake-up timer
  checkWakeupTimer();
  
  // Update state machine
  updateStateMachine();
  
  // Control heating based on current state
  updateHeatingControl();
  
  // Update display (display controller handles refresh rate internally)
  renderDisplay();
  
  // Auto-close mode screen if inactive
  if (currentState == STATE_BREWING_MODE && (currentMillis - lastMenuActivity) > MENU_TIMEOUT_MS) {
    currentState = STATE_HOME;
  }
}

// ===== STATE MACHINE =====
void updateStateMachine() {
  unsigned long currentMillis = millis();
  
  // Don't process state changes if machine is shutdown
  if (machineShutdown && currentState != STATE_SHUTDOWN) {
    currentState = STATE_SHUTDOWN;
    setSsrOutput(0);
    setPumpOutput(0);
    return;
  }
  
  // Check for automatic portafilter preheat on boot/reactivation
  if (PORTAFILTER_PREHEAT_ENABLED && !preheatDoneThisSession && 
      currentState == STATE_HOME && currentTemp >= brewTemp - 1.0) {
    // Thermoblock reached target temp and preheat not done yet
    currentState = STATE_PORTAFILTER_PREHEAT;
    preheatStartTime = currentMillis;
    waterCheckStartTime = currentMillis;  // Start water pressure check timer
    preheatActive = true;
    waterDetected = false;  // Reset water detection flag
    setPumpPressure(PORTAFILTER_PREHEAT_PRESSURE);
    updateActivityTime();
    Serial.println(F("[Preheat] Starting portafilter flush (5s at 6.0 bar, checking for water..."));
    return;
  }
  
  // Handle preheat countdown
  if (currentState == STATE_PORTAFILTER_PREHEAT) {
    unsigned long preheatElapsed = currentMillis - waterCheckStartTime;
    
    // Check for water pressure after 1 second delay
    if (!waterDetected && preheatElapsed >= WATER_PRESSURE_CHECK_DELAY) {
      if (currentPressure >= WATER_PRESSURE_THRESHOLD) {
        // Water detected - pressure is building
        waterDetected = true;
        Serial.println(F("[Preheat] Water detected - pressure OK"));
      } else {
        // No water - pressure too low
        preheatActive = false;
        preheatDoneThisSession = true;
        waterDetected = false;
        preheatError = true;
        preheatErrorTime = currentMillis;
        setPumpOutput(0);
        currentState = STATE_HOME;
        updateActivityTime();
        Serial.println(F("[Preheat] NO WATER DETECTED - Tank empty?"));
        return;
      }
    }
    
    // Check if preheat duration complete (only if water was detected)
    if (waterDetected && preheatElapsed >= PORTAFILTER_PREHEAT_DURATION) {
      // Preheat complete
      preheatActive = false;
      preheatDoneThisSession = true;
      waterDetected = false;
      setPumpOutput(0);
      currentState = STATE_HOME;
      updateActivityTime();
      Serial.println(F("[Preheat] Portafilter flush complete - ready for brewing"));
    }
    return;  // Stay in preheat, don't check other switches
  }
  
  // Check if brew switch state changed
  if (brewSwitchOn && !lastBrewState) {
    if (!steamSwitchOn && !machineShutdown) {
      // INTERLOCK: Brew can only activate if steam is OFF
      // If in preheat, skip it and go straight to brewing
      if (currentState == STATE_PORTAFILTER_PREHEAT) {
        preheatActive = false;
        preheatDoneThisSession = true;
        setPumpOutput(0);
        Serial.println(F("[Preheat] Interrupted by brew switch - starting extraction"));
      }
      
      currentState = STATE_BREWING;
      brewStartTime = currentMillis;
      brewElapsedTime = 0;
      brewEstimatedVolume = 0.0;
      brewTimerActive = true;
      targetTemp = brewTemp;
      // Apply brew pressure pump control
      setPumpBrewPressure(brewPressure);
      updateActivityTime();  // Track activity
    }
  } else if (!brewSwitchOn && lastBrewState) {
    if (currentState == STATE_BREWING) {
      brewTimerActive = false;
      setSsrOutput(0);
    }
  }
  
  // Check if steam switch state changed
  if (steamSwitchOn && !lastSteamState) {
    if (!brewSwitchOn && !machineShutdown) {
      // INTERLOCK: Steam can only activate if brew is OFF
      // If in preheat, skip it and go to steam
      if (currentState == STATE_PORTAFILTER_PREHEAT) {
        preheatActive = false;
        preheatDoneThisSession = true;
        setPumpOutput(0);
        Serial.println(F("[Preheat] Interrupted by steam switch - entering steam mode"));
      }
      
      currentState = STATE_STEAMING;
      targetTemp = steamTemp;
      // Apply steam pressure pump control
      setPumpSteamPressure(steamPressure);
      updateActivityTime();  // Track activity
    }
  } else if (!steamSwitchOn && lastSteamState) {
    if (currentState == STATE_STEAMING) {
      currentState = STATE_HOME;
      setSsrOutput(0);
      setPumpOutput(0);
    }
  }
  
  lastBrewState = brewSwitchOn;
  lastSteamState = steamSwitchOn;
  
  // Handle brewing timer
  if (brewTimerActive && brewSwitchOn) {
    brewElapsedTime = (currentMillis - brewStartTime) / 1000;
    // Calculate estimated volume based on flow rate and elapsed time
    brewEstimatedVolume = brewElapsedTime * PUMP_FLOW_RATE;
    if (brewElapsedTime >= brewTime && pumpStopOnTimeout) {
      brewTimerActive = false;
      setSsrOutput(0);
    }
  }
}

// ===== SWITCH HANDLING =====
void checkSwitches() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastSwitchCheck < 20) return;
  lastSwitchCheck = currentMillis;
  
  // Read power switch (A3): HIGH = ON, LOW = OFF
  bool newPowerState = digitalRead(POWER_SWITCH_PIN) == HIGH;
  bool newBrewState = digitalRead(BREW_SWITCH_PIN) == LOW;
  bool newSteamState = digitalRead(STEAM_SWITCH_PIN) == LOW;
  
  // Handle power switch state changes
  if (newPowerState != powerSwitchOn) {
    powerSwitchOn = newPowerState;
    
    if (!powerSwitchOn) {
      // Power switch turned OFF - shut down machine immediately
      Serial.println(F("[Power] Switch turned OFF - shutting down"));
      setRelayOutput(false);  // Cut relay
      setSsrOutput(0);        // Stop heating
      setPumpOutput(0);       // Stop pump
      machineShutdown = true;
      currentState = STATE_SHUTDOWN;
    } else {
      // Power switch turned ON - machine will start preheat sequence
      Serial.println(F("[Power] Switch turned ON - reactivating"));
      preheatDoneThisSession = false;  // Reset preheat flag
      setRelayOutput(true);             // Activate relay - machine gets power
      machineShutdown = false;
      currentState = STATE_HOME;
      updateActivityTime();
    }
    lastPowerState = newPowerState;
    return;  // Skip other switch processing if power changed
  }
  
  // Only process brew/steam switches if power is ON
  if (!powerSwitchOn) {
    return;  // Ignore brew/steam switches when power is off
  }
  
  // Track activity when switch changes
  if (newBrewState != brewSwitchOn || newSteamState != steamSwitchOn) {
    updateActivityTime();  // Update activity timer
    if (!machineShutdown && newBrewState) {
      // User activated brew - reset shutdown flag
      machineShutdown = false;
    }
  }
  
  brewSwitchOn = newBrewState;
  steamSwitchOn = newSteamState;
}

// ===== HEATING CONTROL (PID) =====
void updateHeatingControl() {
  unsigned long currentMillis = millis();
  float deltaTime = (currentMillis - lastPidUpdate) / 1000.0;
  lastPidUpdate = currentMillis;
  
  // Immediately shut down if machine is in shutdown state
  if (machineShutdown || currentState == STATE_SHUTDOWN) {
    setSsrOutput(0);
    setPumpOutput(0);
    pidIntegral = 0;
    return;
  }
  
  // Only control when in brewing or steaming (not in mode selection)
  if (currentState == STATE_HOME || currentState == STATE_BREWING_MODE) {
    setSsrOutput(0);
    pidIntegral = 0;
    return;
  }
  
  // In steam mode, run at high power to reach steam temp
  if (currentState == STATE_STEAMING) {
    if (currentTemp >= steamTemp - TEMP_HYSTERESIS) {
      setSsrOutput(255);  // Full power to maintain steam
    } else {
      setSsrOutput(200);  // Ramp up to steam temp
    }
    return;
  }
  
  // PID for brewing
  pidError = targetTemp - currentTemp;
  pidIntegral += pidError * deltaTime;
  pidIntegral = constrain(pidIntegral, -50, 50);  // Anti-windup
  pidDerivative = (pidError - lastError) / deltaTime;
  lastError = pidError;
  
  float pidOutput = (KP * pidError) + (KI * pidIntegral) + (KD * pidDerivative);
  byte ssrPwm = constrain((int)pidOutput, 0, 255);
  
  setSsrOutput(ssrPwm);
}

// ===== ENCODER INPUT =====
void handleEncoderInput() {
  int delta = getEncoderDelta();
  
  if (delta != 0) {
    lastMenuActivity = millis();
    updateActivityTime();  // Track activity
    preheatError = false;  // Clear any preheat error on encoder input
    
    // If machine is shutdown, reactivate on encoder input
    if (machineShutdown && currentState == STATE_SHUTDOWN) {
      reactivateMachine();
      return;
    }
    
    if (currentState == STATE_HOME) {
      if (delta < 0) {
        currentState = STATE_BREWING_MODE;
      }
    } else if (currentState == STATE_BREWING_MODE) {
      // Check if we're in pressure edit mode
      MenuState menuState = getCurrentMenuSelection();
      
      if (menuState == MENU_BREW_PRESSURE || menuState == MENU_STEAM_PRESSURE) {
        if (isInEditMode()) {
          // Adjust pressure in edit mode
          if (menuState == MENU_BREW_PRESSURE) {
            adjustBrewPressure(delta);
          } else {
            adjustSteamPressure(delta);
          }
        } else {
          // Navigate menu when not editing
          if (delta > 0) {
            advanceMenuSelection();
          } else {
            reverseMenuSelection();
          }
        }
      } else {
        // Navigate menu for other settings
        if (delta > 0) {
          advanceMenuSelection();
        } else {
          reverseMenuSelection();
        }
      }
    }
  }
  
  // Check encoder button press
  if (isEncoderButtonPressed()) {
    updateActivityTime();  // Track activity
    preheatError = false;  // Clear any preheat error on button press
    
    if (machineShutdown && currentState == STATE_SHUTDOWN) {
      reactivateMachine();
      return;
    }
    
    if (currentState == STATE_BREWING_MODE) {
      MenuState menuState = getCurrentMenuSelection();
      
      if (menuState == MENU_BREW_PRESSURE) {
        if (!isInEditMode()) {
          enterEditMode();
        } else {
          commitBrewPressureChange();
          exitEditMode();
        }
      } else if (menuState == MENU_STEAM_PRESSURE) {
        if (!isInEditMode()) {
          enterEditMode();
        } else {
          commitSteamPressureChange();
          exitEditMode();
        }
      }
    }
  }
}

// ===== DISPLAY RENDERING =====
void renderDisplay() {
  if (machineShutdown && currentState == STATE_SHUTDOWN) {
    displayShutdown(currentTemp);  // Show shutdown screen
    return;
  }
  
  // Check if preheat error should be displayed
  if (preheatError) {
    unsigned long timeSinceError = millis() - preheatErrorTime;
    // Auto-clear error after 5 seconds or on any user input
    if (timeSinceError > 5000) {
      preheatError = false;
    } else {
      displayPreheatError(F("NO WATER DETECTED"));
      return;
    }
  }
  
  if (currentState == STATE_HOME || currentState == STATE_PORTAFILTER_PREHEAT) {
    // Show home screen, possibly with preheat countdown
    unsigned long preheatCountdown = 0;
    if (currentState == STATE_PORTAFILTER_PREHEAT) {
      unsigned long preheatElapsed = millis() - preheatStartTime;
      preheatCountdown = (PORTAFILTER_PREHEAT_DURATION - preheatElapsed) / 1000;
    }
    displayHome(currentTemp, brewTemp, pumpStopOnTimeout, preheatCountdown, powerSwitchOn);
  } else if (currentState == STATE_BREWING_MODE) {
    // Display menu with pressure adjustment options
    MenuState menuState = getCurrentMenuSelection();
    if (menuState == MENU_BREW_PRESSURE) {
      displayBrewPressureMenu(getTempBrewPressure(), isInEditMode());
    } else if (menuState == MENU_STEAM_PRESSURE) {
      displaySteamPressureMenu(getTempSteamPressure(), isInEditMode());
    } else {
      displayBrewingMode(pumpStopOnTimeout);
    }
  } else if (currentState == STATE_BREWING) {
    displayBrewing(currentTemp, brewTemp, currentPressure, 9.0, brewElapsedTime, brewTime, false, brewEstimatedVolume);
  } else if (currentState == STATE_STEAMING) {
    displaySteamingMode(currentTemp, steamTemp, true);
  }
}

// ===== INACTIVITY & AUTO-SHUTOFF =====
void checkInactivityTimeout() {
  unsigned long currentMillis = millis();
  
  // Only check periodically to save cycles
  if (currentMillis - lastActivityCheck < ACTIVITY_CHECK_INTERVAL) {
    return;
  }
  lastActivityCheck = currentMillis;
  
  unsigned long timeSinceActivity = getTimeSinceLastActivity();
  
  // Check if machine should shut down due to inactivity
  if (!machineShutdown && timeSinceActivity >= INACTIVITY_TIMEOUT) {
    // Trigger shutdown
    machineShutdown = true;
    currentState = STATE_SHUTDOWN;
    setSsrOutput(0);
    setPumpOutput(0);
    setRelayOutput(false);  // Cut relay - machine powers down
    
    Serial.println(F("[Inactivity] Machine entering low-power shutdown after 20 minutes"));
    Serial.println(F("[Inactivity] Toggle manual power switch (down then up) to reactivate"));
  }
}

// Reactivate machine from shutdown state
void reactivateMachine() {
  if (!machineShutdown) {
    return;  // Already active
  }
  
  machineShutdown = false;
  currentState = STATE_HOME;
  preheatDoneThisSession = false;  // Reset preheat flag for new session
  updateActivityTime();  // Reset activity timer
  
  Serial.println(F("[Reactivation] Machine reactivated - heating enabled"));
  Serial.println(F("[Reactivation] Activity timer reset to 20 minutes"));
}

// ===== WAKE-UP TIMER =====
void checkWakeupTimer() {
  unsigned long currentMillis = millis();
  
  // Check if wake-up timer is enabled and time has arrived
  if (wakeupTimerEnabled && currentMillis >= wakeupTimeMillis) {
    // Time to wake up!
    wakeupTimerEnabled = false;  // Disable timer (one-time activation)
    
    // Reactivate machine from shutdown if needed
    if (machineShutdown) {
      reactivateMachine();
      Serial.println(F("[WakeUp] Machine activated by wake-up timer!"));
    } else {
      // Machine was already on, just make sure heating is active
      currentState = STATE_HOME;
      Serial.println(F("[WakeUp] Wake-up timer triggered - machine ready"));
    }
  }
}

// Set wake-up timer (hours from now)
void setWakeupTimer(byte hoursFromNow) {
  if (hoursFromNow == 0) {
    // Disable timer
    wakeupTimerEnabled = false;
    Serial.println(F("[WakeUp] Wake-up timer disabled"));
  } else if (hoursFromNow <= WAKEUP_MAX_HOURS) {
    // Calculate wake-up time in milliseconds
    wakeupTimeMillis = millis() + (unsigned long)hoursFromNow * 3600000UL;  // Convert hours to ms
    wakeupTimerEnabled = true;
    
    Serial.print(F("[WakeUp] Timer set for "));
    Serial.print(hoursFromNow);
    Serial.println(F(" hour(s) from now"));
  }
}

