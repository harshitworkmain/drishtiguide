# Main Controller Schematic

## 📍 Overview
Comprehensive ESP32-based main controller integrating MPU6050 IMU, NEO-6M GPS, WiFi web server, and emergency buzzer for complete assistive system functionality.

## 🔌 Component Connections

### ESP32 DevKit V1 to MPU6050 IMU
```
ESP32           MPU6050
------           ----------
3.3V      →      VCC
GND        →      GND
GPIO21     →      SDA
GPIO22     →      SCL
```

### ESP32 to NEO-6M GPS Module
```
ESP32           NEO-6M
------           ----------
3.3V      →      VCC
GND        →      GND
GPIO16     →      RX  (GPS TX → ESP32 RX)
GPIO17     →      TX  (GPS RX → ESP32 TX)
```

### ESP32 to Buzzer and Status LEDs
```
ESP32           Components
------           ----------
GPIO27     →      Buzzer (+) → Buzzer (-) → GND
GPIO2      →      Status LED + 220Ω → GND
GPIO4      →      Error LED + 220Ω → GND  
GPIO12     →      GPS LED + 220Ω → GND
```

### ESP32 to Battery Monitoring
```
ESP32           Battery Monitor
------           ----------------
GPIO36     →      Voltage Divider Output
3.3V      →      Voltage Divider Input
```

## 📋 Detailed Pinout

| ESP32 Pin | Function | Connected Component | Notes |
|------------|----------|-------------------|-------|
| 3V3 | Power | MPU6050, GPS, Buzzer, LEDs | 3.3V regulated |
| GND | Ground | All components | Common ground |
| GPIO21 | I2C SDA | MPU6050 SDA | I2C data line |
| GPIO22 | I2C SCL | MPU6050 SCL | I2C clock line |
| GPIO16 | UART2 RX | GPS TX | GPS data input |
| GPIO17 | UART2 TX | GPS RX | GPS configuration |
| GPIO27 | PWM Output | Buzzer | Emergency alerts |
| GPIO2 | GPIO Output | Status LED | System status |
| GPIO4 | GPIO Output | Error LED | Error indication |
| GPIO12 | GPIO Output | GPS LED | GPS fix status |
| GPIO36 | ADC Input | Battery monitor | Battery voltage |

## ⚡ Power Requirements

### System Power Budget
```
ESP32 (Active):         160mA @ 3.3V = 528mW
MPU6050 (Active):       3.8mA @ 3.3V = 12.5mW  
NEO-6M GPS (Active):    25mA @ 3.3V = 82.5mW
Buzzer (Active):          30mA @ 3.3V = 99mW
LEDs (All On):           15mA @ 3.3V = 49.5mW
-------------------------------------------
Total Active Power:       233.8mA = 771mW
```

### Battery Management
```
Battery Type: 3.7V 3000mAh Li-ion
Buck Converter: 3.7V → 3.3V @ 1A
Run Time: ~12 hours (active monitoring)
Standby Current: ~10mA
Standby Time: ~300 hours
```

### Voltage Divider for Battery Monitoring
```
Battery+ → R1(10kΩ) → GPIO36 → R2(10kΩ) → GND
Voltage at GPIO36 = Battery_Voltage × R2/(R1+R2) = Battery_Voltage/2
ADC Range: 0-3.3V maps to 0-6.6V battery voltage
Resolution: 6.6V/4096 = 1.6mV per ADC count
```

## 🔧 Circuit Sections

### MPU6050 IMU Interface
```
ESP32 GPIO21 ←→ SDA → MPU6050 SDA
ESP32 GPIO22 ←→ SCL → MPU6050 SCL

Pull-up Resistors:
SDA: 4.7kΩ to 3.3V
SCL: 4.7kΩ to 3.3V

Decoupling Capacitors:
100nF: Close to MPU6050 VCC
10μF: Bulk decoupling
```

### GPS Module Interface
```
UART2 Configuration:
Baud Rate: 9600 bps (default)
Data Bits: 8
Stop Bits: 1
Parity: None

Signal Levels: 3.3V CMOS
Timeout: 1 second (no fix)
```

