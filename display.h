#ifndef DISPLAY_H
#define DISPLAY_H

#include "config.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

/*
 * TFT Display Driver
 * ST7789V 1.69" IPS Display (240x280)
 * SPI-based, Adafruit library compatible
 * 
 * NOTE: Required libraries:
 * - Adafruit GFX Library
 * - Adafruit ST7789 Library
 */

// Display instance
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS_PIN, TFT_DC_PIN, TFT_RST_PIN);

// Color definitions for easy reference
#define TFT_BLACK       0x0000
#define TFT_RED         0xF800
#define TFT_GREEN       0x07E0
#define TFT_BLUE        0x001F
#define TFT_CYAN        0x07FF
#define TFT_MAGENTA     0xF81F
#define TFT_YELLOW      0xFFE0
#define TFT_WHITE       0xFFFF
#define TFT_ORANGE      0xFD20
#define TFT_DARKGREEN   0x0340

// Forward declarations
void displayInit();
void displayHome(float currentTemp, float brewTemp, bool pumpStopMode, unsigned long preheatCountdown = 0);
void displayBrewingMode(bool pumpStopMode);
void displayBrewing(float currentTemp, float targetTemp, float currentPressure, float targetPressure, unsigned long elapsed, unsigned long total, bool isPreInfusion);
void displaySteamingMode(float currentTemp, float targetTemp, bool brewActive);
void displayMenu();
void displaySteamMode(float currentTemp, float targetTemp);
void displayPreInfusion(float currentTemp, float targetTemp, float currentPressure, unsigned long elapsed, unsigned long total);
void displayHeating(float currentTemp, float targetTemp);
void displayIdle();

void initDisplay() {
  // Initialize ST7789 display
  tft.init(TFT_HEIGHT, TFT_WIDTH);  // 280x240 for portrait mode
  tft.setRotation(0);  // Portrait orientation
  tft.fillScreen(TFT_BLACK);
  
  Serial.println(F("[Display] ST7789V initialized (240x280)"));
  displayInit();
}

void displayInit() {
  // Show startup screen
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(30, 20);
  tft.println(F("ESPRESSO"));
  tft.setCursor(50, 45);
  tft.println(F("MOD"));
  
  tft.setTextSize(1);
  tft.setCursor(40, 100);
  tft.println(F("Initializing..."));
  
  delay(1500);
  tft.fillScreen(TFT_BLACK);
}

void displayHome(float currentTemp, float brewTemp, bool pumpStopMode, unsigned long preheatCountdown) {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  
  // Title
  tft.setTextSize(2);
  tft.setCursor(30, 15);
  tft.setTextColor(TFT_GREEN);
  tft.println(F("READY"));
  
  // Preheat countdown display (if active)
  if (preheatCountdown > 0) {
    tft.setTextSize(2);
    tft.setCursor(20, 40);
    tft.setTextColor(TFT_ORANGE);
    tft.println(F("PREHEAT"));
    
    tft.setTextSize(3);
    tft.setCursor(90, 65);
    tft.setTextColor(TFT_YELLOW);
    tft.print(preheatCountdown);
    tft.println(F("s"));
    
    // Current temperature
    tft.setTextSize(1);
    tft.setCursor(40, 120);
    tft.setTextColor(TFT_WHITE);
    tft.print(F("Flushing: "));
    tft.print(currentTemp, 1);
    tft.println((char)248);
  } else {
    // Current temperature
    tft.setTextSize(3);
    tft.setCursor(40, 60);
    tft.setTextColor(TFT_YELLOW);
    tft.print(currentTemp, 1);
    tft.print((char)248);
    tft.println(F("C"));
    
    // Target temp
    tft.setTextSize(1);
    tft.setCursor(60, 120);
    tft.setTextColor(TFT_WHITE);
    tft.print(F("Target: "));
    tft.print(brewTemp, 0);
    tft.println((char)248);
  }
  
  // Pump stop mode
  tft.setCursor(20, 160);
  tft.print(F("Pump: "));
  tft.setTextColor(pumpStopMode ? TFT_RED : TFT_GREEN);
  tft.println(pumpStopMode ? F("Stop on Timer") : F("Manual Stop"));
  
  // Instructions
  tft.setTextSize(1);
  tft.setTextColor(TFT_CYAN);
  tft.setCursor(15, 190);
  tft.println(F("Rotate LEFT:"));
  tft.setCursor(15, 205);
  tft.println(F("  -> Brewing Mode"));
  tft.setCursor(15, 220);
  tft.println(F("Rotate RIGHT:"));
  tft.setCursor(15, 235);
  tft.println(F("  -> Settings"));
}

