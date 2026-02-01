# Transmitter Node Schematic

## 📍 Overview
Ultrasonic sensor transmitter node for obstacle detection using ESP8266 NodeMCU.

## 🔌 Component Connections

### ESP8266 NodeMCU to HC-SR04 Ultrasonic Sensor
```
NodeMCU          HC-SR04
-------          ----------
3.3V      →      VCC
GND        →      GND
D6 (GPIO12)→      TRIG
D5 (GPIO14)→      ECHO
```

### Optional Components
```
NodeMCU          Optional Components
-------          -----------------
D0 (GPIO16)→      Status LED + 220Ω Resistor → GND
A0         →      Battery Monitor (voltage divider)
```

## 📋 Detailed Pinout

| NodeMCU Pin | Function | Connected To | Notes |
|--------------|----------|---------------|-------|
| 3V3 | Power | HC-SR04 VCC | 3.3V power supply |
| GND | Ground | HC-SR04 GND | Common ground |
| D6 (GPIO12) | Trigger | HC-SR04 TRIG | Ultrasonic trigger output |
| D5 (GPIO14) | Echo | HC-SR04 ECHO | Ultrasonic echo input |
| D0 (GPIO16) | Status LED | LED Anode + 220Ω | Status indication |
| A0 | Battery | Voltage Divider | Battery monitoring (optional) |

## ⚡ Power Supply

### Primary Power (3.3V)
- **Source**: 3.3V from NodeMCU onboard regulator
- **Current**: ~15mA (HC-SR04 active)
- **Voltage Range**: 3.0V - 3.6V

### Battery Monitoring (Optional)
```
Battery+ → 10kΩ → A0 → 10kΩ → GND
```
- **Voltage Range**: 0V - 3.3V (A0 input)
- **Battery Voltage**: 0V - 6.6V (with divider)
- **Formula**: V_battery = A0_voltage * 2

## 📡 Signal Characteristics

### Ultrasonic Trigger Pulse
```
Pulse Width: 10μs minimum
Polarity: Positive
Voltage Level: 3.3V
Rise/Fall Time: <100ns
```

### Ultrasonic Echo Pulse
```
Pulse Width: 100μs - 30ms (proportional to distance)
Polarity: Positive
Voltage Level: 3.3V
Timeout: 30ms (no echo)
```

## 📊 Timing Diagram

### Distance Measurement Cycle
```
Time: 0ms    0.01ms    Variable    0.01ms + Variable
      │        │          │            │
      │  Trigger│  Pulse  │   Echo     │  Processing
      │  10μs  │  Travel  │   Return   │  & Calculate
      │         │          │            │
   Low│         │ Low      │ High       │ Low
──────┴─────────┴──────────┴─────────────┴────────────────────
```

## 🛡️ Protection Circuits

### Input Protection (ECHO Pin)
```
NodeMCU D5 → 1kΩ Resistor → HC-SR04 ECHO
               → 5.1V Zener Diode → GND (protection)
               → 10pF Capacitor → GND (noise filtering)
```

### LED Protection
```
NodeMCU D0 → 220Ω Resistor → LED Anode → LED Cathode → GND
Current: (3.3V - 2.0V) / 220Ω = 5.9mA
```

## 📐 PCB Layout Guidelines

### Component Placement
1. **ESP8266 Module**: Center of board
2. **HC-SR04**: Edge of board (facing outward)
3. **Status LED**: Top edge (visible)
4. **Battery Connector**: Bottom edge

### Routing Guidelines
- **Power Traces**: Minimum 0.5mm width
- **Signal Traces**: 0.2mm width minimum
- **Keep Echo Line Short**: <50mm from HC-SR04 to ESP8266
- **Ground Plane**: Fill unused areas with ground

### Via Placement
- **Power Vias**: 0.8mm diameter, placed every 10mm
- **Signal Vias**: 0.6mm diameter, minimal use
- **Thermal Vias**: Around ESP8266 for heat dissipation

## 📏 Mechanical Dimensions

### PCB Size: 50mm × 30mm
- **Thickness**: 1.6mm standard FR4
- **Mounting Holes**: 3mm diameter, corners
- **Connector Placement**: Edge-mounted for easy access

### HC-SR04 Mounting
- **Orientation**: Facing outward from PCB edge
- **Mounting**: 4x M2 screws at sensor corners
- **Clearance**: 5mm minimum from other components

## 🧪 Testing Points

### Test Point Locations
| TP | Function | Expected Value | Test Condition |
|----|----------|----------------|-----------------|
| TP1 | 3.3V Power | 3.2V - 3.4V | Power on |
| TP2 | Trigger Output | 0V/3.3V pulse | During measurement |
| TP3 | Echo Input | 0V/3.3V pulse | During echo |
| TP4 | Battery Voltage | 0V - 6.6V | Battery connected |
| TP5 | Ground | 0V | Always |

### Test Equipment Needed
- **Multimeter**: Voltage and continuity testing
- **Oscilloscope**: Pulse timing verification
- **Power Supply**: For bench testing (3.3V, 500mA)

## 🔧 Troubleshooting Guide

### Common Issues

#### No Distance Readings
**Possible Causes:**
- HC-SR04 not powered
- Trigger signal not reaching sensor
- Echo signal not reaching ESP8266

**Check:**
1. Verify 3.3V at HC-SR04 VCC pin
2. Check trigger pulse with oscilloscope
3. Verify echo signal return
4. Check all ground connections

#### Inconsistent Readings
**Possible Causes:**
- Electrical noise on echo line
- Power supply instability
- Target surface reflection issues

**Solutions:**
1. Add 10pF capacitor on echo line
2. Use shielded cable for ultrasonic sensor
3. Add 100μF capacitor at power input
4. Ensure perpendicular target surface

#### Short Range Detection Only
**Possible Causes:**
- Echo timeout too short
- Weak echo signal
- Target too absorbing

**Solutions:**
1. Increase echo timeout in software
2. Check target surface (metal works best)
3. Verify sensor alignment

## 📋 Bill of Materials (Schematic)

| Ref | Component | Value | Package | Quantity | Notes |
|-----|-----------|---------|----------|-----------|--------|
| U1 | ESP8266 NodeMCU | ESP-12E | 1 | Main controller |
| U2 | HC-SR04 | Module | 1 | Ultrasonic sensor |
| R1 | Resistor | 220Ω | 1 | LED current limiting |
| R2 | Resistor | 10kΩ | 1 | Battery divider (optional) |
| R3 | Resistor | 10kΩ | 1 | Battery divider (optional) |
| D1 | LED | 3mm Red | 1 | Status indicator |
| J1 | Header | 4-pin | 1 | HC-SR04 connection |
| J2 | Header | 2-pin | 1 | Battery connection |
| TP1-5 | Test Point | - | 5 | Testing access points |

## 🔌 Connector Pinouts

### HC-SR04 Connector (J1)
```
Pin 1: VCC    → 3.3V
Pin 2: TRIG   → D6 (GPIO12)
Pin 3: ECHO   → D5 (GPIO14)
Pin 4: GND    → GND
```

### Battery Connector (J2)
```
Pin 1: VCC+   → Battery Positive
Pin 2: VCC-   → Battery Negative/GND
```

---

*This schematic provides the complete design for the transmitter node. For assembly instructions and testing procedures, see the main project documentation.*