# 👁️ DrishtiGuide - Smart Assistive System for the Visually Impaired

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform: ESP](https://img.shields.io/badge/Platform-ESP8266%2F%2B%20%7C%20ESP32-blue.svg)](https://www.espressif.com/en/products/socs)
[![Language: C++](https://img.shields.io/badge/Language-C++-blue.svg)](https://www.arduino.cc/)
[![Build Status](https://github.com/harshitworkmain/drishtiguide/workflows/CI/badge.svg)](https://github.com/harshitworkmain/drishtiguide/actions)
[![Last Commit](https://img.shields.io/github/last-commit/harshitworkmain/drishtiguide.svg)](https://github.com/harshitworkmain/drishtiguide/commits/main)

Integrated IoT assistive ecosystem for real-time obstacle evasion & safety monitoring. Combines HC-SR04, MPU6050, NEO-6M & ESP-NOW with ESP32/ESP8266/RPi4 orchestrator. Designed as a synchronized smart-cane & haptic wearable module.

## 🌟 Tech Stack

### Hardware Ecosystem
<p align="center">
  <img src="assets/images/techstack/hardware-stack/esp32.png" height="80" alt="ESP32" />
  <img src="assets/images/techstack/hardware-stack/hc-sr04-ultrasonic-sensor.png" height="80" alt="HC-SR04" />
  <img src="assets/images/techstack/hardware-stack/imu-mpu6050.png" height="80" alt="MPU6050" />
  <img src="assets/images/techstack/hardware-stack/neo6m-gps-module.png" height="80" alt="NEO-6M GPS" />
  <img src="assets/images/techstack/hardware-stack/r-pi.png" height="80" alt="Raspberry Pi" />
  <img src="assets/images/techstack/hardware-stack/logitech-720p-webcam.png" height="80" alt="Logitech Webcam" />
</p>

### Software & Frameworks
<p align="center">
  <img src="assets/images/techstack/software-stack/c.png" height="60" alt="C" />
  <img src="assets/images/techstack/software-stack/c++.png" height="60" alt="C++" />
  <img src="assets/images/techstack/software-stack/arduino.png" height="60" alt="Arduino" />
  <img src="assets/images/techstack/software-stack/python.png" height="60" alt="Python" />
  <img src="assets/images/techstack/software-stack/open-cv.png" height="60" alt="OpenCV" />
  <img src="assets/images/techstack/software-stack/ultralytics-yolov8.png" height="60" alt="YOLOv8" />
  <img src="assets/images/techstack/software-stack/react.png" height="60" alt="React" />
  <img src="assets/images/techstack/software-stack/javascript.png" height="60" alt="JavaScript" />
  <img src="assets/images/techstack/software-stack/html.png" height="60" alt="HTML" />
  <img src="assets/images/techstack/software-stack/css.png" height="60" alt="CSS" />
  <img src="assets/images/techstack/software-stack/linux.png" height="60" alt="Linux" />
  <img src="assets/images/techstack/software-stack/r-pi-os.png" height="60" alt="Raspberry Pi OS" />
</p>

## 🎯 Features

### 🌟 Core Capabilities
- **Real-time Obstacle Detection**: Ultrasonic sensors detect obstacles up to 4 meters
- **Intelligent Haptic Feedback**: 5-level vibration motor system for intuitive distance indication
- **Fall Detection System**: Advanced algorithm using MPU6050 accelerometer/gyroscope
- **GPS Location Tracking**: Real-time positioning with web-based monitoring
- **Emergency Alerts**: Buzzer notifications for fall detection and inactivity
- **AI Scene Perception**: YOLOv8-driven object detection using a logitech webcam feed via RPi4.

### 🔧 Technical Highlights
- **Wireless Communication**: Low-latency ESP-NOW protocol for sensor-to-actor communication
- **Multi-node Architecture**: Distributed ESP8266 nodes for scalable design
- **Edge Computing**: Real-time sensor processing and decision making
- **Web Interface**: RESTful API for remote monitoring and configuration
- **Power Optimization**: Efficient sleep modes and battery management

## 🏗️ System Architecture

![System Architecture](assets/images/architecture.png)

*(Note on the Architecture Map: While the visual diagram outlines the core ESP routing, the total ecosystem operates via a synchronized multi-tier node structure:)*
* **ESP8266 Smart-Cane Node (Transmitter):** Scans the environment using HC-SR04 ultrasonic sensors to gather obstacle proximity data.
* **ESP8266 Wearable Hub (Receiver):** Located in the jacket/wristband, providing 3D spatial haptic feedback via an array of vibration motors.
* **ESP32 Edge Orchestrator (Main Controller):** Hosts the local web server dashboard, performs heavy fall detection (MPU6050) & geo-location tagging (GPS NEO-6M), acting as the central nexus triggering buzzer alerts.
* **Raspberry Pi 4 Vision Gateway:** Anchors the advanced visual pipeline, running OpenCV and Ultralytics YOLOv8 inference models on the Logitech webcam feed to provide powerful real-time object detection and augmented scene reality.

### Logic & Flow
![Flow Diagram](assets/images/flow-diagram.png)

## 📁 Project Structure

```
drishtiguide/
├── 📁 assets/                  # Media assets, testing results, diagrams
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
- Raspberry Pi 4 (for YOLO AI vision)
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
- **MCUs**: Raspberry Pi 4, ESP32 (DevKit V1), ESP8266 (NodeMCU) ×2
- **Sensors**: HC-SR04 Ultrasonic, MPU6050 IMU, NEO-6M GPS, Logitech 720p Webcam
- **Actuators**: 5× Vibration Motors, Buzzer
- **Communication**: ESP-NOW, WiFi 802.11 b/g/n

## 🔬 AI Vision Testing & Validation

Powered by **Ultralytics YOLOv8** and **OpenCV**, the object perception system robustly identifies real-world objects in live time, providing critical spatial understanding to the user via edge-compute on the Raspberry Pi. Here are the AI field testing results:

<p align="center">
  <img src="assets/images/testing/yolov8-testing-result-1.png" width="30%" />
  <img src="assets/images/testing/yolov8-testing-result-2.png" width="30%" />
  <img src="assets/images/testing/yolov8-testing-result-3.png" width="30%" />
  <img src="assets/images/testing/yolov8-testing-result-4.png" width="30%" />
  <img src="assets/images/testing/yolov8-testing-result-5.png" width="30%" />
  <img src="assets/images/testing/yolov8-testing-result-6.png" width="30%" />
  <img src="assets/images/testing/yolov8-testing-result-7.png" width="30%" />
</p>

## 🧪 System Testing

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

## 👥 About the Team

<p align="center">
  <img src="assets/images/team-photo-with-project.png" width="45%" alt="Team Photo with Project" />
  <img src="assets/images/team-photo-receiving-first-prize.png" width="45%" alt="Team Photo Receiving First Prize" />
</p>

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