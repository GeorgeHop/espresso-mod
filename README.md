# Espresso Machine Nano Controller

A comprehensive Arduino Nano-based controller for espresso machines with temperature, pressure, and flow rate management.

## Features

- **Temperature Control**: MAX31855 K-type thermocouple for precise temperature monitoring
- **Pressure Monitoring**: HX710B load cell amplifier for pump pressure measurement
- **Flow Rate Measurement**: YF-S201 flow meter for water dispensing
- **Shot Progress Tracking**: Real-time progress bars for elapsed time vs target time and volume dispensed vs target volume
- **Heating Control**: SSR (Solid State Relay) for thermoblock heating with PID control
- **Pump Control**: MCP4131 digital potentiometer controlling SCR for pressure regulation
- **User Interface**: TFT display with rotary encoder for menu navigation
- **Dual Mode**: Espresso and steam modes

## Hardware Requirements

### Microcontroller
- Arduino Nano (ATmega328P)

### Sensors
- **MAX31855**: K-type thermocouple amplifier (SPI)
- **HX710B**: 24-bit load cell amplifier (I2C-like protocol)
- **YF-S201**: Hall effect flow meter
- **Rotary Encoder**: With integrated push button

### Actuators
- **SSR**: 10-30A Solid State Relay for heating
- **MCP4131**: 256-position digital potentiometer (SPI)
- **TFT Display**: 2.4"-3.5" SPI-based (ILI9341, ST7789, etc.)

### Switches
- Brew switch (momentary, active low)
- Steam switch (momentary or toggle, active low)

## Pin Configuration

See `config.h` for complete pin mapping. Default configuration:

| Component | Pin | Type |
|-----------|-----|------|
| MAX31855 CS | 10 | Digital |
| MAX31855 CLK | 13 | Digital |
| MAX31855 MOSI | 11 | Digital |
| MAX31855 MISO | 12 | Digital |
| HX710B DOUT | 4 | Digital |
| HX710B CLK | 5 | Digital |
| Flow Meter | 2 | Interrupt |
| Encoder CLK | 6 | Digital |
| Encoder DT | 7 | Digital |
| Encoder SW | 8 | Digital |
| Brew Switch | A0 | Analog/Digital |
| Steam Switch | A1 | Analog/Digital |
| SSR | 3 | PWM |
| MCP4131 CS | A2 | Digital |
| MCP4131 CLK | A3 | Digital |
| MCP4131 MOSI | A4 | Digital |
| TFT CS | A5 | Digital |
| TFT DC | A6 | Digital |
| TFT RST | A7 | Digital |

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