void displayBrewingMode(bool pumpStopMode) {
  tft.fillScreen(TFT_BLACK);
  
  // Title
  tft.setTextSize(2);
  tft.setCursor(20, 15);
  tft.setTextColor(TFT_MAGENTA);
  tft.println(F("BREWING MODE"));
  
  // Current mode display (large)
  tft.setTextSize(2);
  tft.setCursor(15, 70);
  tft.setTextColor(pumpStopMode ? TFT_RED : TFT_GREEN);
  
  if (pumpStopMode) {
    tft.println(F("STOP ON TIMER"));
    tft.setCursor(20, 110);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE);
    tft.println(F("Pump auto-stops"));
    tft.setCursor(20, 125);
    tft.println(F("when timer expires"));
  } else {
    tft.println(F("MANUAL STOP"));
    tft.setCursor(20, 110);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE);
    tft.println(F("Pump runs until"));
    tft.setCursor(20, 125);
    tft.println(F("switch is released"));
  }
  
  // Instructions
  tft.setCursor(15, 170);
  tft.setTextSize(1);
  tft.setTextColor(TFT_CYAN);
  tft.println(F("Rotate RIGHT:"));
  tft.setCursor(15, 185);
  tft.println(F("  -> Toggle Mode"));
  
  tft.setCursor(15, 220);
  tft.setTextColor(TFT_YELLOW);
  tft.println(F("Rotate LEFT:"));
  tft.setCursor(15, 235);
  tft.println(F("  -> Back to Home"));
}

void displayBrewing(float currentTemp, float targetTemp, float currentPressure, 
                    float targetPressure, unsigned long elapsed, unsigned long total, bool isPreInfusion) {
  tft.fillScreen(TFT_BLACK);
  
  // Title
  tft.setTextSize(2);
  tft.setCursor(isPreInfusion ? 20 : 50, 10);
  tft.setTextColor(isPreInfusion ? TFT_CYAN : TFT_GREEN);
  tft.println(isPreInfusion ? F("PRE-INFUSE") : F("BREWING"));
  
  // Temperature line
  tft.setTextSize(1);
  tft.setCursor(10, 50);
  tft.setTextColor(TFT_WHITE);
  tft.print(F("T: "));
  tft.print(currentTemp, 1);
  tft.print((char)248);
  tft.print(F(" / "));
  tft.print(targetTemp, 0);
  tft.println((char)248);
  
  // Pressure line
  tft.setCursor(10, 70);
  tft.print(F("P: "));
  tft.print(currentPressure, 1);
  tft.print(F(" / "));
  tft.print(targetPressure, 1);
  tft.println(F(" bar"));
  
  // Time display (large)
  tft.setTextSize(3);
  tft.setCursor(40, 110);
  tft.setTextColor(TFT_YELLOW);
  tft.print(elapsed);
  tft.print(F("s"));
  
  tft.setTextSize(1);
  tft.setCursor(120, 125);
  tft.setTextColor(TFT_WHITE);
  tft.print(F("/ "));
  tft.print(total);
  tft.println(F("s"));
  
  // Progress bar
  tft.setCursor(10, 160);
  int percent = (elapsed * 100) / max(total, 1UL);
  int barWidth = (percent * 18) / 100;
  tft.print(F("["));
  for (int i = 0; i < 20; i++) {
    tft.print(i < barWidth ? (char)219 : (char)176);
  }
  tft.print(F("] "));
  tft.print(percent);
  tft.println(F("%"));
  
  // Status
  tft.setCursor(20, 200);
  tft.setTextColor(TFT_GREEN);
  tft.println(F("Extraction Active"));
  tft.setCursor(10, 220);
  tft.setTextColor(TFT_CYAN);
  tft.println(F("Release switch to stop"));
}

