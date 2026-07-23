#ifndef STRINGS_H
#define STRINGS_H

// All strings stored in Flash (PROGMEM) instead of RAM
// Use F() macro to reference them: Serial.println(F(STR_READY));

#define STR_READY                   "READY"
#define STR_BREWING                 "BREWING"
#define STR_PREINFUSE               "PRE-INFUSE"
#define STR_STEAM_MODE              "STEAM MODE"
#define STR_MENU                    "MENU"
#define STR_BREWING_MODE            "BREWING MODE"
#define STR_TARGET                  "Target:"
#define STR_PUMP                    "Pump:"
#define STR_STOP_TIMER              "Stop on Timer"
#define STR_MANUAL_STOP             "Manual Stop"
#define STR_MANUAL                  "MANUAL"
#define STR_ROTATE_LEFT             "Rotate LEFT:"
#define STR_BREW_MODE               "-> Brewing Mode"
#define STR_ROTATE_RIGHT            "Rotate RIGHT:"
#define STR_SETTINGS                "-> Settings"
#define STR_MANUAL_MODE             "MANUAL STOP"
#define STR_AUTO_STOP               "AUTO-STOP"
#define STR_TIME_S                  "Time: "
#define STR_PRESSURE_B              "Pressure: "
#define STR_PREINFUSION             "PRE-INFUSION"
#define STR_ELAPSED                 "Elapsed: "
#define STR_TIMER                   "Timer: "
#define STR_BREW_TEMP               "Brew Temp"
#define STR_BREW_PRESSURE           "Brew Pressure"
#define STR_BREW_TIME               "Brew Time"
#define STR_PREINFUSION_ENABLED     "Pre-infusion"
#define STR_STEAM_TEMP              "Steam Temp"
#define STR_DEGREES_C               "\xF8C"  // Degree symbol

#endif
