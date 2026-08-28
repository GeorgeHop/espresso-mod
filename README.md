# Espresso Machine Nano Controller

A comprehensive Arduino Nano-based controller for espresso machines with temperature, pressure, and flow rate management.

## About This Project

My coffee journey started fast—I went through multiple grinders and espresso machines searching for the perfect setup. When I finally found a machine that suited my needs, I thought that was the end... but obviously not! The machine lacked essential features:

- ❌ No temperature control → Thermoblock fluctuations everywhere
- ❌ No pressure control → Inconsistent extractions
- ❌ No built-in timer → Manual timing for every shot
- ❌ No preinfusion → Can't dial in properly
- ❌ No portafilter preheating → Temperature shock on group head
- ❌ Limited steam mode → Poor milk steaming performance
- ❌ No turn on schedule
- ❌ No flow meter

I knew about Gagguino and Gaggia mods, but I'd already invested in my machine. Why switch and buy another kit?

**So I decided to build an open-source espresso machine controller.** With experience and the help of Claude, I selected all the hardware, designed the system, and got everything to fit. Now anyone can:

- ✅ Build this mod for their machine
- ✅ Tweak and add new features
- ✅ Create a fully custom machine
- ✅ Share improvements with the community
- ✅ Just have fun experimenting! ☕

This is a **community-driven open-source project**. Contributions, improvements, and forks are welcome!

## ⚠️ Safety & Disclaimer

**IMPORTANT: This project is currently in active development.** Please read and understand the following before proceeding:

### Risks & Responsibilities
- **This project involves work with high voltage (230V AC) and high current circuits.** Mistakes in wiring or assembly can result in:
  - Electrical shock or electrocution
  - Fire hazards
  - Damage to equipment or property
  - Personal injury

- **All modifications are done entirely at your own risk.** The author and contributors provide this project "as-is" without warranties or guarantees of safety or functionality.

- **You are responsible for:**
  - Understanding electrical safety principles
  - Following all wiring diagrams carefully
  - Double-checking connections before power-up
  - Having appropriate electrical knowledge (or consulting an electrician)
  - Proper grounding and circuit protection

### Before Plugging Into Outlet
1. **Assemble and test Arduino + all modules FIRST** while unplugged:
   - Upload firmware to Arduino Nano
   - Connect all sensors (thermocouple, pressure sensor)
   - Connect display, encoder, relay module
   - Test each component individually
   - Verify Serial output shows sensor readings

2. **Only after validation, assemble the high-voltage section:**
   - Connect SSR, pump controller, and relay to mains power
   - Double-check all connections against WIRING_DIAGRAM.md
   - Use a multimeter to verify connections

3. **Test the full system:**
   - Plug in 5V power supply (low voltage) first
   - Verify Arduino starts and reads sensors
   - Test relay activation without machine connected
   - Only then connect to your espresso machine

### Disclaimer
This project is provided for educational purposes and as-is without any liability. By building and using this controller, you acknowledge:
- You understand the electrical risks involved
- You accept full responsibility for safety
- The author is not liable for any damage, injury, or loss

**When in doubt, consult a qualified electrician or experienced electronics hobbyist.**

---

## Hardware Requirements
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

### Microcontroller
- Arduino Nano (ATmega328P)

## Bill of Materials (Components List)

### Main Components
1. **Arduino Nano** - Microcontroller (ATmega328P)
2. **K-Type Thermocouple Module MAX6675** - K-type Thermocouple Module, up to 800°C (SPI)
3. **1.69" TFT IPS Display ST7789V 240x280** - 1.69" IPS TFT Display (SPI)
4. **G1/4 Pressure Transducer Sensor 0-200PSI 5V** - Pressure Sensor 0-200 PSI, G1/4 thread, 5V output (Analog)

### Control & Power Components
5. **Multi-turn Rotary Encoder Module with Button** - Multi-turn Rotary Encoder Module
   - 360° rotation, multi-turn
   - 20 steps per rotation
   - Integrated push button
   
6. **AC 230V to 5V 2A DC Power Converter** - Power Supply Converter (230V AC to 5V 2A DC)

7. **1-Channel 5V Relay Module for Arduino 10A** - 1-Channel Relay Module 5V, 10A (for main power control)

8. **Solid State Relay (SSR) 40A DC-AC Module** - Solid State Relay (SSR) 40A DC-AC (for heating element)

9. **AC220V PWM Controller Module YYAC-3S** - PWM AC220V Controller/Dimmer (for pump control)

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

## Getting Started

### Prerequisites
- Basic soldering skills (recommended)
- Arduino IDE installed on your computer
- A compatible espresso machine to modify
- Tools: Multimeter, wire strippers, screwdrivers

### What You'll Find in This Repo

