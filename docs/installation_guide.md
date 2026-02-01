# Installation Guide

Complete setup instructions for DrishtiGuide assistive system, covering hardware assembly, software installation, and system configuration.

## 📋 Prerequisites

### Hardware Requirements
- ESP8266 NodeMCU ×2 (transmitter and receiver)
- ESP32 DevKit V1 ×1 (main controller)
- HC-SR04 Ultrasonic Sensor ×1
- MPU6050 IMU Module ×1
- NEO-6M GPS Module ×1
- Miniature Vibration Motors ×5
- Electromagnetic Buzzer ×1
- 3.7V Li-ion Batteries (2000mAh+) ×3
- Jumper wires, breadboard, and prototyping materials

### Software Requirements
- Arduino IDE 1.8.19+ or PlatformIO
- Python 3.8+ (for development tools)
- Git for version control
- Serial terminal application

### Development Environment
- Windows 10/11, macOS 10.14+, or Linux (Ubuntu 18.04+)
- 8GB RAM minimum
- 2GB free disk space
- USB ports for device programming

## 🛠️ Hardware Assembly

### Step 1: Transmitter Node Assembly

#### Materials Required
- ESP8266 NodeMCU
- HC-SR04 Ultrasonic Sensor
- Jumper wires (male-female)
- Battery holder (2×18650)
- Enclosure (3D printed or project box)

#### Assembly Instructions
1. **Prepare ESP8266**
   ```
   Insert NodeMCU into breadboard
   Verify 3.3V and GND pins are accessible
   ```

2. **Connect Ultrasonic Sensor**
   ```
   NodeMCU  →  HC-SR04
   3.3V     →  VCC
   GND       →  GND
   D6        →  TRIG
   D5        →  ECHO
   ```

3. **Power Connection**
   ```
   Battery Pack (2×18650) → Voltage Regulator → NodeMCU VIN
   Use 3.7V Li-ion with 3.3V buck converter
   ```

4. **Testing**
   ```
   Power on transmitter
   Connect Serial Monitor at 115200 baud
   Verify distance readings appear every 200ms
   ```

### Step 2: Receiver Node Assembly

#### Materials Required
- ESP8266 NodeMCU
- 5× Miniature Vibration Motors
- 5× 2N2222 NPN Transistors
- 10kΩ Resistors ×5
- 220Ω Resistor ×1
- LED ×1
- Wearable enclosure or harness

#### Assembly Instructions

1. **Motor Driver Circuit** (for each motor)
   ```
   NodeMCU D1 → 1kΩ Resistor → 2N2222 Base
   2N2222 Emitter → GND
   2N2222 Collector → Motor Negative
   Motor Positive → 3.3V
   ```

2. **Complete Motor Connections**
   ```
   Motor 1: NodeMCU D1
   Motor 2: NodeMCU D2
   Motor 3: NodeMCU D3
   Motor 4: NodeMCU D4
   Motor 5: NodeMCU D5
   ```

3. **Status LED**
   ```
   NodeMCU D0 → 220Ω Resistor → LED Positive
   LED Negative → GND
   ```

4. **Power Supply**
   ```
   Battery Pack → NodeMCU VIN
   Ensure 3.3V regulator can handle motor current (500mA+)
   ```

5. **Testing**
   ```
   Upload receiver firmware
   Open Serial Monitor
   Test motor activation manually
   Verify ESP-NOW reception
   ```

### Step 3: Main Controller Assembly

#### Materials Required
- ESP32 DevKit V1
- MPU6050 IMU Module
- NEO-6M GPS Module
- Electromagnetic Buzzer
- RGB LEDs (red, green, blue)
- 4.7kΩ Resistors ×3
- Battery pack (3×18650)

#### Assembly Instructions

1. **I2C Connection to MPU6050**
   ```
   ESP32       →  MPU6050
   3.3V        →  VCC
   GND         →  GND
   GPIO21      →  SDA
   GPIO22      →  SCL
   ```

2. **UART Connection to GPS**
   ```
   ESP32       →  NEO-6M
   3.3V        →  VCC
   GND         →  GND
   GPIO16      →  TX (GPS RX)
   GPIO17      →  RX (GPS TX)
   ```

3. **Buzzer Connection**
   ```
   ESP32 GPIO27 → 220Ω Resistor → Buzzer Positive
   Buzzer Negative → GND
   ```

4. **Status LEDs**
   ```
   Status LED:  ESP32 GPIO2 → 220Ω → Green LED → GND
   Error LED:   ESP32 GPIO4 → 220Ω → Red LED → GND
   GPS LED:     ESP32 GPIO12 → 220Ω → Blue LED → GND
   ```