void displaySteamingMode(float currentTemp, float targetTemp, bool brewActive) {
  tft.fillScreen(TFT_BLACK);
  
  // Title
  tft.setTextSize(2);
  tft.setCursor(15, 10);
  tft.setTextColor(TFT_ORANGE);
  tft.println(F("STEAM MODE"));
  
  // Temperature (large)
  tft.setTextSize(3);
  tft.setCursor(40, 70);
  tft.setTextColor(TFT_RED);
  tft.print(currentTemp, 1);
  tft.print((char)248);
  tft.println(F("C"));
  
  // Target
  tft.setTextSize(1);
  tft.setCursor(60, 140);
  tft.setTextColor(TFT_WHITE);
  tft.print(F("Target: "));
  tft.print(targetTemp, 0);
  tft.println((char)248);
  
  // Status
  tft.setCursor(20, 180);
  if (currentTemp >= targetTemp - 1.5) {
    tft.setTextColor(TFT_GREEN);
    tft.println(F("Ready for Steaming!"));
    tft.setCursor(20, 200);
    tft.print(F("Pump: "));
    tft.println(brewActive ? F("ON (Full)") : F("OFF"));
  } else {
    tft.setTextColor(TFT_YELLOW);
    tft.println(F("Heating Thermoblock..."));
  }
  
  // Instructions
  tft.setTextSize(1);
  tft.setCursor(10, 240);
  tft.setTextColor(TFT_CYAN);
  tft.println(F("Release steam switch"));
  tft.setCursor(10, 255);
  tft.println(F("to exit"));
}

void displayMenu() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  
  tft.setTextSize(2);
  tft.setCursor(40, 10);
  tft.setTextColor(TFT_MAGENTA);
  tft.println(F("MENU"));
  
  tft.setTextSize(1);
  tft.setCursor(20, 60);
  tft.setTextColor(TFT_CYAN);
  tft.println(F("Settings (Rotate RIGHT):"));
  
  tft.setCursor(20, 90);
  tft.setTextColor(TFT_WHITE);
  tft.println(F("1. Brew Temp"));
  tft.setCursor(20, 110);
  tft.println(F("2. Brew Pressure"));
  tft.setCursor(20, 130);
  tft.println(F("3. Brew Time"));
  tft.setCursor(20, 150);
  tft.println(F("4. Pre-infusion"));
  tft.setCursor(20, 170);
  tft.println(F("5. Steam Temp"));
  tft.setCursor(20, 190);
  tft.println(F("6. Steam Pressure"));
  
  tft.setCursor(20, 230);
  tft.setTextColor(TFT_YELLOW);
  tft.println(F("Rotate LEFT to exit"));
}

// ===== NEW DISPLAY FUNCTIONS =====

void displayShutdown(float currentTemp) {
  // Static display update to avoid flicker
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate < 500) return;
  lastUpdate = millis();
  
  tft.fillScreen(TFT_BLACK);
  
  // Title - Machine Shutdown
  tft.setTextSize(2);
  tft.setCursor(15, 40);
  tft.setTextColor(TFT_RED);
  tft.println(F("SHUTDOWN"));
  
  // Message
  tft.setTextSize(1);
  tft.setCursor(10, 100);
  tft.setTextColor(TFT_WHITE);
  tft.println(F("20 min Inactivity"));
  tft.setCursor(10, 115);
  tft.println(F("Timeout reached"));
  
  // Current temperature (cooling down)
  tft.setTextSize(3);
  tft.setCursor(40, 150);
  tft.setTextColor(TFT_ORANGE);
  tft.print(currentTemp, 1);
  tft.print((char)248);
  tft.println(F("C"));
  
  // Instructions
  tft.setTextSize(1);
  tft.setCursor(10, 220);
  tft.setTextColor(TFT_CYAN);
  tft.println(F("To reactivate:"));
  tft.setCursor(10, 235);
  tft.println(F("Toggle manual switch"));
  tft.setCursor(10, 250);
  tft.println(F("or rotate encoder"));
}

