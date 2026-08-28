# Espresso Machine Controller - Wiring Diagram

## Arduino Nano Pin Configuration

```
                    ARDUINO NANO
                   ┌─────────────┐
                5V │ 1        30 │ GND
                GND│ 2        29 │ A5 (SPARE)
       (UNUSED)  D0│ 3        28 │ A4 - RELAY_CONTROL (OUTPUT)
       (UNUSED)  D1│ 4        27 │ A3 - POWER_SWITCH (INPUT)
ENCODER_SW_PIN    D4│ 5        26 │ A2 - PUMP_PWM (OUTPUT)
BREW_SWITCH_PIN   D5│ 6        25 │ A1 (SPARE)
STEAM_SWITCH_PIN  D6│ 7        24 │ A0 - PRESSURE_SENSOR (INPUT)
TFT_RST_PIN       D7│ 8        23 │ GND
SSR_PIN           D9│ 9        22 │ D11 - TFT_CS
TFT_CS_PIN       D11│10        21 │ D12 - MOSI (SPI)
TFT_MOSI_PIN     D12│11        20 │ D13 - CLK (SPI)
MAX6675_MISO_PIN D12│12        19 │ D12 - MOSI (shared SPI)
MAX6675_CLK_PIN  D13│13        18 │ 3.3V (optional, for display logic)
                GND │14        17 │ RST
                5V  │15        16 │ GND
                   └─────────────┘

Unused Pins: 0, 1, A1, A5
Available Pins: A1, A5 (for future expansion)
```

## Pin Assignment Summary

### **INPUTS** (Sensors & Switches)
| Pin | Function | Component | Type | Voltage |
|-----|----------|-----------|------|---------|
| A0  | Pressure Sensor | 5V G1/4 analog sensor | Analog In | 0-5V (0-10 bar) |
| A3  | Power Switch | Latching switch (UP/DOWN) | Digital In | 5V (HIGH=ON, LOW=OFF) |
| D4  | Encoder Button | Rotary encoder with button | Digital In | 5V (LOW = pressed) |
| D5  | Brew Switch | Latching switch (Ulka pump) | Digital In | 5V (LOW = active) |
| D6  | Steam Switch | Latching switch (thermoblock) | Digital In | 5V (LOW = active) |
| D2  | Encoder CLK | Rotary encoder (INT0) | Digital In | 5V (pulse) |
| D3  | Encoder DT  | Rotary encoder (INT1) | Digital In | 5V (pulse) |
| D12 | SPI MISO | MAX6675 + TFT Display | Digital In | 5V (shared) |
| D13 | SPI CLK  | MAX6675 + TFT Display | Digital In | 5V (shared) |

### **OUTPUTS** (Control)
| Pin | Function | Component | Type | Voltage | Purpose |
|-----|----------|-----------|------|---------|---------|
| D9  | SSR PWM | SSR 40A relay | PWM Output | 0-5V | Heating element control (0-100%) |
| A2  | Pump PWM | AC220V pump controller | Analog Out | 0-5V | Pump speed/pressure (0-100%) |
| A4  | Relay Control | Power relay | Digital Out | HIGH/LOW | Machine main power on/off |
| D8  | TFT DC   | ST7789V display | Digital Out | 3.3V/5V | Display data/command |
| D11 | TFT CS   | ST7789V display | Digital Out | 3.3V/5V | Display chip select |
| D7  | TFT RST  | ST7789V display | Digital Out | 3.3V/5V | Display reset |

### **SPI BUS** (Shared)
- **MOSI (D12)**: Master Out Slave In → MAX6675 + TFT Display
- **CLK (D13)**: Serial Clock → MAX6675 + TFT Display
- **CS Pins**: 
  - D10: MAX6675 thermocouple chip select
  - D11: TFT Display chip select

---

## Component Wiring Details

### 1. **Temperature Sensor - MAX6675 Thermocouple Amplifier**
```
MAX6675 Module Pin    Arduino Pin    Signal
──────────────────────────────────────────────
VCC                   5V             (Module accepts 3.3V-5V)
GND                   GND
CLK (D0)              D13 (CLK)      SPI Clock
MOSI (D1)             D12 (MOSI)     SPI Data In
CS (D2)               D10            Chip Select
SO (D3)               D12 (MISO)     SPI Data Out

Thermocouple (K-Type, M6 thread):
+ (Red)               K+ input       (Max: 800°C)
- (Yellow)            K- input       (Probe: 48cm cable, M6 thread)
```

**Module Specs:**
- Operating voltage: 3.3V – 5V (powers directly from Arduino 5V)
- Resolution: 12-bit, 0.25°C precision
- Interface: SPI
- Temperature range: -20°C to +85°C (module), -200°C to +800°C (thermocouple)
- Sampling: ~10Hz (conversion every ~100ms)
- Calibration: Temperature offset tunable via TEMP_OFFSET

