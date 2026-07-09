# Pre-Infusion Setup Guide

## What is Pre-Infusion?

Pre-infusion is a brewing technique where low pressure is applied to the coffee puck for a short duration before the full espresso shot begins. This allows water to slowly saturate the grounds, improving extraction and reducing channeling.

## How it Works

### Default Configuration
- **Pre-infusion Duration**: 5 seconds
- **Pre-infusion Pressure**: 2.5 bar (low pressure for saturation)
- **Full Pressure**: 9 bar (after pre-infusion completes)

### Brew Sequence

```
1. HEATING STATE
   └─> Temperature reaches target (92°C)

2. PRE-INFUSION STATE (5 seconds)
   ├─> Pump pressure: 2.5 bar (low)
   ├─> Coffee grounds slowly saturate
   └─> Display shows: "━━ PRE-INFUSION ━━"
       with SOAK progress bar

3. BREWING STATE
   ├─> Pump pressure: 9 bar (full)
   ├─> Full extraction begins
   └─> Display shows: "━━ BREWING ━━"
       with TIME and VOLUME progress bars
```

## Enabling/Disabling Pre-Infusion

### To Enable
Edit `config.h`:
```cpp
#define PREINFUSION_ENABLED     1        // 1 = enabled
#define PREINFUSION_TIME        5        // seconds
#define PREINFUSION_PRESSURE    2.5      // bar
#define PREINFUSION_POT_VALUE   80       // digital pot value
```

### To Disable
Edit `config.h`:
```cpp
#define PREINFUSION_ENABLED     0        // 0 = disabled
```

When disabled, brew process skips pre-infusion and goes directly to full pressure brewing.

## Customizing Pre-Infusion

### Adjust Duration
```cpp
#define PREINFUSION_TIME        3        // Faster: 3 seconds
#define PREINFUSION_TIME        8        // Longer: 8 seconds
```

### Adjust Pressure
The pressure is controlled by the MCP4131 digital pot value (0-255). To find your values:

```
Pot Value → Approximate Pressure
50        → 1.5 bar (very low)
80        → 2.5 bar (recommended default)
100       → 3.0 bar (medium)
120       → 3.5 bar (medium-high)
```

To change:
```cpp
#define PREINFUSION_POT_VALUE   100      // Higher value = higher pressure
```

## Display During Pre-Infusion

### Serial Output Example

```
━━ PRE-INFUSION ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
TEMP     88.5°C → 92.0°C
PRESSURE 2.1bar → 2.5bar (low)
FLOW                    0.8 ml/s
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
SOAK   [███░░░░░░░░░░░░░░░░] 23%
       1s / 5s
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
```

### Display Elements

- **PRE-INFUSION header** - Clearly indicates saturation phase
- **TEMP** - Current vs target temperature
- **PRESSURE** - Current vs pre-infusion target pressure (marked "low")
- **FLOW** - Water flow rate (typically 0.5-1.0 ml/s)
- **SOAK** - Progress bar showing saturation time remaining
- **Time display** - Elapsed / Total pre-infusion time

## Workflow Example

### Pulling a Shot with Pre-Infusion

```
1. Machine heated to 92°C ✓

2. Press brew switch
   Serial: "Entering HEATING state"

3. Temperature reaches 92°C
   Serial: "Entering PRE-INFUSION state"

4. Screen shows PRE-INFUSION with:
   - SOAK progress bar
   - 5 second countdown
   - Low pressure (2.5 bar)

5. After 5 seconds
   Serial: "Transitioning to BREWING state"

6. Screen changes to BREWING
   - TIME and VOLUME progress bars
   - Full pressure (9 bar)
   - Shot extraction begins

7. At 30s or 30ml (whichever first)
   Serial: "Shot complete - Time: 30s, Volume: 30ml"
```

## Technical Details

### State Machine Flow

```
STATE_IDLE
    ↓ (brew switch pressed)
STATE_HEATING
    ↓ (temp reached & PREINFUSION_ENABLED = 1)
STATE_PRE_INFUSION (5 seconds)
    ├─ SSR: PID heating enabled
    ├─ MCP4131: PREINFUSION_POT_VALUE (2.5 bar)
    ├─ Display: PRE-INFUSION screen
    └─ After timeout or brew switch release:
        ↓
    STATE_BREWING
        ├─ SSR: PID heating enabled
        ├─ MCP4131: controlPressure() (9 bar)
        ├─ Display: BREWING screen with progress
        └─ After time/volume limit or switch release:
            ↓
        STATE_IDLE
```

### Output Control During Pre-Infusion

**SSR (Heating):**
- Continuously active to maintain 92°C
- PID controller adjusts power as needed

**MCP4131 (Pump Pressure):**
- Fixed at `PREINFUSION_POT_VALUE` (typically ~80 for 2.5 bar)
- Provides low pressure saturation phase

**Flow Rate:**
- Typically 0.5-1.0 ml/s during pre-infusion
- Lower than full extraction (~2-3 ml/s during brewing)

## Cancelling Pre-Infusion

You can exit pre-infusion at any time by releasing the brew switch:

```
STATE_PRE_INFUSION + brew switch released
    ↓
STATE_IDLE
Serial: "Pre-infusion cancelled"
```

## Pre-Infusion with Other Parameters

### Combining with Shot Settings

Pre-infusion time is **fixed** at 5 seconds (or your custom value). It runs before the target time counter starts:

```
Scenario: TARGET_TIME = 30 seconds
├─ Pre-infusion: 5 seconds (not counted toward 30s)
├─ Then brewing: 30 seconds (with full pressure)
└─ Total shot: ~35 seconds
```

### Effect on Flow Meter

The flow meter counts pulses throughout pre-infusion AND brewing. So for a 5-second pre-infusion followed by 25-second brew at target volume 30ml:

```
Pre-infusion (5s): ~4-5 ml flows
Brewing (25s):     ~25-26 ml flows
Total:             ~30 ml (reaches target)
```

## Troubleshooting

**Pre-infusion not activating:**
- Check `PREINFUSION_ENABLED = 1` in config.h
- Verify brew switch is still pressed when temp is reached
- Look for "Entering PRE-INFUSION state" in serial output

**Pre-infusion pressure too high/low:**
- Adjust `PREINFUSION_POT_VALUE` in config.h
- Test with values 70-110
- Remember higher value = higher pressure

**Pre-infusion time too short/long:**
- Adjust `PREINFUSION_TIME` in config.h
- Typical range: 3-8 seconds
- Default: 5 seconds

**Screen not showing PRE-INFUSION:**
- Verify display.h has updated `updateDisplay()` function
- Check state is actually STATE_PRE_INFUSION (serial debug)
- May need to clear serial monitor and re-run

## Advanced: Custom Pre-Infusion Curves

For future enhancement, you could implement:
- Ramp pre-infusion (gradual pressure increase)
- Multiple pre-infusion pulses
- Temperature-dependent pre-infusion time
- Variable pressure based on puck resistance

Currently, pre-infusion is a fixed: **5 seconds at 2.5 bar**.

## References

- Pre-infusion improves extraction by 5-10%
- Recommended for: lighter roasts, high-end espresso
- Not necessary for: darker roasts, commercial machines
- Typical 9 bar pressure reduced to 2-3 bar saturation
