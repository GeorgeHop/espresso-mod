#ifndef MENU_H
#define MENU_H

#include "config.h"

/*
 * Menu System for Settings Control
 * Rotary encoder: rotate to adjust, press to cycle through menu items
 */

enum MenuState {
  MENU_DISABLED,
  MENU_TEMP,
  MENU_PRESSURE,
  MENU_TIME,
  MENU_VOLUME,
  MENU_PREINFUSION_ENABLE,
  MENU_PREINFUSION_TIME,
  MENU_EXIT
};

MenuState currentMenu = MENU_DISABLED;
MenuState previousMenu = MENU_DISABLED;

void enterMenu() {
  currentMenu = MENU_TEMP;
  Serial.println("\n=== SETTINGS MENU ===");
  displayMenuState();
}

void exitMenu() {
  if (currentMenu != MENU_DISABLED) {
    Serial.println("=== EXITING MENU ===\n");
    currentMenu = MENU_DISABLED;
  }
}

void cycleMenu() {
  if (currentMenu == MENU_DISABLED) {
    return;
  }
  
  // Cycle through menu items
  switch (currentMenu) {
    case MENU_TEMP:
      currentMenu = MENU_PRESSURE;
      break;
    case MENU_PRESSURE:
      currentMenu = MENU_TIME;
      break;
    case MENU_TIME:
      currentMenu = MENU_VOLUME;
      break;
    case MENU_VOLUME:
      currentMenu = MENU_PREINFUSION_ENABLE;
      break;
    case MENU_PREINFUSION_ENABLE:
      currentMenu = MENU_PREINFUSION_TIME;
      break;
    case MENU_PREINFUSION_TIME:
      currentMenu = MENU_EXIT;
      break;
    case MENU_EXIT:
      exitMenu();
      return;
    case MENU_DISABLED:
      break;
  }
  
  if (currentMenu != MENU_DISABLED) {
    displayMenuState();
  }
}

void adjustMenuValue(int delta) {
  if (currentMenu == MENU_DISABLED) {
    return;
  }
  
  switch (currentMenu) {
    case MENU_TEMP:
      // Each encoder tick = 0.5°C
      extern void adjustTargetTemp(int);
      adjustTargetTemp(delta);
      break;
      
    case MENU_PRESSURE:
      // Each encoder tick = 0.1 bar
      extern void adjustTargetPressure(int);
      adjustTargetPressure(delta);
      break;
      
    case MENU_TIME:
      // Each encoder tick = 1 second
      extern void adjustTargetTime(int);
      adjustTargetTime(delta);
      break;
      
    case MENU_VOLUME:
      // Each encoder tick = 1 ml
      extern void adjustTargetVolume(int);
      adjustTargetVolume(delta);
      break;
      
    case MENU_PREINFUSION_ENABLE:
      // Toggle pre-infusion on/off
      extern void togglePreInfusion();
      togglePreInfusion();
      break;
      
    case MENU_PREINFUSION_TIME:
      // Each encoder tick = 1 second
      extern void adjustPreInfusionTime(int);
      adjustPreInfusionTime(delta);
      break;
      
    case MENU_EXIT:
    case MENU_DISABLED:
      break;
  }
}

void displayMenuState() {
  if (currentMenu == MENU_DISABLED) {
    return;
  }
  
  extern float targetTemp;
  extern float targetPressure;
  extern unsigned long targetTime;
  extern float targetVolume;
  extern unsigned long preInfusionTime;
  extern byte preInfusionEnabled;
  
  Serial.print("\n> ");
  
  switch (currentMenu) {
    case MENU_TEMP:
      Serial.print("Temperature: ");
      Serial.print(targetTemp, 1);
      Serial.println("°C (rotate to adjust, press for next)");
      break;
      
    case MENU_PRESSURE:
      Serial.print("Pressure: ");
      Serial.print(targetPressure, 1);
      Serial.println("bar (rotate to adjust, press for next)");
      break;
      
    case MENU_TIME:
      Serial.print("Target Time: ");
      Serial.print(targetTime);
      Serial.println("s (rotate to adjust, press for next)");
      break;
      
    case MENU_VOLUME:
      Serial.print("Target Volume: ");
      Serial.print(targetVolume, 1);
      Serial.println("ml (rotate to adjust, press for next)");
      break;
      
    case MENU_PREINFUSION_ENABLE:
      Serial.print("Pre-Infusion: ");
      Serial.print(preInfusionEnabled ? "ON" : "OFF");
      Serial.println(" (rotate to toggle, press for next)");
      break;
      
    case MENU_PREINFUSION_TIME:
      Serial.print("Pre-Infusion Time: ");
      Serial.print(preInfusionTime);
      Serial.println("s (rotate to adjust, press for next)");
      break;
      
    case MENU_EXIT:
      Serial.println("Exit Settings (press to confirm)");
      break;
      
    case MENU_DISABLED:
      break;
  }
}

bool isMenuActive() {
  return currentMenu != MENU_DISABLED;
}

#endif
