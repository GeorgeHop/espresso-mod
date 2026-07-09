# Settings Menu Control Guide

## How to Access and Change Settings

### Method 1: Via Rotary Encoder (Recommended)

#### Entering Settings Menu
1. **Press the rotary encoder button** (short press, ~50ms) to enter the settings menu
2. Serial output will show: `=== SETTINGS MENU ===`
3. Currently selected setting is shown: `> Temperature: 92.0°C`

#### Navigating Settings
- **Rotate encoder clockwise**: Increase the current value
- **Rotate encoder counter-clockwise**: Decrease the current value
- **Press encoder button**: Move to next setting

#### Available Settings (in order)
1. **Temperature** - Rotate to adjust ±0.5°C per click (50-120°C range)
2. **Pressure** - Rotate to adjust ±0.1 bar per click (6-12 bar range)
3. **Target Time** - Rotate to adjust ±1 second per click (5-60 seconds)
4. **Target Volume** - Rotate to adjust ±1 ml per click (10-100 ml)
5. **Exit** - Press to confirm exit and return to normal operation

#### Example Session
```
Encoder Button: Press to enter menu
Serial Output: === SETTINGS MENU ===
> Temperature: 92.0°C (rotate to adjust, press for next)

Encoder: Rotate 2 clicks clockwise (increases temp by 1°C)
Serial Output: Target Temp: 93.0°C

Encoder Button: Press to move to next setting
Serial Output: > Pressure: 9.0bar (rotate to adjust, press for next)

Encoder: Rotate 3 clicks counter-clockwise (decreases by 0.3 bar)
Serial Output: Target Pressure: 8.7bar

Encoder Button: Press to move to next setting
... (continue through Time and Volume)

Serial Output: > Exit Settings (press to confirm)
Encoder Button: Press to exit
Serial Output: === EXITING MENU ===
```

### Method 2: Serial Commands (Optional Enhancement)

You can add serial command support by modifying the Arduino code to listen for serial input during normal operation:

```cpp
if (Serial.available()) {
  char cmd = Serial.read();
  
  if (cmd == 't') {
    // Set temperature
    if (Serial.available()) {
      float temp = Serial.parseFloat();
      targetTemp = constrain(temp, MIN_TEMP, MAX_TEMP);
      Serial.print("Temperature set to: ");
      Serial.println(targetTemp);
    }
  }
  // Similar for p, m (time), v (volume)
}
```

### Method 3: Compile-Time Configuration

Edit settings directly in `config.h` before uploading:

```cpp
#define DEFAULT_TARGET_TEMP     92.0
#define DEFAULT_TARGET_PRESSURE 9.0
#define DEFAULT_TARGET_TIME     30
#define DEFAULT_TARGET_VOLUME   30
```

## Adjustments Per Encoder Click

| Setting | Per Click | Range | Notes |
|---------|-----------|-------|-------|
| Temperature | 0.5°C | 50-120°C | Typical range for espresso |
| Pressure | 0.1 bar | 6-12 bar | Standard pump pressure range |
| Time | 1 second | 5-60 seconds | 30s = standard shot |
| Volume | 1 ml | 10-100 ml | 30ml = standard shot |

## Viewing Current Settings

At any time, send this serial command (optional):
```
s
```

And the controller will display:
```
=== CURRENT SETTINGS ===
Temperature: 92.0°C
Pressure: 9.0bar
Target Time: 30s
Target Volume: 30ml
```

## Constraining Values

All values are automatically constrained to safe operating ranges:
- Temperature won't go below 50°C or above 120°C
- Pressure won't go below 6 bar or above 12 bar
- Time won't go below 5 seconds or above 60 seconds
- Volume won't go below 10 ml or above 100 ml

## Integration with Brew Cycle

Once you've set your preferred parameters:

1. Set **Target Temperature**: The machine heats to this temperature before brewing
2. Set **Target Pressure**: The pump maintains this pressure during the shot
3. Set **Target Time**: The shot will continue until this time is reached (or volume)
4. Set **Target Volume**: The shot will stop when this volume is dispensed (or time)

The shot completes when **EITHER** the time expires **OR** the volume is reached, whichever comes first.

### Example: 30-Second / 30ml Shot
```
Target Time: 30s
Target Volume: 30ml

Result: Shot stops when either 30 seconds elapsed OR 30ml dispensed
```

This gives you flexibility:
- If flow is slow: Time limit catches it (prevents dried-out puck)
- If flow is fast: Volume limit catches it (prevents over-extraction)

## Real-Time Progress Display

During brewing, the display shows progress toward your targets:
```
Time: 15s/30s | Volume: 25ml/30ml
Time progress:   [████████░░] 80%
Volume progress: [██████░░░░] 60%
```

This helps you see how the shot is progressing toward your preset values.
