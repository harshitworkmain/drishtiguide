# System Architecture

## 🏗️ Overview

DrishtiGuide employs a distributed multi-node architecture designed for reliability, scalability, and real-time performance. The system consists of three main components that communicate wirelessly to provide comprehensive assistance for visually impaired users.

## 📐 System Components

### 1. Transmitter Node (ESP8266 + Ultrasonic Sensor)
```
┌─────────────────────────────────────┐
│          ESP8266 NodeMCU           │
│  ┌───────────────────────────────┐ │
│  │      HC-SR04 Ultrasonic      │ │
│  │        Sensor Array           │ │
│  └───────────────────────────────┘ │
│  ┌───────────────────────────────┐ │
│  │      ESP-NOW Controller      │ │
│  │         (Transmitter)         │ │
│  └───────────────────────────────┘ │
│           Power Management          │
└─────────────────────────────────────┘
```

**Responsibilities:**
- Continuous distance measurement (2cm - 400cm range)
- Sensor data filtering and validation
- Wireless transmission via ESP-NOW protocol
- Power optimization for battery operation

### 2. Receiver Node (ESP8266 + Haptic Motors)
```
┌─────────────────────────────────────┐
│          ESP8266 NodeMCU           │
│  ┌───────────────────────────────┐ │
│  │     Haptic Motor Array        │ │
│  │    M1 → M5 (Progressive)      │ │
│  └───────────────────────────────┘ │
│  ┌───────────────────────────────┐ │
│  │       ESP-NOW Slave          │ │
│  │        (Receiver)             │ │
│  └───────────────────────────────┘ │
│        Motor Driver Circuitry       │
└─────────────────────────────────────┘
```

**Responsibilities:**
- Real-time distance data reception
- Progressive haptic feedback calculation
- 5-level motor activation control
- User safety through intuitive feedback

### 3. Main Controller (ESP32 + Sensors + WiFi)
```
┌─────────────────────────────────────────────────────────┐
│                   ESP32 DevKit V1                       │
│  ┌─────────────┐ ┌─────────────┐ ┌───────────────────┐  │
│  │   MPU6050   │ │    GPS      │ │    WiFi AP        │  │
│  │  IMU Sensor │ │  Module     │ │  Web Server       │  │
│  │             │ │             │ │  + API           │  │
│  └─────────────┘ └─────────────┘ └───────────────────┘  │
│  ┌─────────────┐ ┌─────────────┐ ┌───────────────────┐  │
│  │ Fall Detect │ │   Buzzer    │ │  Power Mgmt       │  │
│  │  Algorithm  │ │   Alerts    │ │  System           │  │
│  └─────────────┘ └─────────────┘ └───────────────────┘  │
└─────────────────────────────────────────────────────────┘
```

**Responsibilities:**
- Advanced fall detection using 6-axis IMU
- GPS location tracking and logging
- WiFi hotspot for remote monitoring
- Emergency alert management
- System health monitoring

### 4. Cloud-Connected All-In-One Stick (ESP32 + Firebase + Vercel)
```
┌─────────────────────────────────────────────────────────┐
│              ESP32 NodeMCU-32S Cloud Stick              │
│  ┌─────────────┐ ┌─────────────┐ ┌───────────────────┐  │
│  │   HC-SR04   │ │   MPU6050   │ │    NEO-M8N/6M     │  │
│  │ Ultrasonic  │ │  IMU Sensor │ │    GPS Module     │  │
│  │ (TRIG:32    │ │ (SDA:21     │ │ (RX:16            │  │
│  │  ECHO:33)   │ │  SCL:22)    │ │  TX:17)           │  │
│  └─────────────┘ └─────────────┘ └───────────────────┘  │
│  ┌─────────────┐ ┌─────────────┐ ┌───────────────────┐  │
│  │Obstacle LED │ │   Buzzer    │ │  Firebase RTDB    │  │
│  │  (GPIO 27)  │ │  (GPIO 4)   │ │  Client Engine    │  │
│  └─────────────┘ └─────────────┘ └───────────────────┘  │
└─────────────────────────────────────────────────────────┘
                             │
                  WiFi (HTTPS / WebSockets)
                             ▼
┌─────────────────────────────────────────────────────────┐
│               Firebase Realtime Database                │
│    /stick/latitude, /stick/longitude, /stick/fallDetected│
└─────────────────────────────────────────────────────────┘
                             │
                             ▼
┌─────────────────────────────────────────────────────────┐
│             Guardian Cloud Web Dashboard                │
│            https://stick-theta.vercel.app/              │
│      (Live Map, Real-Time Alerts, Remote Reset)         │
└─────────────────────────────────────────────────────────┘
```