### 2. **Pressure Sensor - 5V G1/4 Analog**
```
Sensor Pin     Arduino Pin    Signal
──────────────────────────────────────
VCC (5V)       5V
GND            GND
Signal (0-5V)  A0             Analog input (0-1023 ADC)
```

**Calibration:**
- PRESSURE_MIN_ADC: 102 (≈ 0.5V, 0 bar)
- PRESSURE_MAX_ADC: 921 (≈ 4.5V, 10 bar)
- Adjust if sensor reads offset

### 3. **Display - ST7789V TFT 1.69" IPS (240x280)**
```
Display Pin    Arduino Pin    Signal
──────────────────────────────────────
VCC            5V (with 100µF cap)
GND            GND
CLK            D13 (SPI CLK)
MOSI           D12 (SPI MOSI)
DC             D8
CS             D11
RST            D7
GND (LED-)     GND
VCC (LED+)     5V (via resistor, ≈20Ω to limit current)
```

**Notes:**
- 3.3V logic safe with 5V tolerant pins
- Add 100µF capacitor on VCC for stability
- LED backlight draws ~80mA

### 4. **Rotary Encoder with Button**
```
Encoder Pin    Arduino Pin    Signal
──────────────────────────────────────
+5V            5V
GND            GND
CLK            D2 (INT0)      Clock pulse
DT             D3 (INT1)      Data pulse
SW             D4             Button (pulled up, LOW = pressed)
```

**Notes:**
- Debounce: 20ms in code
- Double-click detection: 500ms window
- Typically 20 PPR (pulses per rotation) = 40 detents

### 5. **Switches - TODEA Latching (3x)**

#### 5a. Power Switch (Pin A3)
**Switch: TODEA Latching 2-Position (Machine Power)**
```
Switch Terminal    Arduino Pin    Signal
──────────────────────────────────────────
Terminal 1        A3             (Power ON/OFF state)
Terminal 2        GND            (Common ground)
```
**Connection Details:**
- Either terminal → Arduino A3 (POWER_SWITCH_PIN)
- Other terminal → GND
- ✅ Can connect either way - switch is bidirectional
- **UP position**: A3 reads HIGH → Machine ON
- **DOWN position**: A3 reads LOW → Machine OFF
- **Persistent**: Stays in position while held

#### 5b. Brew Switch - Ulka Pump (Pin D5)
**Switch: TODEA Latching 2-Position (Espresso Pump Control)**
```
Switch Terminal    Arduino Pin    Signal
──────────────────────────────────────────
Terminal 1        D5             (Brew ON/OFF)
Terminal 2        GND            (Common ground)
```
**Connection Details:**
- Either terminal → Arduino D5 (BREW_SWITCH_PIN)
- Other terminal → GND
- ✅ Can connect either way - switch is bidirectional
- **DOWN position**: D5 reads LOW → Pump runs (brew mode active)
- **UP position**: D5 reads HIGH → Pump off
- **Persistent**: Runs continuously while held DOWN
- **Interlock**: Automatically disabled if steam switch is ON

#### 5c. Steam Switch - Thermoblock (Pin D6)
**Switch: SW312 Click Button (Steam Wand - Persistent/Latching)**
```
SW312 Terminal     Arduino Pin    Signal
──────────────────────────────────────────
C (Common)        D6             (Steam ON/OFF)
NO (Normally Open) GND            (Common ground)
```
**Connection Details:**
- C terminal → Arduino D6 (STEAM_SWITCH_PIN) OR NO terminal → D6 (either works)
- Other terminal → GND
- ✅ Can connect either way - switch is bidirectional
- **Held DOWN**: D6 reads LOW → Steam mode active (thermoblock heating)
- **Released**: D6 reads HIGH → Steam off
- **Persistent**: Runs continuously while held DOWN
- **Interlock**: Automatically disabled if brew switch is ON
- **Safety**: Only ONE of (Brew OR Steam) can be active at a time

**Notes:**
- Use **Arduino internal pull-ups** (no external resistors needed)
- Enable in code: `pinMode(D5, INPUT_PULLUP)` and `pinMode(D6, INPUT_PULLUP)`
- **All 3 switches are PERSISTENT/LATCHING** (not momentary buttons):
  - Power Switch: Holds ON/OFF position
  - Brew Switch: Runs pump continuously while held
  - Steam Switch: Runs steam mode continuously while held
- **Brew-Steam Interlock**: Only ONE can be active at a time
  - If brew switch is ON → steam switch is ignored/disabled
  - If steam switch is ON → brew switch is ignored/disabled
- When switch open: pin reads HIGH (5V from internal pull-up)
- When switch closed/pressed: pin reads LOW (shorted to GND)

