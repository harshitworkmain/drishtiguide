# Cloud-Connected Architecture & Firebase Telemetry

## 🌐 Overview

The **DrishtiGuide Guardian Cloud System** extends the assistive stick from a purely localized sensor into a globally connected IoT health & safety platform. By streaming high-priority telemetry to **Firebase Realtime Database (RTDB)** and serving an ultra-low-latency, responsive web dashboard deployed on **Vercel** ([https://stick-theta.vercel.app/](https://stick-theta.vercel.app/)), caregivers and family members can monitor the user's location and receive emergency alerts anywhere in the world.

```
┌────────────────────────────────────────────────────────────────────────────────────────┐
│                        GUARDIAN CLOUD TELEMETRY PIPELINE                               │
└────────────────────────────────────────────────────────────────────────────────────────┘

 [ ESP32 Cloud Stick ]
   - HC-SR04 Proximity (<10cm) ──► Obstacle LED (GPIO27)
   - MPU6050 6-Axis Motion      ──► Dual-Threshold Fall Detection (<0.3g, >2.8g)
   - NEO-M8N / NEO-6M GNSS      ──► Lat / Long Geolocation
   - Buzzer (GPIO4)             ──► Fall & Inactivity Acoustic Alarms
           │
           │ HTTPS / WebSockets (WiFi Station Mode)
           ▼
 [ Google Firebase Realtime Database ]
   stick-ff202-default-rtdb.asia-southeast1.firebasedatabase.app
   │
   ├── /stick/latitude      : 12.840600
   ├── /stick/longitude     : 80.153400
   ├── /stick/fallDetected  : true | false
   └── /stick/lastFallTime  : "14:32:15"
           │
           │ Real-Time Value Events (db.ref('stick').on('value', ...))
           ▼
 [ Guardian Web Dashboard (Vercel) ]
   https://stick-theta.vercel.app/
   - Dark-mode glassmorphic Tailwind UI
   - Live Leaflet.js Map with CartoDB Dark Matter tiles
   - Emergency Pulse Glow & Alert Status
   - Caregiver "RESOLVE EMERGENCY" Action Button
           │
           │ Caregiver Clicks [RESOLVE EMERGENCY]
           ▼
   Writes { fallDetected: false } back to Firebase
           │
           ▼
   ESP32 detects remote resolution, silences buzzer, resets alarm state!
```

---

## 🏗️ System Components

### 1. ESP32 NodeMCU-32S All-In-One Stick
- **Firmware Location**: [src/esp32-cloud-stick/esp32-cloud-stick.ino](../src/esp32-cloud-stick/esp32-cloud-stick.ino)
- **Primary Duties**:
  - Continuous ultrasonic scanning on pins `TRIG=32` and `ECHO=33`. Lights up `OBSTACLE_LED=27` when distance $<10\text{ cm}$.
  - Fall detection via MPU6050 on I2C (`SDA=21`, `SCL=22`).
  - Serial GPS parsing on UART2 (`RX=16`, `TX=17`).
  - Non-blocking acoustic alert driving on `BUZZER_PIN=4`.
  - Cloud synchronization via `Firebase_ESP_Client` library.

### 2. Google Firebase Realtime Database
- **Host**: `stick-ff202-default-rtdb.asia-southeast1.firebasedatabase.app`
- **Authentication**: Database secret or Firebase Token Authentication.
- **Latency**: Sub-second synchronization across connected clients worldwide.
- **Data Model**:
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

### 3. Guardian Vercel Frontend
- **Live URL**: [https://stick-theta.vercel.app/](https://stick-theta.vercel.app/)
- **Source Code**: [src/cloud-interface/index.html](../src/cloud-interface/index.html)
- **Features**:
  - Tailwind CSS glassmorphic aesthetic (`Plus Jakarta Sans` font).
  - Leaflet.js interactive dark-mode map panning automatically to live GPS coordinates.
  - Emergency banner and red pulsing glow upon critical fall events.
  - One-click **RESOLVE EMERGENCY** button updating Firebase to clear alarms remotely.

---

## 🔄 The Closed-Loop Safety Cycle

```
1. Physical Fall Event Occurs
   └─► Total Acceleration drops < 0.3g (Free-fall)
   └─► Followed within 300ms by collision spike > 2.8g (Impact)
   └─► ESP32 marks fallDetected = true

2. Local Notification & Cloud Upload
   └─► Buzzer triggers immediate double-beep alarm (GPIO 4)
   └─► ESP32 immediately writes to Firebase:
         /stick/fallDetected = true
         /stick/lastFallTime = "HH:MM:SS"
         /stick/latitude     = <currentLat>
         /stick/longitude    = <currentLon>

3. Caregiver Dashboard Activation
   └─► Vercel web app receives instant Firebase update via WebSocket
   └─► Dashboard enters emergency mode:
         - Status badge turns into pulsing red alert: "🚨 EMERGENCY: FALL DETECTED"
         - Map container acquires animated emergency-glow shadow
         - Marker centers on exact location
         - "RESOLVE EMERGENCY" button appears

4. Remote Resolution
   └─► Caregiver assists the user and clicks [RESOLVE EMERGENCY]
   └─► Web app pushes /stick/fallDetected = false to Firebase
   └─► ESP32 Stick detects status change, silences the buzzer, and resumes normal monitoring
```

---

## ⚙️ Setup and Deployment

### 1. Flash the ESP32 Cloud Stick
1. Connect ESP32 to computer.
2. Edit credentials in [src/esp32-cloud-stick/config.h](../src/esp32-cloud-stick/config.h):
   ```cpp
   #define WIFI_SSID "your_ssid"
   #define WIFI_PASSWORD "your_password"
   #define FIREBASE_HOST "your-project-default-rtdb.firebaseio.com"
   #define FIREBASE_AUTH "your_database_secret"
   ```
3. Compile and flash using Arduino CLI:
   ```bash
   arduino-cli compile --fqbn esp32:esp32:devkitv1 src/esp32-cloud-stick/
   arduino-cli upload --fqbn esp32:esp32:devkitv1 --port /dev/ttyUSB0 src/esp32-cloud-stick/
   ```

### 2. Deploy Web Interface to Vercel
1. Ensure the `databaseURL` in [src/cloud-interface/index.html](../src/cloud-interface/index.html) matches your Firebase instance.
2. Deploy with Vercel CLI:
   ```bash
   cd src/cloud-interface
   vercel deploy --prod
   ```
3. Your live monitor is immediately accessible from any phone, tablet, or PC.

