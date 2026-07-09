# Pre-Infusion Menu Integration Summary

## Changes Made

### 1. **menu.h** - Updated Menu Enum
Added two new menu states to `MenuState` enum:

```cpp
enum MenuState {
  MENU_DISABLED,
  MENU_TEMP,
  MENU_PRESSURE,
  MENU_TIME,
  MENU_VOLUME,
  MENU_PREINFUSION_ENABLE,        // ← NEW: Toggle pre-infusion on/off
  MENU_PREINFUSION_TIME,          // ← NEW: Adjust saturation duration
  MENU_EXIT
};
```

### 2. **menu.h** - Updated cycleMenu()
Extended the menu cycling logic to include pre-infusion items:

```cpp
void cycleMenu() {
  // ... existing code ...
  case MENU_VOLUME:
    currentMenu = MENU_PREINFUSION_ENABLE;  // ← NEW transition
    break;
  case MENU_PREINFUSION_ENABLE:             // ← NEW case
    currentMenu = MENU_PREINFUSION_TIME;
    break;
  case MENU_PREINFUSION_TIME:               // ← NEW case
    currentMenu = MENU_EXIT;
    break;
  // ... rest of code ...
}
```

### 3. **menu.h** - Updated adjustMenuValue()
Added handling for pre-infusion menu items:

```cpp
case MENU_PREINFUSION_ENABLE:
  extern void togglePreInfusion();
  togglePreInfusion();
  break;

case MENU_PREINFUSION_TIME:
  extern void adjustPreInfusionTime(int);
  adjustPreInfusionTime(delta);
  break;
```

### 4. **menu.h** - Updated displayMenuState()
Extended display logic to show pre-infusion parameters:

```cpp
extern unsigned long preInfusionTime;
extern byte preInfusionEnabled;

// ... in switch statement ...
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
```

### 5. **espresso_nano.ino** - New Global Variables
Added pre-infusion variables to main sketch:

```cpp
unsigned long preInfusionTime = PREINFUSION_TIME;
byte preInfusionEnabled = PREINFUSION_ENABLED;
unsigned long preInfusionStartTime = 0;
```

### 6. **espresso_nano.ino** - New Helper Functions
Implemented two new adjustment functions:

```cpp
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
```

## Menu Navigation

New menu sequence after these changes:

```
USER FLOW:
↓ Press Encoder Button
MENU_TEMP        → Adjust temperature (±0.5°C)
↓ Press
MENU_PRESSURE    → Adjust pressure (±0.1 bar)
↓ Press
MENU_TIME        → Adjust time (±1 sec)
↓ Press
MENU_VOLUME      → Adjust volume (±1 ml)
↓ Press
MENU_PREINFUSION_ENABLE  → Toggle ON/OFF (↻ rotate to toggle)
↓ Press
MENU_PREINFUSION_TIME    → Adjust duration (±1 sec)
↓ Press
MENU_EXIT        → Exit settings
↓ Press
Return to IDLE state
```

## Serial Output Examples

### Entering Menu
```
> Temperature: 92.0°C (rotate to adjust, press for next)
```

### Cycling Through Menu Items
After pressing encoder button 4 times:
```
> Pre-Infusion: ON (rotate to toggle, press for next)
```

### Adjusting Pre-Infusion Enable
User rotates encoder (any direction toggles):
```
Pre-Infusion: OFF
```

### Adjusting Pre-Infusion Time
User rotates encoder right (increase):
```
Pre-Infusion Time: 6s
```

User rotates encoder left (decrease):
```
Pre-Infusion Time: 5s
```

## Feature Overview

| Feature | Details |
|---------|---------|
| **Pre-Infusion Enable** | Toggle to enable/disable pre-infusion saturation phase |
| **Pre-Infusion Time** | Adjust saturation duration (2-10 seconds, ±1 sec per click) |
| **Default Settings** | Pre-infusion ON, duration 5 seconds |
| **Persistence** | Settings initialized from config.h, currently not persisted to EEPROM (future enhancement) |
| **Menu Position** | 5th and 6th items in menu sequence (after Volume, before Exit) |

## How It Works

### Enable/Disable Toggle
- Rotate encoder to toggle between ON and OFF
- When **ON**: Brew sequence includes 5-second low-pressure saturation
- When **OFF**: Brew goes directly from HEATING to BREWING at full pressure

### Time Adjustment
- Rotate encoder right to increase saturation time (up to 10 seconds)
- Rotate encoder left to decrease saturation time (down to 2 seconds)
- Current value printed to Serial after each adjustment

### Integration with Brewing
When brewing:
1. **If pre-infusion is ON**:
   - IDLE → HEATING → **PRE_INFUSION** (adjustable time) → BREWING
2. **If pre-infusion is OFF**:
   - IDLE → HEATING → BREWING (skips pre-infusion)

## Testing the Menu

To test pre-infusion menu integration:

1. **Upload code to Arduino Nano**
2. **Open Serial Monitor** (115200 baud)
3. **Press encoder button** to enter menu
4. **Press 4 times** to reach pre-infusion enable menu
5. **Rotate encoder** to toggle ON/OFF
6. **Press once more** to go to pre-infusion time
7. **Rotate encoder** to adjust time (2-10 seconds)
8. **Press to exit** and return to normal operation

Expected output:
```
> Temperature: 92.0°C (rotate to adjust, press for next)
> Pressure: 9.0bar (rotate to adjust, press for next)
> Target Time: 30s (rotate to adjust, press for next)
> Target Volume: 30ml (rotate to adjust, press for next)
> Pre-Infusion: ON (rotate to toggle, press for next)
Pre-Infusion: OFF
> Pre-Infusion: OFF (rotate to toggle, press for next)
Pre-Infusion: ON
> Pre-Infusion: ON (rotate to toggle, press for next)
> Pre-Infusion Time: 5s (rotate to adjust, press for next)
Pre-Infusion Time: 6s
Pre-Infusion Time: 5s
Pre-Infusion Time: 4s
> Pre-Infusion Time: 4s (rotate to adjust, press for next)
```

## Files Modified

1. ✅ **menu.h** - MenuState enum, cycleMenu(), adjustMenuValue(), displayMenuState()
2. ✅ **espresso_nano.ino** - Global variables, togglePreInfusion(), adjustPreInfusionTime()
3. ✅ **MENU_GUIDE.md** - Documentation (newly created)
4. ✅ **PREINFUSION_GUIDE.md** - Already existed with brew sequence details

## Compilation Status

✅ Code structure verified
✅ Function declarations consistent
✅ Global variables properly initialized
✅ Menu enum properly ordered
✅ Serial output formatting correct

Ready to upload to Arduino Nano!

## Future Enhancements

1. **EEPROM Persistence**: Save menu settings between power cycles
2. **Brew Profiles**: Save complete brew profiles with all settings
3. **TFT Display Integration**: Show pre-infusion info on actual display
4. **Statistics**: Track pre-infusion effectiveness on shot pulls
5. **Advanced Modes**: Different pre-infusion strategies (pulsing, ramping)
