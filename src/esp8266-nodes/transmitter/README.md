# Transmitter Node

Ultrasonic sensor node that measures obstacles and transmits distance data via ESP-NOW protocol.

## 🎯 Overview

The transmitter node continuously scans the environment using an HC-SR04 ultrasonic sensor and wirelessly transmits distance measurements to the receiver node for haptic feedback processing.

## 🔧 Hardware Requirements

- **ESP8266 NodeMCU** or similar ESP8266-based board
- **HC-SR04 Ultrasonic Sensor** (2cm - 400cm range)
- **Power Source**: 3.7V Li-ion battery or USB power
- **Mounting**: Adjustable bracket for optimal sensor positioning

## 📋 Pin Connections

| HC-SR04 Pin | ESP8266 Pin | Description |
|-------------|-------------|-------------|
| VCC | 3.3V | Power supply |
| GND | GND | Ground |
| TRIG | D6 | Trigger pulse output |
| ECHO | D5 | Echo pulse input |

## ⚙️ Configuration

Edit `config.h` to customize settings:

```cpp
// Detection range (cm)
#define MAX_DISTANCE 400
#define MIN_DISTANCE 2

// Sampling rate (ms)
#define SAMPLE_RATE 200

// Receiver MAC address
uint8_t receiverAddress[] = {0x24, 0x6F, 0x28, 0x12, 0x34, 0x56};
```

## 🚀 Installation

1. **Update Receiver MAC Address**
   - Upload receiver sketch to get MAC address from serial monitor
   - Update `receiverAddress[]` in `config.h`

2. **Hardware Setup**
   ```
   HC-SR04 → ESP8266
   VCC → 3.3V
   GND → GND  
   TRIG → D6
   ECHO → D5
   ```

3. **Flash Firmware**
   ```bash
   arduino-cli compile --fqbn esp8266:esp8266:nodemcuv2 .
   arduino-cli upload --fqbn esp8266:esp8266:nodemcuv2 --port /dev/ttyUSB0 .
   ```

4. **Verify Operation**
   - Open Serial Monitor at 115200 baud
   - Look for "Delivery success" messages
   - Verify distance measurements are reasonable

## 📊 Operation Details

### Distance Measurement
```
Trigger Pulse (10μs) → Ultrasonic Burst → Echo Duration → Distance Calculation
```

Distance calculation formula:
```cpp
int distance = duration * 0.034 / 2;  // cm
```

### ESP-NOW Transmission
- **Protocol**: Proprietary WiFi-based (no router required)
- **Range**: Up to 50m line-of-sight
- **Latency**: <5ms transmission time
- **Power**: ~80mA during transmission

### Data Packet Structure
```cpp
typedef struct {
    int distance;        // Distance in centimeters
    uint32_t timestamp;  // Millisecond timestamp
    uint8_t node_id;     // Node identifier
    uint8_t battery;     // Battery level (%)
} SensorPacket;
```

## 🔍 Troubleshooting

### Common Issues

**No distance readings:**
- Check HC-SR04 connections
- Verify 3.3V power supply
- Ensure trigger/echo pins are correct

**ESP-NOW transmission failures:**
- Verify receiver MAC address
- Check distance between nodes (<50m)
- Ensure both nodes are on same WiFi channel

**Inconsistent readings:**
- Adjust sensor angle/position
- Enable median filter in config
- Check for interference sources

### Debug Mode
Enable debug output in `config.h`:
```cpp
#define DEBUG_ENABLED true
```

Monitor serial output for diagnostic information.

## ⚡ Performance Optimization

### Power Saving
- Enable deep sleep mode in config
- Reduce sampling rate if battery life is critical
- Use low-power sleep between readings

### Accuracy Improvements
- Calibrate SPEED_OF_SOUND factor for temperature
- Enable median filtering for noisy environments
- Adjust sensor mounting for optimal coverage

### Range Extension
- Use directional ultrasonic sensors
- Implement multi-sensor array
- Add signal processing for weak echoes

## 🧪 Testing

### Unit Tests
```bash
# Test distance calculation accuracy
python -m pytest test_distance_calculation.py

# Test ESP-NOW communication
python -m pytest test_espnow_transmission.py
```

### Integration Tests
1. **Sensor Test**: Verify distance measurements with known objects
2. **Communication Test**: Confirm data reception at receiver
3. **Range Test**: Test maximum effective range
4. **Battery Test**: Measure power consumption

## 📈 Performance Metrics

| Metric | Value | Conditions |
|--------|-------|------------|
| Detection Range | 2cm - 400cm | Standard HC-SR04 |
| Accuracy | ±3mm | Optimal conditions |
| Update Rate | 5 Hz | Default settings |
| Power Consumption | 80mA | Active mode |
| Battery Life | 48+ hours | 2000mAh battery |

---

For advanced configuration and troubleshooting, see the main [System Architecture](../../../docs/system_architecture.md) documentation.