5. **Battery Monitoring** (optional)
   ```
   Battery+ → 10kΩ → ESP32 GPIO36 → 10kΩ → Battery-
   ```

6. **Testing**
   ```
   Upload main controller firmware
   Open Serial Monitor at 115200 baud
   Verify all sensors initialize
   Check WiFi hotspot appears
   ```

## 💻 Software Installation

### Step 1: Development Environment Setup

#### Option A: Arduino IDE Setup
1. **Download Arduino IDE**
   ```
   Visit: https://www.arduino.cc/en/software
   Download version 1.8.19 or later
   Install with USB drivers included
   ```

2. **Install ESP8266 Board Manager**
   ```
   File → Preferences → Additional Boards Manager URLs
   Add: http://arduino.esp8266.com/stable/package_esp8266com_index.json
   Tools → Board → Boards Manager → Search "ESP8266"
   Install "esp8266 by ESP8266 Community" (version 2.7.4+)
   ```

3. **Install ESP32 Board Manager**
   ```
   File → Preferences → Additional Boards Manager URLs
   Add: https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   Tools → Board → Boards Manager → Search "ESP32"
   Install "esp32 by Espressif Systems" (version 1.0.6+)
   ```

4. **Install Required Libraries**
   ```
   Tools → Manage Libraries
   Install: "TinyGPSPlus by Mikal Hart" (version 1.0.2+)
   Install: "MPU6050 by Electronic Cats" (version 0.5.3+)
   Install: "ArduinoJson by Benoit Blanchon" (version 6.19.4+)
   ```

#### Option B: PlatformIO Setup
1. **Install PlatformIO**
   ```
   Install VS Code
   Install PlatformIO extension
   Restart VS Code
   ```

2. **Create Project**
   ```
   PlatformIO Home → New Project
   Select ESP8266 or ESP32 platform
   Choose appropriate board (NodeMCU, DevKit V1)
   Add required libraries in platformio.ini
   ```

### Step 2: Project Setup

1. **Clone Repository**
   ```bash
   git clone https://github.com/harshitworkmain/drishtiguide.git
   cd drishtiguide
   ```

2. **Run Setup Script**
   ```bash
   # Linux/macOS
   chmod +x deployment/setup_dev_environment.sh
   ./deployment/setup_dev_environment.sh

   # Windows
   deployment\setup_dev_environment.bat
   ```

3. **Verify Installation**
   ```bash
   arduino-cli version
   arduino-cli core list
   arduino-cli lib list
   ```

## ⚙️ Configuration

### Transmitter Configuration

1. **Update MAC Address**
   ```cpp
   // In src/esp8266-nodes/transmitter/config.h
   uint8_t receiverAddress[] = {0x24, 0x6F, 0x28, 0x12, 0x34, 0x56};
   ```

2. **Configure Sensor Parameters**
   ```cpp
   #define MAX_DISTANCE 400     // Maximum detection range (cm)
   #define SAMPLE_RATE 200       // Sampling interval (ms)
   #define DEBUG_ENABLED true    // Enable debug output
   ```

### Receiver Configuration

1. **Configure Motor Mapping**
   ```cpp
   // In src/esp8266-nodes/receiver/config.h
   #define LEVEL_1_MIN 80       // Distance ranges for activation
   #define MOTOR_PULSE_DURATION 100  // Motor on time (ms)
   ```

2. **Safety Settings**
   ```cpp
   #define WATCHDOG_TIMEOUT 5000     // Reset if no data (ms)
   #define EMERGENCY_VIBRATION true   // Alert on data loss
   ```

### Main Controller Configuration

1. **WiFi Settings**
   ```cpp
   // In src/esp32-main-controller/config.h
   #define WIFI_AP_SSID "BlindStick_AP"
   #define WIFI_AP_PASSWORD "12345678"
   ```

2. **Fall Detection Settings**
   ```cpp
   #define FALL_LOW_G 0.3        // Free fall threshold
   #define FALL_HIGH_G 2.8       // Impact threshold
   #define FALL_WINDOW_MS 300      // Detection window
   ```

## 🚀 First-Time Setup

### Step 1: Hardware Testing

1. **Test Transmitter**
   ```
   - Connect via USB
   - Select Board: "NodeMCU 1.0 (ESP-12E Module)"
   - Select Port: COMx or /dev/ttyUSBx
   - Upload transmitter firmware
   - Open Serial Monitor (115200 baud)
   - Verify distance readings
   ```

