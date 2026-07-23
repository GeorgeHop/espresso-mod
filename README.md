# Espresso Machine Nano Controller

A comprehensive Arduino Nano-based controller for espresso machines with temperature, pressure, and flow rate management.

## Features

- **Temperature Control**: MAX6675 K-type thermocouple for precise temperature monitoring
- **Pressure Monitoring**: Analog pressure sensor for pump pressure measurement
- **Pump Control**: PWM AC220V controller for smooth pump speed adjustment
- **Shot Progress Tracking**: Real-time display of brewing progress
- **Heating Control**: SSR (Solid State Relay) for thermoblock heating with PID control
- **User Interface**: TFT display with rotary encoder for menu navigation
- **Dual Mode**: Espresso and steam modes
- **Auto-Shutoff Protection**: 
  - Automatic machine shutdown after 20 minutes of inactivity
  - Activity is detected when user makes any changes (encoder, switches)
  - To reactivate: toggle manual power switch (down then up)
  - Activity timer resets on each user interaction

## Hardware Requirements

### Microcontroller
- Arduino Nano (ATmega328P)

## Bill of Materials (Components List)

### Main Components
1. **Arduino Nano** - Microcontroller (ATmega328P)
2. **Termopara Typu K Moduł MAX6675** - K-type Thermocouple Module, up to 800°C (SPI)
3. **Wyświetlacz TFT IPS 1,69″ ST7789V 240x280px** - 1.69" IPS TFT Display (SPI)
4. **Czujnik Przetwornika G1/4 Ciśnienie 0-200PSI 5V** - Pressure Sensor 0-200 PSI, G1/4 thread, 5V output (Analog)

### Control & Power Components
5. **Moduł enkodera wieloobrotowego z przyciskiem** - Multi-turn Rotary Encoder Module
   - 360° rotation, multi-turn
   - 20 steps per rotation
   - Integrated push button
   
6. **Przetwornica napięcia 230V AC - 5V 2A DC** - Power Supply Converter (230V AC to 5V 2A DC)

7. **PRZEKAŹNIK 1-KANAŁOWY MODUŁ 5V AVR ARDUINO ARM 10A** - 1-Channel Relay Module 5V, 10A (for aux control)

8. **PRZEKAŹNIK PÓŁPRZEWODNIKOWY SSR DA 40A DC-AC** - Solid State Relay (SSR) 40A DC-AC (for heating element)

9. **Moduł Sterownik PWM AC220V YYAC-3S** - PWM AC220V Controller/Dimmer (for pump control)

### Sensors
- **MAX6675**: K-type thermocouple amplifier (SPI)
- **Pressure Transducer**: 0-200 PSI analog 5V output
- **Rotary Encoder**: With integrated push button

### Actuators
- **SSR 40A DC-AC**: Solid State Relay for heating element
- **PWM AC220V Controller (YYAC-3S)**: PWM dimmer for pump control
- **TFT Display**: 1.69" IPS ST7789V (240x280px)

### Switches
- Brew switch (momentary, active low)
- Steam switch (momentary or toggle, active low)
- Manual power switch (toggle) for machine on/off

## Pin Configuration

See `config.h` for complete pin mapping. Default configuration:

| Component | Pin | Type |
|-----------|-----|------|
| MAX6675 CS | 10 | Digital |
| MAX6675 CLK | 13 | Digital |
| MAX6675 MOSI | 11 | Digital |
| MAX6675 MISO | 12 | Digital |
| Pressure Sensor | A0 | Analog |
| Encoder CLK | 2 | Digital |
| Encoder DT | 3 | Digital |
| Encoder SW | 4 | Digital |
| Brew Switch | 5 | Digital |
| Steam Switch | 6 | Digital |
| Display CLK (SPI) | 13 | Digital |
| Display MOSI (SPI) | 11 | Digital |
| Display CS | 7 | Digital |
| Display DC | 8 | Digital |
| Display RST | 9 | Digital |
| SSR Heating | 3 | PWM |
| PWM Pump Controller | A2 | Analog (PWM via DAC/GPIO) |

### PWM Pump Control

The pump is controlled via the **YYAC-3S AC220V PWM Controller** which receives a 0-5V analog signal from the Arduino:

- **Signal Pin**: A2 (can be configured as PWM output or analog output)
- **Voltage Range**: 0-5V DC
  - 0V = Pump off (0% speed)
  - 5V = Pump at maximum speed (100%)
- **Control Method**: 
  - Use `analogWrite(PWM_PUMP_PIN, value)` where value is 0-255
  - Or use `analogReference()` with DAC if available
  - Smoothly ramp up/down pump speed for consistent shot quality

**Typical Usage:**
```cpp
analogWrite(PWM_PUMP_PIN, 180);  // ~70% pump speed
```

## Required Libraries

Add these to your Arduino libraries:
- `Adafruit_GFX` - Graphics library
- Display-specific library:
  - `Adafruit_ILI9341` (for ILI9341 displays)
  - `TFT_ST7789` or equivalent for other displays

## Installation

