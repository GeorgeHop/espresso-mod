#ifndef DISPLAY_H
#define DISPLAY_H

#include "config.h"

/*
 * TFT Display Driver
 * UI for espresso machine controller
 * Supports ILI9341 and similar SPI-based displays
 * 
 * Note: Requires Adafruit_GFX and display-specific library
 * (e.g., Adafruit_ILI9341 for ILI9341)
 */

// Forward declaration - implement with your specific display library
// Example using Adafruit ILI9341:
// #include <Adafruit_ILI9341.h>
// Adafruit_ILI9341 tft(TFT_CS_PIN, TFT_DC_PIN, TFT_RST_PIN);

void initDisplay() {
  // Initialize display based on your hardware
  // tft.begin();
  // tft.setRotation(1);
  // tft.fillScreen(ILI9341_BLACK);
  
  Serial.println("Display initialized");
}

void displayInit() {
  // Show startup screen
  // tft.fillScreen(ILI9341_BLACK);
  // tft.setTextColor(ILI9341_WHITE);
  // tft.setTextSize(2);
  // tft.setCursor(10, 10);
  // tft.println("ESPRESSO CTRL");
}

void updateDisplay(float currentTemp, float targetTemp, 
                   float currentPressure, float targetPressure, 
                   float flowRate, float currentVolume, float targetVolume,
                   unsigned long elapsedTime, unsigned long targetTime,
                   int state, unsigned long preInfusionElapsed = 0) {
  // Check if in steam mode and display accordingly
  if (state == 4) {  // STATE_STEAMING
    displaySteam(currentTemp, STEAM_TARGET_TEMP, currentPressure);
    return;
  }
  
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate < 500) {
    return; // Don't update too frequently
  }
  lastUpdate = millis();
  
  if (Serial) {
    // Display pre-infusion or brewing
    if (state == 2) {  // STATE_PRE_INFUSION
      Serial.println("\n━━ PRE-INFUSION ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
      
      Serial.print("TEMP     ");
      Serial.print(currentTemp, 1);
      Serial.print("°C → ");
      Serial.print(targetTemp, 1);
      Serial.println("°C");
      
      Serial.print("PRESSURE ");
      Serial.print(currentPressure, 1);
      Serial.print("bar → ");
      Serial.print(PREINFUSION_PRESSURE, 1);
      Serial.println("bar (low)");
      
      Serial.print("FLOW                    ");
      Serial.print(flowRate, 1);
      Serial.println(" ml/s");
      
      Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
      
      // Pre-infusion progress
      int preInfusionPercent = (preInfusionElapsed * 100) / PREINFUSION_TIME;
      preInfusionPercent = constrain(preInfusionPercent, 0, 100);
      
      Serial.print("SOAK   [");
      for (int i = 0; i < 20; i++) {
        Serial.print(i < (preInfusionPercent / 5) ? "█" : "░");
      }
      Serial.print("] ");
      Serial.print(preInfusionPercent);
      Serial.println("%");
      
      Serial.print("       ");
      Serial.print(preInfusionElapsed);
      Serial.print("s / ");
      Serial.print(PREINFUSION_TIME);
      Serial.println("s");
      
      Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
      
    } else if (state == 3) {  // STATE_BREWING
      // Minimalistic clean layout with bold labels
      Serial.println("\n━━ BREWING ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
      
      Serial.print("TEMP     ");
      Serial.print(currentTemp, 1);
      Serial.print("°C → ");
      Serial.print(targetTemp, 1);
      Serial.println("°C");
      
      Serial.print("PRESSURE ");
      Serial.print(currentPressure, 1);
      Serial.print("bar → ");
      Serial.print(targetPressure, 1);
      Serial.println("bar");
      
      Serial.print("FLOW                    ");
      Serial.print(flowRate, 1);
      Serial.println(" ml/s");
      
      Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
      
      // Calculate and display progress bars with 20 characters
      int timeProgress = (elapsedTime * 20) / max(targetTime, 1UL);
      int volumeProgress = (int)((currentVolume * 20) / max(targetVolume, 1.0f));
      
      Serial.print("TIME   [");
      for (int i = 0; i < 20; i++) {
        Serial.print(i < timeProgress ? "█" : "░");
      }
      Serial.print("] ");
      Serial.print((timeProgress * 5));
      Serial.println("%");
      
      Serial.print("       ");
      Serial.print(elapsedTime);
      Serial.print("s / ");
      Serial.print(targetTime);
      Serial.println("s");
      
      Serial.print("VOLUME [");
      for (int i = 0; i < 20; i++) {
        Serial.print(i < volumeProgress ? "█" : "░");
      }
      Serial.print("] ");
      Serial.print((volumeProgress * 5));
      Serial.println("%");
      
      Serial.print("       ");
      Serial.print(currentVolume, 1);
      Serial.print("ml / ");
      Serial.print(targetVolume, 1);
      Serial.println("ml");
      
      Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    } else {
      Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    }
  }

void displaySteam(float currentTemp, float targetTemp, float currentPressure) {
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate < 500) {
    return;
  }
  lastUpdate = millis();
  
  // Minimalistic steam display
  Serial.print("\x1B[2J");  // Clear screen
  Serial.print("\x1B[H");   // Home cursor
  
  Serial.println("\n           ███████╗████████╗███████╗ █████╗ ███╗   ███╗");
  Serial.println("           ██╔════╝╚══██╔══╝██╔════╝██╔══██╗████╗ ████║");
  Serial.println("           ███████╗   ██║   █████╗  ███████║██╔████╔██║");
  Serial.println("           ╚════██║   ██║   ██╔══╝  ██╔══██║██║╚██╔╝██║");
  Serial.println("           ███████║   ██║   ███████╗██║  ██║██║ ╚═╝ ██║");
  Serial.println("           ╚══════╝   ╚═╝   ╚══════╝╚═╝  ╚═╝╚═╝     ╚═╝\n");
  
  Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  
  Serial.print("TEMP     ");
  Serial.print(currentTemp, 1);
  Serial.print("°C → ");
  Serial.print(targetTemp, 1);
  Serial.println("°C");
  
  Serial.print("PRESSURE ");
  Serial.print(currentPressure, 1);
  Serial.println("bar → 15.0bar (MAX)");
  
  Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  
  // Temperature progress bar
  int tempPercent = constrain((int)((currentTemp - 50.0) / (targetTemp - 50.0) * 100), 0, 100);
  Serial.print("HEAT   [");
  for (int i = 0; i < 20; i++) {
    Serial.print(i < (tempPercent / 5) ? "█" : "░");
  }
  Serial.print("] ");
  Serial.print(tempPercent);
  Serial.println("%\n");
  
  if (tempPercent >= 100) {
    Serial.println("✓ READY TO STEAM\n");
  }
}

void displayError(const char* errorMsg) {
  // Display error on screen
  Serial.print("ERROR: ");
  Serial.println(errorMsg);
  
  // tft.fillScreen(ILI9341_RED);
  // tft.setTextColor(ILI9341_WHITE);
  // tft.setTextSize(2);
  // tft.setCursor(10, 10);
  // tft.println(errorMsg);
}

void displayMenu() {
  // Display menu items for encoder navigation
  // Implement based on your menu structure
}

#endif
