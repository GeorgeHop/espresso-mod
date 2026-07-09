#ifndef CONFIG_H
#define CONFIG_H

// ===== PIN CONFIGURATION =====
// Thermocouple (MAX31855)
#define MAX31855_CS_PIN         10
#define MAX31855_CLK_PIN        13
#define MAX31855_MOSI_PIN       11
#define MAX31855_MISO_PIN       12

// Pressure Sensor (HX710B)
#define HX710B_DOUT_PIN         4
#define HX710B_CLK_PIN          5

// Flow Meter (YF-S201)
#define FLOW_METER_PIN          2    // Interrupt pin

// Rotary Encoder
#define ENCODER_CLK_PIN         6
#define ENCODER_DT_PIN          7
#define ENCODER_SW_PIN          8

// Brew Switch
#define BREW_SWITCH_PIN         A0

// Steam Switch
#define STEAM_SWITCH_PIN        A1

// Outputs
#define SSR_PIN                 3    // SSR for heating element (PWM)
#define MCP4131_CS_PIN          A2   // MCP4131 chip select
#define MCP4131_CLK_PIN         A3   // MCP4131 clock
#define MCP4131_MOSI_PIN        A4   // MCP4131 data

// TFT Display (SPI based - adjust for your display type)
#define TFT_CS_PIN              A5
#define TFT_DC_PIN              A6
#define TFT_RST_PIN             A7

// ===== SENSOR CALIBRATION =====
// Thermocouple calibration
#define TEMP_OFFSET             0.0      // °C offset for calibration
#define TEMP_SCALE              1.0      // Scale factor

// Pressure sensor calibration (HX710B)
#define PRESSURE_SCALE          0.0001   // ADC to bar conversion
#define PRESSURE_OFFSET         0.0      // bar offset
#define PRESSURE_SMOOTHING      10       // Number of samples for averaging

// Flow meter calibration (YF-S201)
#define FLOW_METER_PPL          450      // Pulses per liter for YF-S201
#define FLOW_SMOOTHING_FACTOR   0.7      // EMA smoothing (0-1)

// ===== CONTROL PARAMETERS =====
// Temperature control PID
#define KP                      2.5      // Proportional gain
#define KI                      0.5      // Integral gain
#define KD                      0.1      // Derivative gain
#define TEMP_HYSTERESIS         1.0      // °C hysteresis

// Pre-infusion parameters
#define PREINFUSION_ENABLED     1        // 1 = enabled, 0 = disabled
#define PREINFUSION_TIME        5        // seconds
#define PREINFUSION_PRESSURE    2.5      // bar (low pressure for saturation)
#define PREINFUSION_POT_VALUE   80       // digital pot value (approx 2.5 bar)

// Pressure control
#define STEAM_PRESSURE_VALUE    100      // Digital pot value for steam

// ===== UPDATE INTERVALS (milliseconds) =====
#define TEMP_UPDATE_INTERVAL    500
#define PRESSURE_UPDATE_INTERVAL 200
#define FLOW_UPDATE_INTERVAL    100
#define DISPLAY_UPDATE_INTERVAL 500

// ===== DISPLAY SETTINGS =====
#define TFT_WIDTH               240
#define TFT_HEIGHT              320
#define DISPLAY_TYPE            ILI9341  // Change based on your display

// ===== DEFAULT VALUES =====
#define DEFAULT_TARGET_TEMP     92.0     // °C
#define DEFAULT_TARGET_PRESSURE 9.0      // Bar
#define DEFAULT_TARGET_TIME     30       // seconds (typical espresso shot)
#define DEFAULT_TARGET_VOLUME   30       // ml (typical espresso shot)
#define STEAM_TARGET_TEMP       120.0    // °C for building steam
#define STEAM_TARGET_PRESSURE   15.0     // Bar for steam wand pressure
#define MIN_TEMP                50.0
#define MAX_TEMP                130.0
#define MIN_PRESSURE            6.0
#define MAX_PRESSURE            15.0
#define MIN_TIME                5        // seconds
#define MAX_TIME                60       // seconds
#define MIN_VOLUME              10       // ml
#define MAX_VOLUME              100      // ml

#endif
