# Hardware Integration Notes

## Component Specifications

### MAX31855 Thermocouple Amplifier
- **Protocol**: SPI (3-wire)
- **Resolution**: 0.25°C
- **Range**: -200 to +700°C
- **Conversion Time**: ~220ms
- **Supply**: 3.3V-5V
- **Cold Junction**: Internal compensation
- **Wiring**:
  - CS → Pin 10
  - CLK → Pin 13 (shared SPI)
  - MOSI → Pin 11 (shared SPI)
  - MISO → Pin 12 (shared SPI)
  - GND, VCC

### HX710B Load Cell Amplifier
- **Protocol**: Custom serial (DT/CLK)
- **Resolution**: 24-bit
- **Sampling Rate**: 80Hz
- **Sensitivity**: Depends on load cell rating
- **Supply**: 5V
- **Conversion Time**: ~12.5ms
- **Wiring**:
  - DOUT → Pin 4
  - CLK → Pin 5
  - GND, VCC

**Typical Pressure Transducer Pairing:**
- 0-16 bar load cell (4-20mA converter not needed with HX710B)
- Excitation voltage: 5V from Arduino

### YF-S201 Flow Meter
- **Sensor Type**: Hall effect
- **Output**: Open collector (NPN)
- **Flow Range**: 0.3-6 L/min
- **Calibration**: 450 pulses/liter
- **Supply**: 5V-24V
- **Wiring**:
  - Signal → Pin 2 (interrupt capable)
  - GND
  - VCC (5V-24V, Arduino powered is 5V)
- **Notes**: 
  - Pull-up resistor usually built-in
  - Water-sealed connector typically M16

### Rotary Encoder
- **Type**: Mechanical or optical
- **Contacts**: 3 (A, B, Common)
- **Switch**: Integrated push button
- **Resolution**: Typically 20-30 PPR (pulses per rotation)
- **Wiring**:
  - CLK → Pin 6
  - DT → Pin 7
  - SW → Pin 8
  - GND
- **Notes**: Use pull-up resistors (10K) if not built-in

### Solid State Relay (SSR)
- **Type**: 10-30A, 3-32VDC input, 24-240VAC output
- **Control**: PWM from pin 3 (0-255 maps to 0-100%)
- **Typical Part**: Fotek SSR-25 DA or similar
- **Input Circuit**: 
  - Arduino pin 3 → 1K resistor → SSR+ (input positive)
  - GND → SSR- (input negative)
- **Output Circuit**:
  - AC1 → Heating element (one side)
  - AC2 → AC supply (other side)
- **Heat Dissipation**: Mount on heatsink with thermal compound

### MCP4131 Digital Potentiometer
- **Protocol**: SPI (3-wire)
- **Resistance**: 10kΩ
- **Positions**: 256 (0-255)
- **Resolution**: ~39Ω per step
- **Supply**: 5V
- **Wiring**:
  - CS → Pin A2
  - CLK → Pin A3 (can share with SPI)
  - MOSI → Pin A4 (can share with SPI)
  - GND, VCC
  - A (endpoint) → SCR input
  - W (wiper) → Ground or signal
  - B (endpoint) → Signal output

**SCR Integration:**
- MCP4131 wiper output controls SCR gate voltage
- Gate resistor: ~10Ω-100Ω to SCR gate
- Snubber circuit recommended across SCR output
- Typical SCR: BTA16 or equivalent

### TFT Display (ILI9341 Example)
- **Protocol**: SPI (4-wire)
- **Resolution**: 240×320 or 320×480
- **Supply**: 3.3V (logic), 5V (backlight)
- **Wiring** (SPI):
  - CS → Pin A5
  - DC → Pin A6 (Data/Command)
  - RST → Pin A7 (Reset)
  - CLK → Pin 13 (shared SPI)
  - MOSI → Pin 11 (shared SPI)
  - GND, VCC
- **Backlight**: PWM capable for brightness control
- **Notes**: May need level shifter if 3.3V only

## Electrical Considerations

### Power Supply
- Arduino Nano: 5V, ~500mA typical
- Display: 100-200mA (backlight ~50-100mA)
- Sensors: ~50mA combined
- SSR/SCR outputs: Logic-only draws minimal current
- **Total**: ~1A USB supply recommended, or external 5V 2A supply

### Grounding
- Single point ground reference critical for sensor accuracy
- Use star grounding pattern
- Keep signal grounds separate from power grounds where possible

### PCB Layout
- Keep SPI traces short and grouped
- HX710B: Differential pairs important for signal integrity
- Clock signals away from analog inputs
- Use ground planes if possible

### Noise Mitigation
- 100nF bypass capacitors on all IC power pins
- 10µF electrolytic on main power rail
- Ferrite beads on sensor supply lines
- Use shielded cables for signals

## Assembly Tips

1. **Thermocouple Installation**:
   - Mount thermowell in thermoblock
   - Use thermal paste for good contact
   - Weatherproof connector for steam environment

2. **Pressure Transducer**:
   - Mount on pump outlet or group head
   - Use thread sealant (PTFE tape)
   - Pressure relief valve recommended (9-10 bar)

3. **Flow Meter**:
   - Install inline in water supply line
   - Check flow direction arrow
   - Filter upstream to prevent particle clogging

4. **Housing**:
   - Espresso machines generate heat and steam
   - Consider sealed/weather-resistant enclosure
   - Silicone potting compound for humid environment
   - Keep display in dry area

## Testing Checklist

- [ ] SPI communication verified (use test sketches)
- [ ] Thermocouple reads reasonable values
- [ ] Pressure sensor calibrated to 0 bar
- [ ] Flow meter counts pulses correctly
- [ ] Encoder rotates without noise
- [ ] SSR switches heating on/off
- [ ] MCP4131 wiper moves across range
- [ ] Display shows all elements
- [ ] Serial debug output looks correct
- [ ] All switches respond to input
