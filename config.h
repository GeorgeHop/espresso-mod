#ifndef CONFIG_H
#define CONFIG_H

// ===== PIN CONFIGURATION =====
// Thermocouple (MAX6675) - K-type, up to 800°C
#define MAX6675_CS_PIN          10
#define MAX6675_CLK_PIN         13
#define MAX6675_MISO_PIN        12

// Pressure Sensor (5V G1/4 Water Pressure)
#define PRESSURE_SENSOR_PIN     A0   // Analog input for pressure

// Rotary Encoder with Button
#define ENCODER_CLK_PIN         2    // INT0
#define ENCODER_DT_PIN          3    // INT1
#define ENCODER_SW_PIN          4

// Switches (2 switches we're using)
#define BREW_SWITCH_PIN         5    // Espresso pump switch (Ulka)
#define STEAM_SWITCH_PIN        6    // Steam thermoblock switch

// Outputs
#define SSR_PIN                 9    // SSR for heating element (PWM on Arduino Nano)
#define PUMP_PWM_PIN            A2   // PWM controller for AC pump (0-5V analog signal)

// TFT Display ST7789V (SPI based)
#define TFT_CS_PIN              11
#define TFT_DC_PIN              8
#define TFT_RST_PIN             7
#define TFT_MOSI_PIN            12   // Standard SPI
#define TFT_CLK_PIN             13   // Standard SPI

// ===== SENSOR CALIBRATION =====
// Thermocouple calibration (MAX6675)
#define TEMP_OFFSET             0.0      // °C offset for calibration
#define TEMP_SCALE              1.0      // Scale factor

// Pressure sensor calibration (5V analog)
// Typically: 4-20mA sensor with 0-10 bar range
// ADC reading 0-1023 maps to pressure
#define PRESSURE_MIN_ADC        102      // ~0.5V (0 bar) - adjust for your sensor
#define PRESSURE_MAX_ADC        921      // ~4.5V (10 bar) - adjust for your sensor
#define PRESSURE_MIN_BAR        0.0      // Minimum pressure (bar)
#define PRESSURE_MAX_BAR        10.0     // Maximum pressure (bar)
#define PRESSURE_SMOOTHING      8        // Number of samples for averaging

// ===== CONTROL PARAMETERS =====
// Temperature control PID
#define KP                      3.0      // Proportional gain
#define KI                      0.8      // Integral gain
#define KD                      0.15     // Derivative gain
#define TEMP_HYSTERESIS         1.5      // °C hysteresis

// Pre-infusion parameters
#define PREINFUSION_ENABLED     1        // 1 = enabled, 0 = disabled
#define PREINFUSION_TIME        5        // seconds
#define PREINFUSION_PRESSURE    2.5      // bar (low pressure for saturation)
#define PREINFUSION_SSR_VALUE   100      // SSR PWM value for pre-infusion (~1.5 bar equiv)

// SSR Heating Control
#define SSR_MIN_VALUE           0        // Minimum SSR value
#define SSR_MAX_VALUE           255      // Maximum SSR PWM value

// ===== UPDATE INTERVALS (milliseconds) =====
#define TEMP_UPDATE_INTERVAL    500
#define PRESSURE_UPDATE_INTERVAL 200
#define DISPLAY_UPDATE_INTERVAL 500

// ===== DISPLAY SETTINGS =====
#define TFT_WIDTH               240
#define TFT_HEIGHT              280     // 1.69" IPS typically has 240x280 pixels
#define DISPLAY_TYPE            ST7789V

// ===== DEFAULT VALUES =====
#define DEFAULT_TARGET_TEMP     92.0     // °C for espresso
#define DEFAULT_TARGET_PRESSURE 9.0      // Bar for espresso extraction
#define DEFAULT_TARGET_TIME     30       // seconds (typical espresso shot)
#define STEAM_TARGET_TEMP       130.0    // °C for steam mode
#define STEAM_TARGET_PRESSURE   0.0      // N/A for steam (manual control)
#define MIN_TEMP                75.0     // Safety: minimum safe temperature
#define MAX_TEMP                140.0    // Safety: maximum temperature
#define MIN_PRESSURE            0.0      // Minimum pressure
#define MAX_PRESSURE            15.0     // Maximum pressure (SSR safety limit)
#define MIN_TIME                5        // seconds
#define MAX_TIME                120      // seconds (extended for pre-infusion + brew)

// ===== SWITCH DEBOUNCE =====
#define SWITCH_DEBOUNCE_MS      20       // Milliseconds to debounce switches

// ===== MENU & SETTINGS =====
#define PUMP_STOP_ON_TIMEOUT    1        // 1 = stop pump at time end, 0 = keep pumping
#define ENCODER_DOUBLE_CLICK_MS 500      // Milliseconds to detect double click
#define MENU_TIMEOUT_MS         10000    // Return to home after 10s inactivity

// ===== AUTO-SHUTOFF & ACTIVITY TRACKING =====
#define INACTIVITY_TIMEOUT      1200000  // 20 minutes (1200000 ms) - auto-shutoff time
#define ACTIVITY_CHECK_INTERVAL 100      // Check activity every 100ms
#define MACHINE_SHUTDOWN_THRESHOLD 1200000 // Time before full shutdown

// ===== PUMP CONTROL (PWM AC220V Controller) =====
#define PUMP_MIN_VALUE          0        // Minimum pump PWM (0%)
#define PUMP_MAX_VALUE          255      // Maximum pump PWM (100%)
#define PUMP_DEFAULT_BREW_PRESSURE 9.0   // Default brew pressure (bar) - espresso extraction
#define PUMP_DEFAULT_STEAM_PRESSURE 12.0 // Default steam pressure (bar) - milk steaming
#define MIN_BREW_PRESSURE       2.0      // Minimum brew pressure (bar)
#define MAX_BREW_PRESSURE       15.0     // Maximum brew pressure (bar) - allows headroom
#define MIN_STEAM_PRESSURE      2.0      // Minimum steam pressure (bar)
#define MAX_STEAM_PRESSURE      15.0     // Maximum steam pressure (bar)

// ===== PORTAFILTER PREHEAT =====
#define PORTAFILTER_PREHEAT_ENABLED 1    // 1 = enabled, 0 = disabled
#define PORTAFILTER_PREHEAT_DURATION 5000 // Duration in milliseconds (5 seconds)
#define PORTAFILTER_PREHEAT_PRESSURE 6.0  // Flush pressure (bar) - gentle, not full brewing

// ===== EEPROM SETTINGS (for future persistence) =====
// These values can be stored in EEPROM and modified through menu
#define EEPROM_BREW_TEMP        0
#define EEPROM_BREW_PRESSURE    1
#define EEPROM_BREW_TIME        2
#define EEPROM_PREINFUSION_ENABLED 3
#define EEPROM_STEAM_TEMP       4
#define EEPROM_STEAM_PRESSURE   5
#define EEPROM_PUMP_STOP_MODE   6

#endif