1. Clone/download this project
2. Open `espresso_nano.ino` in Arduino IDE
3. Install required libraries via Library Manager
4. Adjust `config.h` for your specific hardware:
   - Pin assignments
   - Sensor calibration values
   - PID parameters
   - Display type
5. Upload to Arduino Nano

## Configuration

### Calibration

**Temperature Sensor:**
- Adjust `TEMP_OFFSET` and `TEMP_SCALE` in config.h
- Place thermocouple in ice water (0°C) or boiling water (100°C) to verify

**Pressure Sensor:**
- Run calibration at startup (no pressure = 0 bar)
- Adjust `PRESSURE_SCALE` and `PRESSURE_OFFSET` based on your transducer specs

**Flow Meter:**
- Default calibration: 450 pulses/liter for YF-S201
- Adjust `FLOW_METER_PPL` if using different meter

### Shot Parameters

**Default values** (adjust in `config.h`):
```cpp
#define DEFAULT_TARGET_TIME     30       // seconds for typical espresso
#define DEFAULT_TARGET_VOLUME   30       // ml for typical espresso
```

**Runtime adjustment** (via encoder):
- Call `adjustTargetTime(delta)` or `adjustTargetVolume(delta)` from encoder handler
- Range: 5-60 seconds, 10-100 ml

### PID Tuning

Adjust in `config.h`:
```cpp
#define KP  2.5    // Proportional gain
#define KI  0.5    // Integral gain
#define KD  0.1    // Derivative gain
```

Use Ziegler-Nichols method or empirical tuning for your machine.

## Operation

### States

1. **IDLE**: Heating element off, pump off
2. **HEATING**: Heating to target temperature
3. **BREWING**: Maintaining temperature and dispensing
4. **STEAMING**: High temperature, low pressure mode
5. **MENU**: Configuration access via rotary encoder

### Controls

- **Brew Switch**: Trigger brew/espresso shot
- **Steam Switch**: Switch to steam mode
- **Rotary Encoder**: Nav (temp, pressure, time, volume)
  - Press: Confirm/Enter menu
- **Manual Power Switch**: Toggle to activate machine after auto-shutoff

### Auto-Shutoff & Activity Tracking

The machine automatically enters a low-power shutdown mode after **20 minutes of inactivity** to save energy and protect the heating element:

**How it works:**
- Activity timer starts on boot and is reset whenever the user:
  - Adjusts settings with the rotary encoder
  - Presses the encoder button
  - Activates brew or steam mode
  - Toggles any control switch
- After 20 minutes without any user interaction:
  - Heating element turns off (SSR disabled)
  - Display may dim or show standby mode
  - Pump is disabled
  
**To reactivate the machine:**
- Toggle the **manual power switch** down, then back up
- This clears the inactivity flag and resets the activity timer
- Machine returns to normal operation and reheats

**Configuration:**
- Inactivity timeout: `#define INACTIVITY_TIMEOUT 1200000` (20 minutes in milliseconds) in `config.h`
- Adjust this value if you need a different auto-shutoff duration

### Display Output

During brewing, the controller displays:
- **Temperature**: Current vs target (°C)
- **Pressure**: Current vs target (bar)
- **Flow Rate**: Real-time flow in ml/s
- **Shot Time**: Elapsed vs target time with progress bar
- **Shot Volume**: Current vs target volume with progress bar
- **Status**: Current state (IDLE, HEATING, BREWING, STEAMING)

Example serial output during brew:
```
=== ESPRESSO CONTROLLER ===
Temp: 92.5°C → 92.0°C | Pressure: 9.2bar → 9.0bar | Flow: 2.5ml/s
Time: 15s/30s | Volume: 25ml/30ml
Time progress:   [████████░░] 80%
Volume progress: [██████░░░░] 60%
```
  - Press: Confirm/Enter menu

## Safety Features

- Thermocouple fault detection
- Temperature limits (50-120°C)
- Pressure limits (6-12 bar)
- Emergency shutdown on critical errors

## Serial Debug Output

Connect Arduino to computer and open Serial Monitor (115200 baud) for:
- Temperature, pressure, and flow rate readings
- State changes
- Encoder input
- Error messages

## Wiring Notes

### Power
- Arduino Nano: 5V from USB or external supply
- SSR: Connect to 3.3V/5V output from pin 3
- MCP4131: 5V supply
- Display: Check voltage requirements (may need level shifting)

### Ground
- All components share common ground with Arduino

### SPI Sharing
Multiple SPI devices can share CLK and MOSI if chip select pins are separate.

## Troubleshooting

**Display not showing:**
- Check pin configuration matches your display
- Verify SPI communication
- Ensure display library is installed

**Temperature reading errors:**
- Check MAX31855 wiring
- Look for "Thermocouple fault" messages in serial output

**Pressure reading stuck at 0:**
- Verify HX710B wiring and power
- Run calibration routine

**Flow meter not working:**
- Check interrupt pin connection
- Verify filter/blockage in flow meter

## Future Enhancements

- Data logging to SD card
- WiFi connectivity for remote monitoring
- Multiple brew profiles
- Pressure ramp control
- Pre-infusion support
- Shot timer display

## License

[Add your license here]

## Support

For issues or questions, check the serial debug output and verify calibration values match your hardware.
