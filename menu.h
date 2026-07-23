#ifndef MENU_H
#define MENU_H

#include "config.h"

/*
 * Menu System for Settings Control
 * Encoder Navigation:
 * - Rotate: Navigate through settings or adjust value
 * - Click: Select/confirm setting
 * 
 * In STEAM_PRESSURE menu, rotation adjusts pressure directly
 */

enum MenuState {
  MENU_DISABLED = 0,
  MENU_BREW_TEMP = 1,
  MENU_BREW_PRESSURE = 2,
  MENU_BREW_TIME = 3,
  MENU_PREINFUSION = 4,
  MENU_STEAM_TEMP = 5,
  MENU_STEAM_PRESSURE = 6,
  MENU_EXIT = 7
};

MenuState currentMenuSelection = MENU_BREW_TEMP;
bool inSettingEdit = false;  // True when user is editing a value
float tempBrewPressure = PUMP_DEFAULT_BREW_PRESSURE;
float tempSteamPressure = PUMP_DEFAULT_STEAM_PRESSURE;

void initMenu() {
  tempBrewPressure = PUMP_DEFAULT_BREW_PRESSURE;
  tempSteamPressure = PUMP_DEFAULT_STEAM_PRESSURE;
}

void advanceMenuSelection() {
  if (currentMenuSelection == MENU_STEAM_PRESSURE) {
    currentMenuSelection = MENU_BREW_TEMP;
  } else {
    currentMenuSelection = (MenuState)((int)currentMenuSelection + 1);
  }
  inSettingEdit = false;  // Reset edit mode when navigating
}

void reverseMenuSelection() {
  if (currentMenuSelection == MENU_BREW_TEMP) {
    currentMenuSelection = MENU_STEAM_PRESSURE;
  } else {
    currentMenuSelection = (MenuState)((int)currentMenuSelection - 1);
  }
  inSettingEdit = false;  // Reset edit mode when navigating
}

void enterEditMode() {
  if (currentMenuSelection == MENU_STEAM_PRESSURE) {
    inSettingEdit = true;
    tempSteamPressure = getCurrentSteamPressure();
  }
}

void exitEditMode() {
  inSettingEdit = false;
}

bool isInEditMode() {
  return inSettingEdit;
}

// Adjust brewing pressure during edit mode
void adjustBrewPressure(int delta) {
  if (inSettingEdit && currentMenuSelection == MENU_BREW_PRESSURE) {
    // Each step = 0.5 bar adjustment
    float adjustment = delta * 0.5;
    tempBrewPressure += adjustment;
    tempBrewPressure = constrain(tempBrewPressure, MIN_BREW_PRESSURE, MAX_BREW_PRESSURE);
  }
}

// Adjust steam pressure during edit mode
void adjustSteamPressure(int delta) {
  if (inSettingEdit && currentMenuSelection == MENU_STEAM_PRESSURE) {
    // Each step = 0.5 bar adjustment
    float adjustment = delta * 0.5;
    tempSteamPressure += adjustment;
    tempSteamPressure = constrain(tempSteamPressure, MIN_STEAM_PRESSURE, MAX_STEAM_PRESSURE);
  }
}

void commitBrewPressureChange() {
  if (currentMenuSelection == MENU_BREW_PRESSURE) {
    setPumpBrewPressure(tempBrewPressure);
  }
}

void commitSteamPressureChange() {
  if (currentMenuSelection == MENU_STEAM_PRESSURE) {
    setPumpSteamPressure(tempSteamPressure);
  }
}

MenuState getCurrentMenuSelection() {
  return currentMenuSelection;
}

float getTempBrewPressure() {
  return tempBrewPressure;
}

float getTempSteamPressure() {
  return tempSteamPressure;
}

#endif
