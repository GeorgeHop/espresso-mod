# Menu System with Pre-Infusion

## Menu Navigation Flow

Press the **rotary encoder button** to enter settings menu, then rotate and press to navigate:

```
SETTINGS MENU
    ↓ (Press)
1. TEMPERATURE
    ↓ (Press)
2. PRESSURE
    ↓ (Press)
3. TARGET TIME
    ↓ (Press)
4. TARGET VOLUME
    ↓ (Press)
5. PRE-INFUSION ENABLE ← NEW
    ↓ (Press)
6. PRE-INFUSION TIME ← NEW
    ↓ (Press)
7. EXIT
    ↓ (Press)
EXIT MENU
```

## Serial Display Examples

### Menu Item 1: Temperature
```
> Temperature: 92.0°C (rotate to adjust, press for next)
```
- **Rotate left**: Decrease temperature
- **Rotate right**: Increase temperature
- **Adjustment**: ±0.5°C per click
- **Range**: 50-130°C

### Menu Item 2: Pressure
```
> Pressure: 9.0bar (rotate to adjust, press for next)
```
- **Rotate left**: Decrease pressure
- **Rotate right**: Increase pressure
- **Adjustment**: ±0.1 bar per click
- **Range**: 6-15 bar

### Menu Item 3: Target Time
```
> Target Time: 30s (rotate to adjust, press for next)
```
- **Rotate left**: Decrease time
- **Rotate right**: Increase time
- **Adjustment**: ±1 second per click
- **Range**: 5-60 seconds

### Menu Item 4: Target Volume
```
> Target Volume: 30ml (rotate to adjust, press for next)
```
- **Rotate left**: Decrease volume
- **Rotate right**: Increase volume
- **Adjustment**: ±1 ml per click
- **Range**: 10-100 ml

### Menu Item 5: Pre-Infusion Enable ⭐ NEW
```
> Pre-Infusion: ON (rotate to toggle, press for next)
```
- **Rotate left or right**: Toggle ON/OFF
- **Options**: ON or OFF
- **Function**: Enables/disables the pre-infusion phase
- **Effect**: 
  - **ON**: Brew sequence includes 5-second low-pressure saturation
  - **OFF**: Skips pre-infusion, goes directly to full pressure

### Menu Item 6: Pre-Infusion Time ⭐ NEW
```
> Pre-Infusion Time: 5s (rotate to adjust, press for next)
```
- **Rotate left**: Decrease time
- **Rotate right**: Increase time
- **Adjustment**: ±1 second per click
- **Range**: 2-10 seconds
- **Default**: 5 seconds
- **Effect**: How long the low-pressure saturation phase lasts

### Menu Item 7: Exit
```
> Exit Settings (press to confirm)
```
- **Press**: Exits menu and returns to normal operation
- **All settings saved automatically**

## Complete Example Session

### Scenario: User adjusts settings for a lighter roast

```
User presses encoder button:
Serial: === SETTINGS MENU ===

Step 1: Check/Adjust Temperature
Serial: > Temperature: 92.0°C (rotate to adjust, press for next)
User rotates 2 clicks right (increase 1°C):
Serial: Target Temp: 93.0°C
Serial: > Temperature: 93.0°C (rotate to adjust, press for next)
User presses encoder button

Step 2: Check/Adjust Pressure
Serial: > Pressure: 9.0bar (rotate to adjust, press for next)
User rotates 2 clicks left (decrease 0.2 bar):
Serial: Target Pressure: 8.8bar
Serial: > Pressure: 8.8bar (rotate to adjust, press for next)
User presses encoder button

Step 3: Check/Adjust Time
Serial: > Target Time: 30s (rotate to adjust, press for next)
User rotates 3 clicks right (increase 3s):
Serial: Target Time: 33s
Serial: > Target Time: 33s (rotate to adjust, press for next)
User presses encoder button

Step 4: Check/Adjust Volume
Serial: > Target Volume: 30ml (rotate to adjust, press for next)
User leaves at default, presses encoder button

Step 5: Enable Pre-Infusion ⭐
Serial: > Pre-Infusion: OFF (rotate to toggle, press for next)
User rotates once (any direction) to toggle ON:
Serial: Pre-Infusion: ON
Serial: > Pre-Infusion: ON (rotate to toggle, press for next)
User presses encoder button

Step 6: Adjust Pre-Infusion Time ⭐
Serial: > Pre-Infusion Time: 5.0s (rotate to adjust, press for next)
User rotates 2 clicks right (increase 1s):
Serial: Pre-Infusion Time: 6.0s
Serial: > Pre-Infusion Time: 6.0s (rotate to adjust, press for next)
User presses encoder button

Step 7: Exit
Serial: > Exit Settings (press to confirm)
User presses encoder button
Serial: === EXITING MENU ===

All settings saved:
✓ Temperature: 93.0°C
✓ Pressure: 8.8bar
✓ Time: 33s
✓ Volume: 30ml
✓ Pre-Infusion: ON
✓ Pre-Infusion Time: 6.0s
```

