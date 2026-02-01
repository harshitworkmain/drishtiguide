# Receiver Node

Haptic feedback node that receives distance data and provides intuitive vibration alerts to guide visually impaired users.

## 🎯 Overview

The receiver node processes distance measurements transmitted via ESP-NOW and activates vibration motors according to proximity levels, providing tactile feedback for obstacle avoidance.

## 🔧 Hardware Requirements

- **ESP8266 NodeMCU** or similar ESP8266-based board
- **5× Vibration Motors** (30mA - 100mA each)
- **Motor Driver Circuitry** (NPN transistors or MOSFETs)
- **Power Source**: 3.7V Li-ion battery or USB power
- **Mounting**: Wearable harness or belt attachment

## 📋 Pin Connections

| Motor | ESP8266 Pin | Description |
|-------|-------------|-------------|
| Motor 1 | D1 | Distance 80-100cm |
| Motor 2 | D2 | Distance 60-80cm |
| Motor 3 | D3 | Distance 40-60cm |
| Motor 4 | D4 | Distance 20-40cm |
| Motor 5 | D5 | Distance <20cm |

## 🔌 Motor Driver Circuit

```
ESP8266 GPIO → 1kΩ Resistor → NPN Transistor Base
              → NPN Transistor Emitter → GND
              → NPN Transistor Collector → Motor Negative
              → Motor Positive → 3.3V (or 5V for stronger vibration)
```

**Recommended Transistors:**
- 2N2222 NPN (up to 800mA)
- 2N3904 NPN (up to 200mA)
- 2N7000 MOSFET (up to 500mA)

## ⚙️ Configuration

Edit `config.h` to customize settings:

```cpp
// Distance thresholds (cm)
#define LEVEL_1_MIN 80   // Motor 1 activation
#define LEVEL_2_MIN 60   // Motor 2 activation  
#define LEVEL_3_MIN 40   // Motor 3 activation
#define LEVEL_4_MIN 20   // Motor 4 activation
#define LEVEL_5_MAX 20   // Motor 5 activation

// Motor timing
#define MOTOR_PULSE_DURATION 100   // ms
#define MOTOR_COOLDOWN 50          // ms
```

## 🚀 Installation

1. **Hardware Assembly**
   ```
   ESP8266 → Motor Driver → Vibration Motors
   D1-D5 → Transistor bases → Motor negative terminals
   3.3V → Motor positive terminals
   GND → Common ground
   ```

2. **Receiver Configuration**
   - No MAC address configuration needed (auto-discovers transmitters)
   - Ensure same WiFi channel as transmitter

3. **Flash Firmware**
   ```bash
   arduino-cli compile --fqbn esp8266:esp8266:nodemcuv2 .
   arduino-cli upload --fqbn esp8266:esp8266:nodemcuv2 --port /dev/ttyUSB1 .
   ```

4. **System Test**
   - Pair with transmitter node
   - Test with object at various distances
   - Verify progressive motor activation

## 📊 Haptic Feedback Logic

### Distance-to-Motor Mapping

| Distance Range | Active Motors | User Feedback |
|----------------|---------------|---------------|
| 80-100 cm | Motor 1 | Low intensity - distant object |
| 60-80 cm | Motors 1-2 | Low-medium intensity |
| 40-60 cm | Motors 1-3 | Medium intensity |
| 20-40 cm | Motors 1-4 | High-medium intensity |
| <20 cm | Motors 1-5 | Maximum intensity - immediate danger |

### Activation Pattern
```cpp
// Progressive activation from closest to farthest
if (distance <= LEVEL_5_MAX) {
    // Activate all motors (maximum alert)
}
else if (distance <= LEVEL_4_MAX) {
    // Activate motors 1-4 (high alert)
}
// ... continue pattern
```

## 🔍 Troubleshooting

### Common Issues

**Motors not vibrating:**
- Check power supply voltage
- Verify transistor connections
- Test motors individually with 3.3V

**Weak vibration:**
- Increase supply voltage to 5V
- Use higher current transistors
- Check motor specifications

**ESP-NOW reception failures:**
- Ensure transmitter is powered
- Verify distance between nodes (<50m)
- Check for WiFi interference

**Inconsistent activation:**
- Adjust distance thresholds in config
- Enable median filtering
- Calibrate for environment

### Safety Features

**Watchdog Timer:**
```cpp
#define WATCHDOG_TIMEOUT 5000  // Reset if no data for 5 seconds
```

**Emergency Vibration:**
```cpp
#define EMERGENCY_VIBRATION true  // Alert if data lost
```

## ⚡ Performance Optimization

### Power Management
- Enable deep sleep mode for battery operation
- Optimize motor pulse duration
- Use PWM for variable intensity control

### User Experience
- Customize distance thresholds for user preference
- Adjust vibration patterns for different environments
- Implement haptic "training mode" for user adaptation

### Motor Control
- Use PWM for variable vibration intensity
- Implement ramp-up/ramp-down for smoother feedback
- Add motor health monitoring

## 🧪 Testing

### Motor Test Routine
```cpp
// Built-in test sequence
test_motors();  // Activates each motor sequentially
```

### Integration Tests
1. **Communication Test**: Verify ESP-NOW packet reception
2. **Motor Test**: Confirm all motors activate properly
3. **Range Test**: Test activation at different distances
4. **Battery Test**: Measure power consumption under load

### User Testing
- Test with visually impaired users
- Collect feedback on intuitiveness
- Optimize for real-world scenarios

## 📈 Performance Metrics

| Metric | Value | Conditions |
|--------|-------|------------|
| Response Time | <50ms | Data to vibration |
| Motor Current | 50-100mA | Per motor |
| Power Consumption | 300-500mA | Full activation |
| Battery Life | 12-24 hours | Continuous use |
| Detection Accuracy | ±5cm | Distance mapping |

## 🎨 Customization Options

### Motor Arrangements
- **Linear Array**: Motors in straight line (belt/harness)
- **Circular Array**: Motors around wrist/ankle
- **Vest Configuration**: Distributed across torso

### Feedback Patterns
- **Proximity-Based**: Static activation by distance
- **Direction-Based**: motors indicate direction
- **Intensity-Based**: Variable vibration strength

### Advanced Features
- **Learning Mode**: User adapts to personal sensitivity
- **Environmental Adaptation**: Adjust thresholds for different environments
- **Multi-Object Detection**: Handle multiple obstacles

---

For integration with the main system and advanced configuration, see the [System Architecture](../../../docs/system_architecture.md) documentation.