### 6. **Heating Element Control - SSR 40A (Pin D9)**
```
SSR Control Pin    Arduino Pin    Signal
───────────────────────────────────────────
Positive (Control) D9 (PWM)       0-5V PWM signal
Negative (Control) GND            Common ground

Main SSR Load (Mains):
AC IN             Live (230V AC)
AC OUT → Load     Heating element
AC OUT → Load     Neutral
```

**Specifications:**
- Max current: 40A DC-AC
- Trigger voltage: 3-5V DC (logic compatible)
- PWM frequency: ~490Hz (Arduino D9 default)
- Output: 0-100% heating power

**Safety:**
- Include 30A breaker in AC circuit
- Ensure SSR heatsink is adequate
- Add ferrite clamp on AC leads to prevent EMI

### 7. **Pump Control - PWM AC220V Controller (Pin A2)**
```
Pump Controller Pin    Arduino Pin    Signal
────────────────────────────────────────────
Control Input (0-5V)   A2 (PWM)       0-5V analog
GND                    GND            Common ground

Main Pump (AC):
AC IN                  Live (230V AC)
AC OUT                 Pump motor
AC OUT                 Neutral
```

**Specifications:**
- Input: 0-5V analog signal (PWM converted)
- Output: 0-255 PWM maps to 0-100% pump speed
- Typical pump: AC 220V Ulka 40W (15 bar max)
- Pressure mapping: 2-15 bar range via firmware

### 8. **Relay - Main Power Control (Pin A4)**
```
Relay Module Pin    Arduino Pin    Signal
──────────────────────────────────────────
VCC (5V)           5V
GND                GND
IN (Input)         A4             Control signal (LOW = ON)

Relay Contacts (Main Power):
NO (Normally Open)      Common
                        → SSR (heating)
                        → Pump controller (AC)
                        → Other 240V components
```

**Configuration:**
- 1-Channel 5V Relay Module (10A @ 230V)
- **LOW-active trigger** (internal transistor + diode included)
- Arduino LOW (0V) on A4 → Relay energized → Machine powered
- Arduino HIGH (5V) on A4 → Relay off → Machine cut
- Status LED on module indicates relay state
- No external transistor needed (already built-in)

---

## Power Distribution

### 5V Supply (Arduino & Logic)
```
AC 240V Mains
    ↓
[Power Supply: AC-DC 240V→5V @ 3A]
    ↓
    ├─→ Arduino Nano 5V
    ├─→ MAX6675 thermocouple module (direct 5V)
    ├─→ Rotary Encoder
    ├─→ Switch pull-ups
    ├─→ Pressure Sensor
    ├─→ Display (via regulator to 3.3V)
    └─→ Relay module (VCC pin)

⚠️ NOTE: Total logic current: ~500mA typical
Recommend 3-5A PSU for headroom
```

### Mains AC Distribution (240V)
```
AC 240V Live (Phase)
    ↓
[30A Breaker]
    ↓
    ├─→ SSR Input (heating element on output)
    ├─→ Pump Controller Input (pump on output)
    └─→ Other 240V loads
    
Common: Return to Neutral/Ground
```

---

## Voltage Levels by Component

| Component | Supply | Logic Levels | Notes |
|-----------|--------|--------------|-------|
| Arduino Nano | 5V USB | 5V logic | Microcontroller |
| MAX6675 Module | 5V (direct) | 5V logic | Temperature sensor, no regulator needed |
| TFT Display | 5V (logic reg) | 3.3V logic | 5V tolerant |
| Rotary Encoder | 5V | 5V logic | Mechanical switch |
| Switches | 5V (pull-up) | 5V logic | TODEA latching |
| Pressure Sensor | 5V | 0-5V analog | Linear output |
| SSR | 5V control | 3-5V DC trigger | Logic-level compatible |
| Pump Controller | 5V control | 0-5V analog | PWM input |
| Relay Module | 5V | 5V logic (low-active) | 10A @ 230V, built-in protection |

---

## Assembly Checklist

### Step 1: Connect Arduino + Sensors
- [ ] MAX6675 VCC → 5V, GND → GND
- [ ] MAX6675 thermocouple interface → D13(CLK), D12(MOSI), D10(CS)
- [ ] Thermocouple probe (K-type red/yellow) → MAX6675 inputs (M6 thread into sensor)
- [ ] Pressure sensor → A0 (with smoothing in code)
- [ ] Add 100µF cap on pressure sensor VCC

### Step 2: Connect Display
- [ ] TFT ST7789V → D13(CLK), D12(MOSI), D8(DC), D11(CS), D7(RST)
- [ ] Add 100µF cap on display VCC
- [ ] Add 20Ω resistor on LED+ for backlight current limit