## Quick Reference: Menu Adjustments

| Setting | Adjustment | Range | Default |
|---------|-----------|-------|---------|
| Temperature | ±0.5°C/click | 50-130°C | 92.0°C |
| Pressure | ±0.1 bar/click | 6-15 bar | 9.0 bar |
| Time | ±1 sec/click | 5-60s | 30s |
| Volume | ±1 ml/click | 10-100ml | 30ml |
| Pre-Infusion Enable | Toggle ON/OFF | ON/OFF | ON |
| Pre-Infusion Time | ±1 sec/click | 2-10s | 5s |

## How Pre-Infusion Settings Affect Brewing

### Example 1: Light Roast with Pre-Infusion ON (6 seconds)
```
Menu Settings:
- Temperature: 93°C
- Pressure: 8.8 bar
- Pre-Infusion: ON
- Pre-Infusion Time: 6.0s
- Target Time: 33s
- Target Volume: 30ml

Brew Sequence:
1. HEATING (to 93°C) ~30 seconds
2. PRE-INFUSION (6 seconds) ← Low pressure saturation
   ├─ "━━ PRE-INFUSION ━━"
   ├─ SOAK progress bar [██████░░░░░░░░░░░░░░] 50%
   ├─ Time: 3s / 6s
   └─ Pressure: 2.5 bar (low)
3. BREWING (33 seconds) ← Full pressure extraction
   ├─ "━━ BREWING ━━"
   ├─ TIME progress bar
   ├─ VOLUME progress bar
   └─ Pressure: 8.8 bar (full)
4. COMPLETE after 33s or 30ml reached

Total shot time: ~6s + 33s = ~39 seconds
```

### Example 2: Dark Roast with Pre-Infusion OFF
```
Menu Settings:
- Temperature: 92°C
- Pressure: 9.0 bar
- Pre-Infusion: OFF ← No saturation phase
- Target Time: 28s
- Target Volume: 30ml

Brew Sequence:
1. HEATING (to 92°C) ~30 seconds
2. BREWING (skips pre-infusion, goes straight to full pressure)
   ├─ "━━ BREWING ━━"
   ├─ TIME progress bar
   ├─ VOLUME progress bar
   └─ Pressure: 9.0 bar (full)
3. COMPLETE after 28s or 30ml reached

Total shot time: ~28 seconds
```

## Tips for Using the Menu

1. **Enter Menu**: Press encoder button (short press ~50ms)
2. **Navigate**: Press again to go to next item
3. **Adjust**: Rotate encoder to change value
4. **View Changes**: Settings print to Serial when adjusted
5. **Exit**: Navigate to "Exit Settings" and press

## Pre-Infusion Menu Benefits

With pre-infusion in the menu, you can now:
- ✓ Enable/disable pre-infusion on-the-fly
- ✓ Adjust saturation time (2-10 seconds)
- ✓ Experiment with different profiles
- ✓ Fine-tune without recompiling

Example workflow:
```
Monday: Pre-Infusion Time: 4s (quick saturation)
Tuesday: Pre-Infusion Time: 6s (medium saturation)
Wednesday: Pre-Infusion Time: 8s (long saturation)
→ Compare shot quality without changing temp/pressure!
```

## Advanced: Using Pre-Infusion with Profiles

Combine pre-infusion settings with other adjustments:

### Light Roast Profile
```
SETTINGS:
Temp: 93°C (higher for extraction)
Pressure: 8.5 bar (lower for delicate flavors)
Pre-Infusion: ON
Pre-Infusion Time: 6.0s (longer saturation)
Time: 35s, Volume: 30ml
```

### Dark Roast Profile
```
SETTINGS:
Temp: 91°C (lower to avoid bitterness)
Pressure: 9.0 bar (standard)
Pre-Infusion: OFF (not needed)
Time: 27s, Volume: 30ml
```

You can switch between profiles just by adjusting the menu values!