### Buzzer Driver Circuit
```
ESP32 GPIO27 → 220Ω Resistor → Buzzer (+) → Buzzer (-) → GND

Buzzer Specifications:
Type: Electromagnetic
Voltage: 3.3V
Current: 30mA
Sound Pressure: 85dB @ 10cm
Frequency: 2.5kHz
```

### LED Indicators
```
Status LED (GPIO2):   Green → System OK
Error LED (GPIO4):    Red → System Error  
GPS LED (GPIO12):    Blue → GPS Fix

Current: (3.3V-2.0V)/220Ω = 5.9mA per LED
```

## 📊 Signal Characteristics

### I2C Communication (MPU6050)
```
Clock Frequency: 400kHz (fast mode)
Device Address: 0x68 (7-bit)
Data Format: 16-bit signed integers
Sample Rate: 100Hz (configurable)
```

### UART Communication (GPS)
```
Baud Rate Options: 4800, 9600 (default), 19200, 38400
Protocol: NMEA 0183
Update Rate: 1Hz (configurable to 5Hz)
Sentences: GGA, RMC, GSA, GSV, VTG
```

### PWM Output (Buzzer)
```
PWM Frequency: 1kHz - 10kHz (configurable)
Duty Cycle: 50% for square wave
Resolution: 8-bit (0-255)
Volume Control: 0 (off) to 255 (maximum)
```

## 🛡️ Protection and Filtering

### Power Supply Protection
```
Input Protection:
- TVS Diode: 3.6V (overvoltage)
- PTC Fuse: 1A hold, 2A trip
- Reverse Polarity Protection: Diode

Decoupling:
- 100μF Electrolytic: Bulk decoupling
- 100nF Ceramic: High frequency decoupling
- 10nF Ceramic: Local decoupling per IC
```

### Signal Protection
```
I2C Protection:
- 4.7kΩ Pull-up resistors
- 100pF Capacitors on SDA/SCL
- ESD Protection Diodes

UART Protection:
- 100Ω Series Resistors
- TVS Diodes on RX/TX lines
- Common Mode Choke (optional)
```

## 📐 PCB Layout Guidelines

### PCB Dimensions: 80mm × 60mm
- **Thickness**: 1.6mm FR4
- **Layers**: 2-layer (standard)
- **Copper Weight**: 1oz per layer

### Component Placement
```
Top Layer:
┌─────────────────────────────────┐
│ ESP32 DevKit              │
│ MPU6050                  │
│ GPS Module                 │
│ Status LEDs                │
│ Buzzer                    │
└─────────────────────────────────┘

Bottom Layer:
┌─────────────────────────────────┐
│ Battery Connector          │
│ Power Regulation           │
│ Decoupling Capacitors     │
│ Test Points              │
└─────────────────────────────────┘
```

### Routing Guidelines
- **Power Traces**: 1mm minimum width
- **Signal Traces**: 0.2mm minimum
- **Ground Plane**: Fill both layers with thermal vias
- **I2C Lines**: Keep short, parallel routing
- **UART Lines**: Differential pair if possible

### Thermal Management
- **Heat Sinks**: None required (low power)
- **Thermal Vias**: Under ESP32 and voltage regulator
- **Copper Areas**: Maximized for heat dissipation

## 📏 Mechanical Design

### Enclosure Requirements
```
Internal Dimensions: 85mm × 65mm × 30mm
Material: ABS Plastic (IP65 rated)
Mounting: 4x M3 screws, corners
Ventilation: Small vents for GPS antenna
Access: Removable cover for battery
```

### Antenna Placement
```
GPS Antenna:
- Position: Top surface
- Clearance: 10mm from metal
- View: Sky access preferred
- Mounting: Internal or external

WiFi Antenna:
- Position: Side or top
- Orientation: Vertical preferred
- Ground Plane: Keep clear underneath
```

## 🧪 Testing and Calibration