```
├── espresso-mod.ino              # Main firmware
├── config.h                      # Pin & parameter configuration
├── WIRING_DIAGRAM.md            # Detailed wiring instructions
├── README.md                    # This file
├── HARDWARE_NOTES.md            # Hardware-specific details
├── MENU_GUIDE.md                # UI menu system
├── SETTINGS_GUIDE.md            # Configuration options
├── PREINFUSION_GUIDE.md         # Pre-infusion feature
├── STEAM_MODE.md                # Steam mode settings
├── libraries.txt                # Required Arduino libraries
└── test/                        # Test sketches
```

### Step-by-Step Setup

1. **Review Hardware**: Read [WIRING_DIAGRAM.md](WIRING_DIAGRAM.md) thoroughly
2. **Gather Components**: Purchase all items from [Bill of Materials](#bill-of-materials)
3. **Prepare Arduino**: Install Arduino IDE and required libraries
4. **Wire Components**: Follow [WIRING_DIAGRAM.md](WIRING_DIAGRAM.md) carefully
5. **Configure**: Edit `config.h` for your specific hardware
6. **Upload Firmware**: Flash the code to Arduino Nano
7. **Test Components**: Run test sketches before connecting to machine
8. **Integrate**: Install into your espresso machine carefully
9. **Calibrate**: Follow setup wizard for pressure & temperature calibration
10. **Enjoy**: Brew perfect espresso! ☕

### Recommended Reading Order

1. [WIRING_DIAGRAM.md](WIRING_DIAGRAM.md) - Understand all connections
2. [HARDWARE_NOTES.md](HARDWARE_NOTES.md) - Component specifics
3. [config.h](config.h) - See available settings
4. [MENU_GUIDE.md](MENU_GUIDE.md) - How to use the interface

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
#define PUMP_FLOW_RATE          2.0      // ml/s at 9 bar (calibrate for your machine)
```

**Flow Rate Calibration:**
To calibrate your pump's flow rate for accurate volume estimation:
1. Run the pump at 9 bar for exactly 10 seconds into a measuring cup
2. Measure the volume in ml
3. Divide by 10 to get ml/s
   - Example: 20ml in 10s → 2.0 ml/s
4. Update `PUMP_FLOW_RATE` in `config.h`

**Runtime adjustment** (via encoder):
- Call `adjustTargetTime(delta)` from encoder handler
- Range: 5-60 seconds
- Volume automatically calculated: `Volume = Time × Flow Rate`

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

### Wake-Up Timer (Manual Time-Based)

Automatically enable the espresso machine after a set time **from bootup**. Perfect for starting the machine before you wake up in the morning!

**How to use:**
1. Power on Arduino (or press manual power switch ON)
2. Use menu or Serial command to set wake-up time
3. Set desired hours (1-12 hours from boot)
4. Machine will automatically:
   - Exit shutdown state if inactive
   - Begin heating
   - Be ready when timer reaches zero

**Example:**
```cpp
setWakeupTimer(8);  // Machine will wake up in 8 hours
setWakeupTimer(0);  // Disable timer
```

**Configuration:**
```cpp
#define WAKEUP_TIMER_ENABLED    0        // Set to 1 to enable feature
#define WAKEUP_MAX_HOURS        12       // Maximum: 12 hours
```

**Note:** Wake-up timer counts from Arduino boot time (uses `millis()` internally). Set the number of hours you want from the current moment.

### Display Output

During brewing, the controller displays:
- **Temperature**: Current vs target (°C)
- **Pressure**: Current vs target (bar)
- **Time**: Elapsed vs target time with progress bar
- **Volume**: Estimated volume (ml) based on pump flow rate and elapsed time
- **Status**: Current state (IDLE, HEATING, BREWING, STEAMING)

**Volume Calculation:**
- Estimated volume = Elapsed Time (seconds) × Pump Flow Rate (ml/s)
- Updated in real-time as brewing progresses
- No flow meter required!
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

## Contributing

This is an **open-source community project**! Contributions are welcome:

- 🐛 **Found a bug?** Open an issue and describe it
- ✨ **Have a feature idea?** Submit a pull request or discussion
- 📖 **Documentation improvements?** Help make it clearer
- 🔧 **Hardware optimizations?** Share your improvements
- 🤝 **Build your own version?** Fork it and customize!

All contributors are credited. Let's make espresso modding accessible to everyone!

## License

MIT License - Free to use, modify, and distribute
See LICENSE file for details

## Support & Community

- 📧 Issues and questions: Use GitHub Issues
- 💬 Discussions: Share your builds and modifications
- ☕ Share your results and feedback!

## Author

**George** - ESP espresso machine enthusiast building open-source espresso control systems with Claude AI

---

**Happy espresso brewing!** ☕ If you build this mod, please share your experience and contribute improvements back to the community!
