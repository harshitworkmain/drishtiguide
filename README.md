# 👁️ DrishtiGuide - Smart Assistive System for the Visually Impaired

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform: ESP](https://img.shields.io/badge/Platform-ESP8266%2F%2B%20%7C%20ESP32-blue.svg)](https://www.espressif.com/en/products/socs)
[![Language: C++](https://img.shields.io/badge/Language-C++-blue.svg)](https://www.arduino.cc/)
[![Build Status](https://github.com/harshitworkmain/drishtiguide/workflows/CI/badge.svg)](https://github.com/harshitworkmain/drishtiguide/actions)
[![Last Commit](https://img.shields.io/github/last-commit/harshitworkmain/drishtiguide.svg)](https://github.com/harshitworkmain/drishtiguide/commits/main)

An innovative IoT-based assistive device designed to enhance navigation and safety for visually impaired individuals through real-time obstacle detection, haptic feedback, and comprehensive health monitoring.

## 🎯 Features

### 🌟 Core Capabilities
- **Real-time Obstacle Detection**: Ultrasonic sensors detect obstacles up to 4 meters
- **Intelligent Haptic Feedback**: 5-level vibration motor system for intuitive distance indication
- **Fall Detection System**: Advanced algorithm using MPU6050 accelerometer/gyroscope
- **GPS Location Tracking**: Real-time positioning with web-based monitoring
- **Emergency Alerts**: Buzzer notifications for fall detection and inactivity

### 🔧 Technical Highlights
- **Wireless Communication**: Low-latency ESP-NOW protocol for sensor-to-actor communication
- **Multi-node Architecture**: Distributed ESP8266 nodes for scalable design
- **Edge Computing**: Real-time sensor processing and decision making
- **Web Interface**: RESTful API for remote monitoring and configuration
- **Power Optimization**: Efficient sleep modes and battery management

## 🏗️ System Architecture

```
┌─────────────────┐    ESP-NOW    ┌─────────────────┐
│   Transmitter   │ ◄─────────────► │    Receiver     │
│  (Ultrasonic)   │               │  (Haptic Motors) │
│   ESP8266       │               │     ESP8266     │
└─────────────────┘               └─────────────────┘
                                          │
                                          │
                                          ▼
┌─────────────────────────────────────────────────────┐
│           Main Controller (ESP32)                  │
│  ┌─────────────┐ ┌─────────────┐ ┌───────────────┐ │
│  │   MPU6050   │ │    GPS      │ │   WiFi AP     │ │
│  │Fall Detection│ │ Location    │ │  Web Server   │ │
│  └─────────────┘ └─────────────┘ └───────────────┘ │
│               │    Buzzer Alerts                     │
└─────────────────────────────────────────────────────┘
```

## 📁 Project Structure

```
drishtiguide/
├── 📁 src/                     # Source code
│   ├── 📁 esp8266-nodes/       # ESP8266 sensor nodes
│   ├── 📁 esp32-main-controller/ # Main ESP32 controller
│   └── 📁 web-interface/       # Web UI for monitoring
├── 📁 hardware/                # Hardware designs & specs
├── 📁 docs/                   # Documentation
├── 📁 tests/                  # Test suites
├── 📁 tools/                  # Development utilities
└── 📁 deployment/            # Production setup
```

## 🚀 Quick Start

### Prerequisites
- Arduino IDE 1.8.19+ or PlatformIO
- ESP8266 (2x) and ESP32 development boards
- Required sensors and components (see [Hardware Specifications](hardware/bill_of_materials/))

### Installation

1. **Clone the repository**
```bash
git clone https://github.com/harshitworkmain/drishtiguide.git
cd drishtiguide
```

2. **Install Arduino dependencies**
- ESP8266 Board Manager (2.7.4+)
- ESP32 Board Manager (1.0.6+)
- Required libraries (see requirements.txt)

3. **Configure hardware**
- Update MAC addresses in `src/esp8266-nodes/transmitter/config.h`
- Set WiFi credentials in `src/esp32-main-controller/config.h`

4. **Flash firmware**
```bash
# Flash transmitter node
arduino-cli compile --fqbn esp8266:esp8266:nodemcuv2 src/esp8266-nodes/transmitter/
arduino-cli upload --fqbn esp8266:esp8266:nodemcuv2 --port /dev/ttyUSB0 src/esp8266-nodes/transmitter/

# Flash receiver node
arduino-cli compile --fqbn esp8266:esp8266:nodemcuv2 src/esp8266-nodes/receiver/
arduino-cli upload --fqbn esp8266:esp8266:nodemcuv2 --port /dev/ttyUSB1 src/esp8266-nodes/receiver/

# Flash main controller
arduino-cli compile --fqbn esp32:esp32:devkitv1 src/esp32-main-controller/
arduino-cli upload --fqbn esp32:esp32:devkitv1 --port /dev/ttyUSB2 src/esp32-main-controller/
```

5. **Monitor system**
- Connect to "BlindStick_AP" WiFi hotspot
- Access monitoring interface at `http://192.168.4.1/gps`

## 📊 Technical Specifications

### Performance Metrics
| Metric | Value |
|--------|-------|
| Detection Range | 2cm - 400cm |
| Response Time | <100ms |
| Battery Life | 48+ hours |
| Wireless Range | 50m+ (ESP-NOW) |
| GPS Accuracy | ±3 meters |

### Hardware Components
- **MCUs**: ESP8266 (NodeMCU) ×2, ESP32 (DevKit V1)
- **Sensors**: HC-SR04 Ultrasonic, MPU6050 IMU, NEO-6M GPS
- **Actuators**: 5× Vibration Motors, Buzzer
- **Communication**: ESP-NOW, WiFi 802.11 b/g/n

## 🧪 Testing

### Unit Tests
```bash
cd tests/unit_tests
python -m pytest test_sensor_algorithms.py -v
```

### Integration Tests
```bash
cd tests/integration_tests
python -m pytest test_espnow_communication.py -v
```

## 📖 Documentation

- [System Architecture](docs/system_architecture.md)
- [API Documentation](docs/api_documentation.md)
- [Hardware Specifications](hardware/bill_of_materials/components.csv)
- [Installation Guide](docs/installation_guide.md)
- [Troubleshooting](docs/troubleshooting.md)

## 🛠️ Development

### Code Style
- Follow Arduino C++ conventions
- Use meaningful variable names
- Add comprehensive comments
- Modular function design

### Contributing
1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📜 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🤝 Acknowledgments

- **ESP-NOW Protocol** - Espressif Systems for reliable wireless communication
- **TinyGPS++ Library** - Mikal Hart for GPS processing
- **MPU6050 Library** - Electronic Cats for IMU integration
- **Assistive Technology Community** - For inspiration and feedback

## 👨‍💻 Author

**Harshit Singh** - *Embedded Systems Developer* - [GitHub Profile](https://github.com/harshitworkmain)
 
---

⚡ **Built with passion for accessible technology and IoT innovation**

## 📞 Support

For support, please open an issue on [GitHub Issues](https://github.com/harshitworkmain/drishtiguide/issues) or contact [harshit.workmain@gmail.com](mailto:harshit.workmain@gmail.com).