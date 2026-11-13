# Cloud-Connected Smart Stick Schematic (All-In-One ESP32)

## 📍 Overview
Integrated single-controller schematic for the DrishtiGuide Cloud Assistive Cane. Uses an ESP32 NodeMCU-32S to directly manage ultrasonic proximity sensing, obstacle LED indication, 6-DOF IMU fall detection, NEO-M8N GPS geolocation, emergency buzzer alerts, and direct Wi-Fi cloud synchronization with Firebase Realtime Database.

## 🔌 Component Interconnects

### ESP32 DevKit / NodeMCU-32S Pin Connections
```
                     ┌────────────────────────┐
                     │   ESP32 NodeMCU-32S    │
                     │                        │
       HC-SR04 TRIG  ◄── GPIO32               │
       HC-SR04 ECHO  ──► GPIO33               │
   Obstacle Red LED  ◄── GPIO27               │
   Emergency Buzzer  ◄── GPIO4                │
                     │                        │
       MPU6050 SDA   ◄─► GPIO21 (I2C SDA)     │
       MPU6050 SCL   ──► GPIO22 (I2C SCL)     │
                     │                        │
       NEO-M8N TX    ──► GPIO16 (UART2 RX)    │
       NEO-M8N RX    ◄── GPIO17 (UART2 TX)    │
                     │                        │
         Power 3.3V  ──► 3V3 Rail             │
             Ground  ──► Common GND           │
                     └────────────────────────┘
```

## 📋 Complete Pinout Table

| ESP32 Pin | Function | Peripheral Pin | Direction / Mode | Electrical Details |
| :--- | :--- | :--- | :--- | :--- |
| **GPIO32** | Digital Output | HC-SR04 `TRIG` | Out (10µs pulse) | 3.3V CMOS Level |
| **GPIO33** | Digital Input | HC-SR04 `ECHO` | In (pulse width) | 5V to 3.3V level-safe ($1\text{k}\Omega$ series resistor) |
| **GPIO27** | Digital Output | Obstacle Alert LED | Out (Active HIGH) | $220\Omega$ current-limiting resistor to Red LED |
| **GPIO4** | Digital Output | Emergency Buzzer | Out (Active HIGH) | $220\Omega$ resistor to 5V/3.3V Electromagnetic Buzzer |
| **GPIO21** | I2C Data (SDA) | MPU6050 `SDA` | Bidirectional | $4.7\text{k}\Omega$ pullup to 3.3V rail |
| **GPIO22** | I2C Clock (SCL)| MPU6050 `SCL` | Out (400kHz) | $4.7\text{k}\Omega$ pullup to 3.3V rail |
| **GPIO16** | UART2 RX | NEO-M8N / NEO-6M `TX` | In (9600 baud) | GPS NMEA sentences input |
| **GPIO17** | UART2 TX | NEO-M8N / NEO-6M `RX` | Out (9600 baud) | GPS configuration commands |
| **3V3** | Regulated Power| All Sensor VCC | Power Rail | 3.3V DC regulated |
| **GND** | Power Ground | Common Ground | Reference | Star ground configuration |

## ⚡ Power Requirements & Budget

```
ESP32 Active (WiFi TX + Processing):  160 - 240mA @ 3.3V (~660mW)
NEO-M8N GPS Receiver (Tracking):       25 - 35mA  @ 3.3V (~100mW)
MPU6050 6-Axis Motion Sensor:          3.8mA      @ 3.3V (~12.5mW)
HC-SR04 Ultrasonic Sensor (Active):    15mA       @ 5V / 3.3V (~75mW)
Obstacle Warning LED (GPIO27):         6mA        @ 3.3V (~20mW)
Buzzer Alert (GPIO4, Pulsed):          30mA       @ 3.3V (~100mW)
------------------------------------------------------------------
Total Peak Active Consumption:        ~320mA (1.05W Peak)
Total Nominal Operating Current:      ~200mA (0.66W Average)
```

### Battery Run-Time Estimation
- **3.7V 3000mAh 18650 Li-ion Cell** with 3.3V buck regulator ($90\%$ efficiency):
  $$\text{Effective Capacity} \approx \frac{3.7\text{V} \times 3000\text{mAh} \times 0.90}{3.3\text{V}} \approx 3027\text{mAh}$$
  $$\text{Nominal Runtime} \approx \frac{3027\text{mAh}}{200\text{mA}} \approx 15.1\text{ hours continuous operation}$$

## 🛡️ Circuit Protection Considerations

1. **HC-SR04 Echo Line Protection**:
   - While modern HC-SR04-P modules accept 3.3V, standard 5V HC-SR04 echo lines output 5V logic.
   - Insert a voltage divider ($1\text{k}\Omega / 2\text{k}\Omega$) or $1\text{k}\Omega$ series resistor between HC-SR04 ECHO and ESP32 GPIO33 to protect the ESP32 input gate.
2. **Decoupling**:
   - $100\mu\text{F}$ electrolytic bulk capacitor across the 3.3V / VIN input rail.
   - $100\text{nF}$ ceramic bypass capacitors placed within 10mm of MPU6050 and GPS power pins.
3. **Buzzer Isolation**:
   - A flyback clamping diode (1N4148) across inductive electromagnetic buzzer terminals prevents reverse-EMF transients from resetting the ESP32.

