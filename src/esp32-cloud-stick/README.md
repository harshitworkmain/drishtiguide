# ESP32 Cloud-Connected Smart Stick

Standalone, all-in-one Smart Assistive Cane firmware integrating obstacle detection, kinematic fall detection, GPS geolocation, and real-time bidirectional synchronization with **Firebase Realtime Database** and the **Guardian Cloud Web Dashboard** on Vercel.

## 🎯 Architecture Overview

Unlike the distributed 3-node configuration (which distributes obstacle sensing and haptic feedback across cane and wearable via ESP-NOW), the **Cloud Stick** is an integrated single-MCU solution running directly on an ESP32 NodeMCU-32S:

- **Direct Obstacle Sensing**: HC-SR04 mounted directly on the cane (`TRIG`: GPIO 32, `ECHO`: GPIO 33).
- **Proximity Indicator**: `OBSTACLE_LED` (GPIO 27) lights up whenever distance $< 10\text{ cm}$.
- **Kinematic Fall Detection**: 6-Axis MPU6050 algorithm ($<0.3g$ free-fall, $>2.8g$ collision impact within $300\text{ ms}$).
- **GPS Coordinates**: NEO-M8N / NEO-6M GPS on HardwareSerial UART2 (`RX`: GPIO 16, `TX`: GPIO 17).
- **Acoustic Emergency Alarms**: Buzzer on GPIO 4 for fall alerts and stationary post-fall alarms.
- **Cloud Backend**: Direct Wi-Fi Station connection to Firebase Realtime Database.
- **Bidirectional Remote Safety Loop**: Real-time push of alerts to Firebase (`/stick/fallDetected = true`) and detection of remote resolution when a caregiver clicks **RESOLVE EMERGENCY** on the Vercel web app (`https://stick-theta.vercel.app/`).

## 📋 Pinout & Connections

| Component | ESP32 Pin | Interface / Mode | Purpose |
| :--- | :--- | :--- | :--- |
| **HC-SR04 TRIG** | GPIO 32 | Digital Output | 10µs ultrasonic trigger pulse |
| **HC-SR04 ECHO** | GPIO 33 | Digital Input | Echo pulse duration measurement |
| **Obstacle LED** | GPIO 27 | Digital Output | Proximity alert indicator ($<10\text{ cm}$) |
| **Buzzer** | GPIO 4 | Digital Output | Fall & inactivity audible alarm |
| **MPU6050 SDA** | GPIO 21 | I2C Data | 6-DOF acceleration & gyro telemetry |
| **MPU6050 SCL** | GPIO 22 | I2C Clock | I2C clock (400kHz) |
| **NEO-M8N GPS TX** | GPIO 16 | UART2 RX (Serial2) | GPS NMEA sentence stream |
| **NEO-M8N GPS RX** | GPIO 17 | UART2 TX (Serial2) | GPS module configuration |

## ⚙️ Configuration (`config.h`)

Copy `config.example.h` to `config.h` if creating a new configuration:
```bash
cp src/esp32-cloud-stick/config.example.h src/esp32-cloud-stick/config.h
```

Edit `config.h` before flashing:

```cpp
#define WIFI_SSID "your-wifi-ssid"
#define WIFI_PASSWORD "your-wifi-password"

#define FIREBASE_HOST "your-project-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "your-database-secret-or-token"
```

## 🌐 Firebase Data Schema

The firmware maintains real-time synchronization under the `/stick` root node:

```json
{
  "stick": {
    "latitude": 12.840600,
    "longitude": 80.153400,
    "fallDetected": false,
    "lastFallTime": "14:32:15"
  }
}
```

## 🚀 Building & Flashing

### Using Arduino IDE
1. Install **ESP32 Board Package** (`esp32 by Espressif Systems`).
2. Install required libraries from Library Manager:
   - `Firebase ESP Client by Mobizt` (v4.x+)
   - `TinyGPSPlus by Mikal Hart`
   - `MPU6050 by Electronic Cats`
3. Select board: `ESP32 Dev Module` or `NodeMCU-32S`.
4. Select port and click **Upload**.

### Using Arduino CLI
```bash
arduino-cli compile --fqbn esp32:esp32:devkitv1 src/esp32-cloud-stick/
arduino-cli upload --fqbn esp32:esp32:devkitv1 --port /dev/ttyUSB0 src/esp32-cloud-stick/
```

## 📱 Cloud Dashboard Integration

This firmware is designed to operate seamlessly with the Guardian Web Interface hosted at:
- **Live URL**: [https://stick-theta.vercel.app/](https://stick-theta.vercel.app/)
- **Source Code**: [src/cloud-interface/index.html](../cloud-interface/index.html)