2. **Test Receiver**
   ```
   - Connect via USB
   - Upload receiver firmware
   - Open Serial Monitor
   - Wait for ESP-NOW data
   - Test motor activation
   ```

3. **Test Main Controller**
   ```
   - Connect via USB
   - Upload main controller firmware
   - Open Serial Monitor
   - Verify sensor initialization
   - Check WiFi hotspot appears
   ```

### Step 2: System Integration

1. **Pair Devices**
   ```
   - Power on all three nodes
   - Verify ESP-NOW communication
   - Check distance-to-motor mapping
   - Test fall detection
   ```

2. **Web Interface Testing**
   ```
   - Connect smartphone/laptop to "BlindStick_AP"
   - Open browser to 192.168.4.1
   - Navigate to /web-interface/
   - Verify GPS data display
   - Test emergency alerts
   ```

### Step 3: Calibration

1. **Run Sensor Calibration**
   ```bash
   python3 tools/calibration/sensor_calibration.py /dev/ttyUSB0
   Follow on-screen instructions
   Apply calibration values to config files
   ```

2. **Distance Calibration**
   ```
   Place object at known distances (10, 50, 100cm)
   Compare measured vs actual values
   Update offset values in configuration
   ```

3. **IMU Calibration**
   ```
   Place device on level surface
   Run MPU6050 calibration routine
   Store calibration values in EEPROM
   ```

## 🔧 Advanced Configuration

### Custom Distance Thresholds
```cpp
// Modify in receiver config.h
#define CUSTOM_THRESHOLDS {120, 90, 60, 30, 15}
#define NUM_THRESHOLDS 5
```

### WiFi Network Configuration
```cpp
// Connect to existing network instead of AP mode
#define WIFI_MODE_STATION
#define WIFI_SSID "YourNetwork"
#define WIFI_PASSWORD "YourPassword"
```

### GPS Configuration
```cpp
// Update GPS baud rate and update frequency
#define GPS_BAUD_RATE 115200
#define GPS_UPDATE_RATE 5000  // 2Hz updates
```

## 🧪 Testing and Validation

### Functional Testing Checklist

- [ ] Transmitter detects obstacles at all ranges
- [ ] Receiver activates correct motors
- [ ] ESP-NOW communication is reliable
- [ ] Main controller detects falls
- [ ] GPS obtains location fix
- [ ] Web interface displays data
- [ ] Emergency alerts work
- [ ] Battery monitoring functions
- [ ] System recovers from errors

### Performance Testing

1. **Range Testing**
   ```
   Test ultrasonic detection from 2cm to 400cm
   Verify accuracy at various distances
   Document minimum and maximum range
   ```

2. **Battery Life Testing**
   ```
   Run system continuously from full battery
   Record operating time
   Measure power consumption at different states
   ```

3. **Environmental Testing**
   ```
   Test in different lighting conditions
   Test with various obstacle materials
   Verify operation in temperature range
   Test with user movement patterns
   ```

## 🔍 Troubleshooting

### Common Issues and Solutions

#### Upload Failures
```
Problem: "Failed to upload to board"
Solutions:
- Check COM port and board selection
- Press BOOT button during upload (ESP8266)
- Check USB cable and driver installation
- Try different USB port
```

#### Sensor Not Working
```
Problem: No sensor readings
Solutions:
- Verify power and ground connections
- Check I2C pull-up resistors
- Test with known-good sensor
- Verify configuration settings
```

#### WiFi Not Starting
```
Problem: WiFi hotspot not visible
Solutions:
- Check antenna connection
- Verify WiFi credentials
- Restart ESP32
- Check for interference
```

#### ESP-NOW Communication Issues
```
Problem: Transmitter/Receiver not communicating
Solutions:
- Update MAC addresses
- Check distance between nodes (<50m)
- Verify both nodes on same channel
- Check for WiFi interference
```

## 📚 Additional Resources

### Documentation
- [System Architecture](../docs/system_architecture.md)
- [API Documentation](../docs/api_documentation.md)
- [Hardware Specifications](../hardware/bill_of_materials/hardware_specifications.md)

### Community Support
- GitHub Issues: Report bugs and request features
- Forum: DrishtiGuide community discussions
- Wiki: Additional tutorials and examples

### Development Tools
- [Calibration Tool](../tools/calibration/sensor_calibration.py)
- [Data Logger](../tools/data_logger/sensor_logger.py)
- [Flash Utility](../tools/flash_utility/flash_utility.py)

---

*For production deployment and mass assembly considerations, see the [Deployment Guide](deployment_guide.md).*