### MPU6050 Calibration Points
```
Test Points:
TP_IMU_SDA: I2C data line
TP_IMU_SCL: I2C clock line
TP_IMU_VCC: Power supply (3.3V)
TP_IMU_GND: Ground reference

Calibration Steps:
1. Place device on level surface
2. Collect 1000 accelerometer samples
3. Calculate offsets for X, Y, Z axes
4. Store offsets in EEPROM/Flash
```

### GPS Test Points
```
Test Points:
TP_GPS_RX: UART receive from GPS
TP_GPS_TX: UART transmit to GPS
TP_GPS_VCC: Power supply (3.3V)
TP_GPS_GND: Ground reference

Test Procedures:
1. Verify 9600 baud communication
2. Check NMEA sentence parsing
3. Validate GPS fix acquisition
4. Test time-to-first-fix (<30s cold)
```

## 🔧 Troubleshooting Guide

### Common Issues

#### GPS Not Getting Fix
**Check:**
1. 3.3V power at GPS module
2. UART communication (9600 baud)
3. Antenna connection and placement
4. NMEA data reception

**Solutions:**
- Ensure clear sky view for antenna
- Check baud rate compatibility
- Verify UART wiring (TX↔RX cross-connection)
- Extend GPS timeout in software

#### MPU6050 Not Responding
**Check:**
1. I2C address (0x68)
2. Pull-up resistors on SDA/SCL
3. Power supply stability
4. I2C bus speed

**Solutions:**
- Scan I2C bus for device detection
- Check pull-up resistor values (4.7kΩ)
- Reduce I2C clock speed
- Verify power supply decoupling

#### WiFi Not Starting
**Check:**
1. 3.3V power to ESP32
2. Antenna connection
3. Crystal oscillator
4. Flash memory integrity

**Solutions:**
- Check antenna solder connection
- Verify crystal frequency and loading
- Re-flash firmware
- Enable WiFi AP mode debugging

#### Buzzer Not Working
**Check:**
1. PWM output on GPIO27
2. Buzzer polarity
3. Series resistor value
4. Power supply current

**Solutions:**
- Test PWM output with oscilloscope
- Verify buzzer specifications
- Increase PWM frequency
- Check for short circuits

## 📋 Bill of Materials (Main Controller)

| Ref | Component | Value/Type | Package | Quantity | Notes |
|-----|-----------|-------------|----------|-----------|--------|
| U1 | ESP32 DevKit | ESP32-WROOM-32 | 1 | Main controller |
| U2 | MPU6050 | GY-521 | 1 | IMU sensor |
| U3 | NEO-6M | GPS Module | 1 | GPS receiver |
| R1-R3 | Resistor | 4.7kΩ | 3 | I2C pull-ups |
| R4 | Resistor | 220Ω | 1 | Buzzer current limit |
| R5-R7 | Resistor | 220Ω | 3 | LED current limit |
| R8-R9 | Resistor | 10kΩ | 2 | Battery divider |
| C1 | Capacitor | 100μF, 16V | 1 | Power filtering |
| C2-C5 | Capacitor | 100nF, 50V | 4 | Decoupling |
| D1-D3 | LED | 3mm (G/R/B) | 3 | Status indicators |
| BZ1 | Buzzer | Electromagnetic | 1 | Emergency alert |
| J1 | Header | 4-pin | 1 | GPS connector |
| J2 | Header | 2-pin | 1 | Power input |
| TP1-TP12 | Test Point | - | 12 | Testing access |

## 🔌 Connector Specifications

### I2C Header (MPU6050)
```
Pin 1: VCC   → 3.3V
Pin 2: GND   → GND
Pin 3: SDA   → GPIO21
Pin 4: SCL   → GPIO22
```

### GPS Header (NEO-6M)
```
Pin 1: VCC   → 3.3V
Pin 2: GND   → GND
Pin 3: TX    → GPIO16 (RX)
Pin 4: RX    → GPIO17 (TX)
```

### Power Header
```
Pin 1: VCC+  → Battery positive
Pin 2: GND   → Battery negative
```

---

*This schematic provides the complete design for the main controller unit. For integration with transmitter/receiver nodes and system testing, see the main project documentation.*