void displaySteamPressureMenu(float steamPressure, bool editing) {
  tft.fillScreen(TFT_BLACK);
  
  // Title
  tft.setTextSize(2);
  tft.setCursor(10, 15);
  tft.setTextColor(TFT_MAGENTA);
  tft.println(F("STEAM PRESSURE"));
  
  // Current pressure value (large)
  tft.setTextSize(4);
  tft.setCursor(50, 80);
  tft.setTextColor(editing ? TFT_YELLOW : TFT_GREEN);
  tft.print(steamPressure, 1);
  
  // Bar unit
  tft.setTextSize(2);
  tft.setCursor(180, 100);
  tft.setTextColor(editing ? TFT_YELLOW : TFT_GREEN);
  tft.println(F("bar"));
  
  // Range indicator
  tft.setTextSize(1);
  tft.setCursor(30, 150);
  tft.setTextColor(TFT_WHITE);
  tft.print(F("Range: "));
  tft.print(MIN_STEAM_PRESSURE, 1);
  tft.print(F(" - "));
  tft.print(MAX_STEAM_PRESSURE, 1);
  tft.println(F(" bar"));
  
  // Status
  tft.setCursor(20, 180);
  tft.setTextColor(editing ? TFT_YELLOW : TFT_CYAN);
  if (editing) {
    tft.println(F("EDITING MODE"));
    tft.setCursor(10, 210);
    tft.setTextColor(TFT_WHITE);
    tft.println(F("Rotate: Adjust"));
    tft.setCursor(10, 225);
    tft.println(F("Click: Confirm"));
  } else {
    tft.println(F("READY"));
    tft.setCursor(10, 210);
    tft.setTextColor(TFT_WHITE);
    tft.println(F("Click to edit"));
    tft.setCursor(10, 225);
    tft.println(F("Rotate to navigate"));
  }
}

void displayBrewPressureMenu(float brewPressure, bool editing) {
  tft.fillScreen(TFT_BLACK);
  
  // Title
  tft.setTextSize(2);
  tft.setCursor(20, 15);
  tft.setTextColor(TFT_MAGENTA);
  tft.println(F("BREW PRESSURE"));
  
  // Current pressure value (large)
  tft.setTextSize(4);
  tft.setCursor(50, 80);
  tft.setTextColor(editing ? TFT_YELLOW : TFT_BLUE);
  tft.print(brewPressure, 1);
  
  // Bar unit
  tft.setTextSize(2);
  tft.setCursor(180, 100);
  tft.setTextColor(editing ? TFT_YELLOW : TFT_BLUE);
  tft.println(F("bar"));
  
  // Range indicator
  tft.setTextSize(1);
  tft.setCursor(30, 150);
  tft.setTextColor(TFT_WHITE);
  tft.print(F("Range: "));
  tft.print(MIN_BREW_PRESSURE, 1);
  tft.print(F(" - "));
  tft.print(MAX_BREW_PRESSURE, 1);
  tft.println(F(" bar"));
  
  // Default note
  tft.setCursor(15, 165);
  tft.setTextColor(TFT_ORANGE);
  tft.println(F("Typical: 9.0 bar"));
  
  // Status
  tft.setCursor(20, 190);
  tft.setTextColor(editing ? TFT_YELLOW : TFT_CYAN);
  if (editing) {
    tft.println(F("EDITING MODE"));
    tft.setCursor(10, 220);
    tft.setTextColor(TFT_WHITE);
    tft.println(F("Rotate: Adjust"));
    tft.setCursor(10, 235);
    tft.println(F("Click: Confirm"));
  } else {
    tft.println(F("READY"));
    tft.setCursor(10, 220);
    tft.setTextColor(TFT_WHITE);
    tft.println(F("Click to edit"));
    tft.setCursor(10, 235);
    tft.println(F("Rotate to navigate"));
  }
}

#endif