**Responsibilities:**
- Single-board standalone smart cane operation
- Real-time obstacle warning LED (<10cm proximity)
- Dual-stage kinematic fall detection & acoustic alarming
- Sub-second global synchronization via Firebase Realtime Database
- Bi-directional remote emergency resolution by caregivers via Vercel dashboard

## 🔄 Data Flow Architecture

```
Environment Obstacles
        ↓
Ultrasonic Sensor (Sampling @ 5Hz)
        ↓
Distance Measurement (cm)
        ↓
ESP-NOW Transmission (0.5ms latency)
        ↓
┌─────────────────┐    ┌─────────────────┐
│   Transmitter   │ →  │    Receiver     │
└─────────────────┘    └─────────────────┘
                              ↓
                      Haptic Processing
                              ↓
                      Motor Activation
                              ↓
                    User Haptic Feedback
```

### Simultaneous Health Monitoring
```
User Movement
     ↓
MPU6050 IMU Data (100Hz)
     ↓
Fall Detection Algorithm
     ↓
┌─────────────────┐    GPS Location    ┌─────────────────┐
│   Buzzer Alert  │ ← ──────────────── │   Web Server    │
└─────────────────┘    WiFi Hotspot     └─────────────────┘
     ↓                                      ↓
Emergency Alert                     Remote Monitoring
```

## 📊 Communication Protocols

### ESP-NOW Protocol
- **Type**: Proprietary WiFi-based protocol
- **Speed**: 250 kbps
- **Range**: Up to 50m line-of-sight
- **Latency**: <5ms transmission time
- **Power**: Ultra-low power consumption

### Data Packet Structure
```cpp
typedef struct {
    int distance;        // Distance in centimeters
    uint32_t timestamp;  // Millisecond timestamp
    uint8_t node_id;     // Node identifier
    uint8_t battery;     // Battery level (%)
} SensorPacket;
```

### WiFi Web Interface
- **Protocol**: HTTP/1.1 over WiFi
- **Format**: JSON REST API
- **Endpoints**: `/gps`, `/status`, `/config`
- **Authentication**: WPA2-PSK

## ⚡ Performance Characteristics

### Real-time Performance
| Operation | Response Time | Frequency |
|-----------|---------------|-----------|
| Distance Measurement | 200ms | 5 Hz |
| ESP-NOW Transmission | <5ms | 5 Hz |
| Haptic Activation | <50ms | Event-driven |
| Fall Detection | 100ms | 10 Hz |
| GPS Update | 1000ms | 1 Hz |

### Power Consumption
| Component | Active Power | Sleep Power | Battery Life |
|-----------|--------------|-------------|--------------|
| ESP8266 TX | 80mA | 20μA | 48 hours |
| ESP8266 RX | 70mA | 20μA | 52 hours |
| ESP32 | 160mA | 10μA | 36 hours |
| Haptic Motors | 150mA | 0mA | 12 hours |

## 🔧 Configuration Management

### Distributed Configuration
Each node maintains its own configuration:

```cpp
// Transmitter Config
#define TRIG_PIN D6
#define ECHO_PIN D5
#define SAMPLE_RATE 200  // ms
#define MAX_DISTANCE 400  // cm

// Receiver Config
#define MOTOR1 D1
#define MOTOR2 D2
// ... MOTOR5 D5
#define ACTIVATION_THRESHOLDS {100, 80, 60, 40, 20}

// Main Controller Config
#define FALL_THRESHOLD 2.8g
#define INACTIVITY_TIMEOUT 10000  // ms
#define WIFI_SSID "BlindStick_AP"
```

## 🛡️ Safety & Reliability

### Fail-Safe Mechanisms
1. **Communication Loss Detection**: Timeout handling for ESP-NOW
2. **Sensor Validation**: Plausibility checks for distance readings
3. **Battery Monitoring**: Low-power operation modes
4. **Watchdog Timer**: System crash recovery
5. **Data Validation**: Checksum verification for packets

### Error Handling Strategy
```cpp
// ESP-NOW Error Handling
if (esp_now_send(receiverAddress, (uint8_t *) &myData, sizeof(myData)) != 0) {
    Serial.println("Transmission failed");
    retry_count++;
    if (retry_count > MAX_RETRIES) {
        enter_safe_mode();
    }
}
```

## 📈 Scalability Considerations

### Multi-Node Support
The architecture supports:
- **Multiple Transmitters**: For 360° obstacle detection
- **Multiple Receivers**: For distributed haptic feedback
- **Mesh Networking**: Extended range via repeater nodes
- **Cloud Integration**: Remote data storage and analysis

### Future Enhancements
- **Machine Learning**: Adaptive obstacle recognition
- **Voice Feedback**: Audio guidance integration
- **Mobile App**: Smartphone control and monitoring
- **Cloud Analytics**: Pattern recognition and alerts

---

This architecture ensures DrishtiGuide provides reliable, real-time assistance while maintaining power efficiency and user safety.