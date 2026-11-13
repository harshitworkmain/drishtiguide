# Hardware Specifications

This document provides detailed technical specifications for all hardware components used in the DrishtiGuide assistive system.

## 🔧 System Requirements

### Power Requirements
| Component | Voltage | Current | Power | Duration |
|-----------|---------|---------|-------|----------|
| ESP8266 (Transmitter) | 3.3V | 80mA | 0.26W | 48 hours |
| ESP8266 (Receiver) | 3.3V | 300mA | 0.99W | 12 hours |
| ESP32 (Main Controller) | 3.3V | 160mA | 0.53W | 36 hours |
| GPS Module | 3.3V | 25mA | 0.08W | 72 hours |
| MPU6050 | 3.3V | 3.8mA | 0.013W | 120 hours |
| Ultrasonic Sensor | 5V | 15mA | 0.075W | 96 hours |

### Environmental Specifications
| Parameter | Operating Range | Storage Range |
|-----------|----------------|---------------|
| Temperature | -10°C to +50°C | -20°C to +70°C |
| Humidity | 10% - 90% RH (non-condensing) | 5% - 95% RH |
| Altitude | Sea level to 3000m | Sea level to 12000m |

## 📡 Communication Specifications

### ESP-NOW Protocol
- **Frequency**: 2.4 GHz ISM band
- **Data Rate**: 250 kbps, 1 Mbps, 2 Mbps
- **Range**: Up to 50m (line of sight)
- **Latency**: <5ms transmission time
- **Power**: 80mA (TX), 70mA (RX), 20μA (sleep)

### WiFi Specifications
- **Standard**: 802.11 b/g/n
- **Frequency**: 2.4 GHz
- **Channels**: 1-13 (configurable)
- **Security**: WPA2-PSK
- **Range**: Up to 100m (open space)

## 🔌 Sensor Specifications

### HC-SR04 Ultrasonic Sensor
| Parameter | Value | Description |
|-----------|-------|-------------|
| Working Voltage | DC 5V | Power supply voltage |
| Working Current | 15mA | Typical operating current |
| Working Frequency | 40kHz | Ultrasonic frequency |
| Max Range | 400cm | Maximum detection distance |
| Min Range | 2cm | Minimum detection distance |
| Measuring Angle | 15° | Detection cone angle |
| Trigger Pulse | 10μs | Minimum trigger duration |
| Resolution | 1cm | Distance resolution |

### MPU6050 IMU Sensor
| Parameter | Value | Description |
|-----------|-------|-------------|
| Supply Voltage | 3.3V - 5V | Operating voltage range |
| Current Consumption | 3.8mA | Normal operating current |
| Accelerometer Range | ±2g, ±4g, ±8g, ±16g | Configurable range |
| Gyroscope Range | ±250, ±500, ±1000, ±2000°/s | Configurable range |
| ADC Resolution | 16-bit | High resolution conversion |
| Data Rate | 8Hz - 1kHz | Configurable sampling rate |
| I2C Address | 0x68 | Default I2C address |

### NEO-6M / NEO-M8N GPS Module
| Parameter | NEO-6M Value | NEO-M8N Value | Description |
|-----------|--------------|---------------|-------------|
| Supply Voltage | 3.3V - 5V | 3.3V - 5V | Operating voltage range |
| Current Consumption | 25mA | 35mA | Average power draw |
| Constellations | GPS | GPS, GLONASS, BeiDou, Galileo | GNSS satellite support |
| Channels | 50 | 72 | Concurrent tracking channels |
| Position Accuracy | 2.5m | 1.5m - 2.0m | Position accuracy (open sky) |
| Update Rate | 5Hz max | 10Hz - 18Hz max | Maximum update frequency |
| Default Baud Rate | 9600 bps | 9600 bps | Configurable serial rate |
| Protocol | NMEA 0183 | NMEA 0183 / UBX | Standard GPS protocol |
| Cold Start TTFF | 27s | 26s | Time to first fix |

## ⚡ Actuator Specifications

### Vibration Motors (Pancake Type)
| Parameter | Value | Description |
|-----------|-------|-------------|
| Operating Voltage | 3V - 3.7V | Optimal voltage range |
| Current Draw | 30-100mA | Depending on intensity |
| Vibration Amplitude | 0.8G | Typical vibration strength |
| Frequency | 10kHz - 15kHz | Vibration frequency |
| Dimensions | 10mm × 2.7mm | Size specifications |
| Weight | 2.5g | Motor mass |

