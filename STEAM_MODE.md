# Steam Mode Display and Control

## Overview

The espresso controller now has a dedicated **STEAM MODE** display that shows a large, prominent "STEAM" indicator with real-time heating progress and pump pressure status. This is activated by flipping the steam switch to the ON position.

## How Steam Mode Works

### Activation
- **Steam Switch**: When you flip the steam switch to ON, the system immediately:
  1. Enters **STEAM state**
  2. Sets target temperature to **120°C** (STEAM_TARGET_TEMP)
  3. Sets pump pressure to **15 bar** (maximum pressure)
  4. Displays dedicated steam screen

### Display During Steam Mode

The steam mode display shows:

```
Large ASCII Art "STEAM" Banner
═════════════════════════════════════════════════════════════
  Temperature: 115.5°C → 120.0°C
  Pressure:    15.0bar → 15.0bar (MAXIMUM)
═════════════════════════════════════════════════════════════

  Heating Progress: [████████████████████] 96%

  ✓ READY TO STEAM - Pump set to maximum pressure (15 bar)
```

### Key Features

**Temperature Control:**
- Automatically heats to 120°C for proper steam production
- Progress bar shows heating percentage (0-100%)
- PID heating system maintains target temperature

**Pump Control:**
- Pump pressure set to **255** (maximum) = 15 bar
- Maximum wand pressure for efficient steam
- Fixed at maximum throughout steam mode

**Visual Feedback:**
- Large "STEAM" text for clear mode indication
- Real-time temperature and pressure display
- Heating progress bar
- "READY TO STEAM" confirmation when temperature reached

**Serial Output:**
- Large ASCII banner for visibility in Serial Monitor
- Temperature tracking in °C
- Pressure status display
- Percentage-based heating progress

## Switching Between Modes

### Espresso to Steam
```
1. Turn steam switch ON
2. Screen displays STEAM banner
3. Temperature ramps to 120°C
4. When ready, use steam wand (pump at 15 bar)
5. Turn steam switch OFF to return to IDLE
```

### Steam to Espresso
```
1. Turn steam switch OFF
2. Screen returns to idle display
3. To brew again, pull brew switch (reheats to 92°C)
4. When heated, espresso pump pressure is controlled to 9 bar
```

## Technical Details

### Configuration Values (in config.h)

```cpp
#define STEAM_TARGET_TEMP       120.0    // °C for building steam
#define STEAM_TARGET_PRESSURE   15.0     // Bar for steam wand pressure
#define MAX_TEMP                130.0    // Updated upper limit
#define MAX_PRESSURE            15.0     // Updated upper limit
```

### State Machine Priority

Steam mode has **priority** over brew mode:
- If steam switch is ON → Force STEAM state immediately
- If steam switch is OFF → Proceed with normal brew state machine
- This prevents accidental mode switches

### Output Control During Steam

**SSR (Heating Element):**
- Enabled during heating phase
- PID controlled to reach 120°C
- Maintains temperature using full heating capacity if needed

**MCP4131 (Pump Pressure):**
- Set to **255** (maximum value)
- Provides 15 bar pressure at steam wand
- Maintained continuously while in STEAM state

## Serial Output Example

```
Entering STEAM state

╔════════════════════════════════════════════════════╗
║   STEAM MODE ACTIVATED                             ║
╚════════════════════════════════════════════════════╝

Temperature: 95.2°C → 120.0°C
Pressure:    15.0bar → 15.0bar (MAXIMUM)

Heating Progress: [████████░░░░░░░░░░░░] 42%
```

## Safety Considerations

1. **Thermoblock Temperature**: 120°C is safe for most espresso machines' thermoblock
2. **Pressure Safety**: 15 bar exceeds typical pump capability - operates at maximum safe level
3. **Auto-Shutoff**: Heating stops immediately when steam switch turns OFF
4. **Switch State Monitoring**: Checked every loop iteration for fast response

## Adjusting Steam Parameters

To change steam temperature or pressure limits, edit `config.h`:

```cpp
// To change steam target temperature (default 120°C)
#define STEAM_TARGET_TEMP       125.0    // New value in °C

// Note: Pump pressure is always set to maximum (255/15 bar) in steam mode
// This cannot be changed during steam operation
```

## Troubleshooting

**Steam mode won't activate:**
- Check steam switch wiring on pin A1
- Verify `readSteamSwitch()` returns LOW when activated
- Check serial output for state changes

**Temperature not reaching 120°C:**
- Check thermocouple connection
- Verify PID gains are appropriate (KP, KI, KD in config.h)
- Ensure heating element is working (SSR control)

**Pressure not at 15 bar:**
- Verify MCP4131 wiring and communication
- Check pump supply pressure
- Confirm pump is properly connected to pot wiper output

## Display Implementation Notes

The current steam display implementation uses **Serial Output** with ASCII art for clarity during debugging. When integrating a TFT display, replace the Serial calls in `displaySteam()` with:

```cpp
void displaySteam(float currentTemp, float targetTemp, float currentPressure) {
  // TFT implementation example:
  tft.fillScreen(ILI9341_BLACK);
  
  // Large STEAM text
  tft.setTextColor(ILI9341_ORANGE);
  tft.setTextSize(5);
  tft.setCursor(40, 40);
  tft.println("STEAM");
  
  // Temperature info
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(3);
  tft.setCursor(20, 120);
  tft.print("T: ");
  tft.print(currentTemp, 1);
  tft.print("C -> ");
  tft.println(targetTemp, 1);
  
  // Pressure info
  tft.setCursor(20, 170);
  tft.print("P: ");
  tft.print(currentPressure, 1);
  tft.println("bar -> 15.0bar");
  
  // Progress bar
  int tempProgress = ((currentTemp - 50) * 200) / (targetTemp - 50);
  tft.drawRect(20, 230, 200, 30);
  tft.fillRect(20, 230, tempProgress, 30, ILI9341_GREEN);
}
```

## Future Enhancements

Potential improvements to steam mode:
- [ ] Variable steam temperature control via menu
- [ ] Pressure gauge/indicator on display
- [ ] Steam wand protection (warning at high temp)
- [ ] Automatic steam mode timer
- [ ] Custom steam profiles for different drinks