### Step 3: Connect Control Inputs
- [ ] Rotary Encoder: D2(CLK), D3(DT), D4(BTN), 5V, GND
- [ ] Power Switch → A3 (HIGH = ON, LOW = OFF) + GND
- [ ] Brew Switch (TODEA latching): One terminal → D5, Other terminal → GND
- [ ] Steam Switch (SW312 click button): C or NO terminal → D6, Other terminal → GND
- [ ] In code: `pinMode(D5, INPUT_PULLUP)` and `pinMode(D6, INPUT_PULLUP)` enabled in setup()

### Step 4: Connect Outputs
- [ ] SSR Control → D9 (PWM heating element)
- [ ] Pump PWM → A2 (pump speed 0-5V)
- [ ] Relay Module: VCC → 5V, GND → GND, IN → A4
- [ ] Verify relay module LED indicator works
- [ ] Add 30A breaker on main AC supply

### Step 5: Test & Calibrate
- [ ] Verify 5V on VCC, GND connectivity
- [ ] Serial monitor: thermocouple reading (should show room temp ~20-25°C)
- [ ] Pressure sensor: 0-10 bar range mapping (adjust PRESSURE_MIN/MAX_ADC)
- [ ] Display: boot animation + home screen
- [ ] Encoder: rotation detection + button press
- [ ] Switches: serial output on state change

---

## Troubleshooting Guide

### Problem: Thermocouple reads 0°C or -20°C
- **Cause**: MAX6675 wiring, SPI bus issue, or probe not seated
- **Fix**: 
  - Verify 5V on MAX6675 VCC (module accepts 3.3V-5V)
  - Verify D13(CLK), D12(MOSI), D10(CS) SPI connections
  - Check thermocouple K-type probe polarity (red=+, yellow=-)
  - Ensure probe is fully screwed into M6 thread

### Problem: Display shows garbage or doesn't start
- **Cause**: SPI conflict or RST not held long enough
- **Fix**:
  - Add 100µF capacitor on display VCC
  - Verify D13(CLK), D12(MOSI), D11(CS), D8(DC), D7(RST)
  - Check 3.3V regulator on MOSI/CLK pins

### Problem: Pressure sensor reads full scale (10.0 bar) at rest
- **Cause**: ADC calibration off or sensor dead
- **Fix**:
  - Verify 0-5V on sensor output (should be ~0.5V at rest)
  - Adjust PRESSURE_MIN_ADC (102) and PRESSURE_MAX_ADC (921)
  - Check sensor power supply

### Problem: Encoder not working (menu doesn't navigate)
- **Cause**: D2/D3 interrupt pins or pull-ups
- **Fix**:
  - Verify 5V pull-ups on CLK/DT lines
  - Check D2/D3 for 5V pulses (use scope)
  - Rotate slowly and check serial output

### Problem: SSR or Relay doesn't activate
- **Cause**: Control pin logic or wiring issue
- **Fix**:
  - Measure 0-5V on D9 (SSR) or A4 (relay) with voltmeter
  - For relay module: LOW (0V) on A4 should activate (check LED on module)
  - Verify relay module VCC (5V), GND, and IN (A4) connections
  - Test relay manually by pulling IN pin to GND

---

## Safety Reminders ⚠️

1. **High Voltage (240V AC)**
   - Always disconnect from mains before wiring
   - Use insulated tools
   - Wear gloves when handling AC circuits

2. **Thermocouple**
   - Reaches 800°C+: Do not touch probe during brewing
   - Use insulated probe guard

3. **Heating Element**
   - SSR will get hot under full load
   - Mount with adequate heatsink
   - Ensure airflow

4. **Power Supply**
   - Use regulated 5V PSU rated for 3-5A
   - Include fuse on 5V supply line
   - Add 100µF capacitors on all component VCC

5. **Testing**
   - Start with display test (safest)
   - Test encoder before heating
   - Test pressure/temp sensors before powering AC circuits

---

## Future Expansions

### Spare Pins Available
- **D0, D1**: Serial (if not debugging) → could use for sensors
- **A1**: Analog input → water level sensor (float switch)
- **A5**: Analog input → flow meter or additional sensor

### Optional Additions
1. **Water Level Sensor** (float switch) → A1
2. **Flow Meter** (Hall effect) → D0 or A5
3. **EEPROM** (I2C) → A4/A5 for persistent settings
4. **Buzzer** (alarm) → unused pin via transistor

---

## Revision History

- **v1.2** (2026-08-27): All 3 switches confirmed as persistent/latching, documented brew-steam interlock logic, fixed steam switch interlock in firmware
- **v1.1** (2026-08-27): Added SW312 steam wand click button documentation, clarified TODEA latching switch for brew, confirmed internal pull-ups enabled in firmware
- **v1.0** (2026-08-24): Initial wiring scheme with power control, 3-switch setup, pressure detection