### Buzzer Specifications
| Parameter | Value | Description |
|-----------|-------|-------------|
| Operating Voltage | 3V - 5V | Wide voltage range |
| Current Draw | 30mA | Typical operating current |
| Sound Pressure | 85dB | Sound output at 10cm |
| Frequency Range | 2kHz - 4kHz | Audio frequency range |
| Dimensions | 12mm × 9.5mm | Physical size |

## 💾 Microcontroller Specifications

### ESP8266 NodeMCU
| Parameter | Value | Description |
|-----------|-------|-------------|
| CPU | Tensilica L106 | 32-bit RISC processor |
| Clock Speed | 80MHz - 160MHz | Adjustable clock rate |
| Flash Memory | 4MB | Program storage |
| SRAM | 80KB | User available RAM |
| WiFi | 802.11 b/g/n | Wireless connectivity |
| GPIO | 17 | General purpose I/O pins |
| ADC | 10-bit | Analog-to-digital converter |
| PWM | 10-bit | Pulse width modulation |
| Power Consumption | 80mA (active), 20μA (sleep) | Low power modes |

### ESP32 DevKit V1
| Parameter | Value | Description |
|-----------|-------|-------------|
| CPU | Dual-core Xtensa LX6 | 32-bit processor |
| Clock Speed | 240MHz | High performance |
| Flash Memory | 4MB | Program storage |
| SRAM | 520KB | User available RAM |
| WiFi | 802.11 b/g/n | Wireless connectivity |
| Bluetooth | BLE + Classic | Dual-mode Bluetooth |
| GPIO | 36 | General purpose I/O pins |
| ADC | 12-bit | High precision ADC |
| DAC | 8-bit | Digital-to-analog converter |
| Touch Sensors | 10 | Capacitive touch inputs |
| Power Consumption | 160mA (active), 10μA (sleep) | Ultra-low power |

## 🔧 Circuit Design Considerations

### Power Supply Design
- **Voltage Regulation**: 3.3V LDO regulators for all modules
- **Battery Protection**: Over-charge/discharge protection circuits
- **Power Management**: Sleep modes and dynamic voltage scaling
- **Current Capacity**: Minimum 2A total supply capability

### Signal Integrity
- **Decoupling Capacitors**: 100nF ceramic capacitors at power pins
- **Pull-up Resistors**: 4.7kΩ for I2C communication
- **Filtering**: RC filters for analog sensor inputs
- **EMI Shielding**: Proper grounding and layout techniques

### Environmental Protection
- **Water Resistance**: IP65 enclosure rating recommended
- **Impact Protection**: Shock-absorbing mounting hardware
- **Temperature Management**: Heat dissipation for continuous operation

## 📐 Mechanical Specifications

### Enclosure Dimensions
| Component | Length | Width | Height | Material |
|-----------|--------|-------|--------|---------|
| Transmitter Node | 50mm | 30mm | 20mm | ABS Plastic |
| Receiver Unit | 80mm | 40mm | 25mm | ABS Plastic |
| Main Controller | 100mm | 60mm | 30mm | ABS Plastic |
| Battery Pack | 70mm | 35mm | 20mm | ABS Plastic |

### Mounting Specifications
- **Wearable Design**: Adjustable straps with quick-release buckles
- **Weight Distribution**: Balanced for comfortable long-term wear
- **Ergonomics**: Rounded edges, smooth surfaces for skin contact
- **Accessibility**: Easy battery replacement and maintenance access

## 🔌 Connector Specifications

### Pin Connectors
| Type | Pitch | Current Rating | Voltage Rating |
|------|-------|----------------|---------------|
| Dupont Jumper | 2.54mm | 1A | 250V |
| JST-XH | 2.50mm | 3A | 250V |
| Micro USB | N/A | 1.8A | 5V |
| Barrel Jack | 2.1mm | 5A | 12-24V |

### Cable Specifications
- **USB Cable**: 28AWG power, 34AWG data conductors
- **Jumper Wires**: 26AWG solid core, PVC insulation
- **Power Wires**: 22AWG stranded, silicone insulation
- **Signal Wires**: 30AWG ribbon cable for internal connections

---

For detailed schematics and PCB layouts, see the [Hardware Schematics](schematics/) section.