# Troubleshooting Guide

This guide provides solutions to common issues encountered while setting up and using the DrishtiGuide assistive system.

## 🔧 Hardware Issues

### Ultrasonic Sensor Problems

#### Issue: No Distance Readings
**Symptoms:**
- Serial monitor shows "Distance: 0cm" or inconsistent values
- No response when placing objects in front of sensor

**Possible Causes:**
1. **Power Supply Issues**
   - Insufficient voltage to HC-SR04
   - Noisy power supply
   - Loose connections

2. **Wiring Problems**
   - Incorrect pin connections
   - Poor solder joints
   - Damaged jumper wires

3. **Sensor Malfunction**
   - Defective HC-SR04 module
   - Physical damage to sensor

**Solutions:**
```cpp
// 1. Check power supply
// Measure voltage at HC-SR04 VCC pin
// Should be stable 5V ±0.2V

// 2. Verify wiring
NodeMCU  →  HC-SR04
3.3V     →      VCC
GND       →      GND  
D6        →      TRIG
D5        →      ECHO

// 3. Test with known-good sensor
// Replace HC-SR04 to rule out sensor failure
```

#### Issue: Inconsistent Readings
**Symptoms:**
- Distance values fluctuating wildly
- Readings don't match actual distances
- Occasional zero or max values

**Solutions:**
```cpp
// 1. Add software filtering
#define FILTER_WINDOW 5
int distanceReadings[FILTER_WINDOW];
int currentReading = 0;

int getFilteredDistance() {
  // Add new reading
  distanceReadings[currentReading] = readRawDistance();
  currentReading = (currentReading + 1) % FILTER_WINDOW;
  
  // Sort and return median
  sort(distanceReadings, distanceReadings + FILTER_WINDOW);
  return distanceReadings[FILTER_WINDOW / 2];
}

// 2. Add hardware filtering
// Add 100nF capacitor between ECHO and GND
// Add 1kΩ resistor in series with ECHO line
```

### Haptic Motor Issues

#### Issue: Motors Not Vibrating
**Symptoms:**
- No vibration when expected
- Intermittent motor operation
- Weak vibration intensity

**Solutions:**
```cpp
// 1. Check transistor connections
// Verify proper NPN transistor wiring
// Emitter → GND, Base → GPIO (via 1kΩ), Collector → Motor(-)

// 2. Test motors directly
// Connect motor directly to 3.3V
// If motor works, issue is in driver circuit

// 3. Check GPIO configuration
pinMode(MOTOR1_PIN, OUTPUT);
digitalWrite(MOTOR1_PIN, HIGH);  // Should activate motor

// 4. Verify power supply
// Motors need 50-100mA each
// Ensure power supply can handle total current
```

#### Issue: Motors Too Weak
**Solutions:**
```cpp
// 1. Increase supply voltage
// Use 5V instead of 3.3V for motors
// Ensure transistors can handle higher voltage

// 2. Use more powerful motors
// Higher current motors for stronger vibration
// Check datasheet for specifications

// 3. Reduce series resistance
// Lower resistor value between GPIO and transistor base
// 470Ω or 330Ω for more base current
```

### ESP-NOW Communication Issues

#### Issue: Packets Not Reaching Receiver
**Symptoms:**
- Transmitter shows "Delivery fail"
- Receiver not receiving data
- Intermittent connection

**Solutions:**
```cpp
// 1. Check MAC addresses
uint8_t receiverAddress[] = {0x24, 0x6F, 0x28, 0x12, 0x34, 0x56};

// Print MAC addresses
Serial.printf("Transmitter MAC: %s\n", WiFi.macAddress().c_str());
Serial.printf("Receiver MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
              receiverAddress[0], receiverAddress[1], receiverAddress[2],
              receiverAddress[3], receiverAddress[4], receiverAddress[5]);
```

```cpp
// 2. Increase transmission power
// Add to setup()
WiFi.setOutputPower(20.5);  // Maximum power (20.5dBm)

// 3. Add retry logic
#define MAX_RETRIES 3
bool sendWithRetry(uint8_t* data, size_t size) {
  for(int i = 0; i < MAX_RETRIES; i++) {
    esp_err_t result = esp_now_send(receiverAddress, data, size);
    if(result == ESP_OK) return true;
    delay(100);  // Wait between retries
  }
  return false;
}
```

## 💻 Software Issues

### Compilation Errors

#### Issue: ESP8266 Compilation Failures
**Common Errors:**
```
'ESP8266WiFi.h' not found
'espnow.h' not found
Sketch too large
```

**Solutions:**
```bash
# 1. Install ESP8266 core
arduino-cli core install esp8266:esp8266

# 2. Select correct board
# Tools → Board → ESP8266 Boards → NodeMCU 1.0 (ESP-12E Module)

# 3. Reduce program size
// Remove debug output
#undef DEBUG_ENABLED

// Use smaller data types
uint8_t instead of int
uint16_t instead of long

// Store constants in PROGMEM
const char message[] PROGMEM = "Hello";
```

#### Issue: ESP32 Compilation Failures
**Solutions:**
```bash
# 1. Install ESP32 core
arduino-cli core install esp32:esp32

# 2. Select correct board
# Tools → Board → ESP32 Arduino → ESP32 Dev Module

# 3. Fix compatibility issues
// Replace ESP8266-specific code
#if defined(ESP8266)
  // ESP8266 code
#elif defined(ESP32)
  // ESP32 code
#endif
```

### Runtime Issues

#### Issue: Watchdog Timer Reset
**Symptoms:**
- Device restarts unexpectedly
- Serial shows "wdt reset" or similar

**Solutions:**
```cpp
// 1. Feed watchdog regularly
#include "esp_task_wdt.h"

void loop() {
  esp_task_wdt_reset();  // Feed watchdog
  
  // Your code here
  delay(10);
}

// 2. Disable watchdog if needed (not recommended)
esp_task_wdt_delete();
```

#### Issue: Memory Overflow
**Symptoms:**
- Device crashes or reboots
- Serial shows "heap corruption"
- Variables changing unexpectedly

**Solutions:**
```cpp
// 1. Monitor free heap
Serial.printf("Free heap: %d\n", ESP.getFreeHeap());

// 2. Use static allocation
// Instead of dynamic arrays, use fixed-size arrays
int sensorBuffer[100];  // Static allocation

// 3. Reduce string usage
// Use F() macro for string literals
Serial.println(F("This saves RAM"));

// 4. Optimize data types
// Use smallest possible data types
uint8_t smallNumber = 100;  // Instead of int
```

## 🌐 Web Interface Issues

### Connection Problems

#### Issue: WiFi Hotspot Not Visible
**Solutions:**
```cpp
// 1. Check WiFi initialization
Serial.println("Setting up AP...");
WiFi.softAP(ssid, password, channel, maxClients);

// 2. Verify AP is active
Serial.println(WiFi.softAPgetHostname());
Serial.println(WiFi.softAPIP());
```

#### Issue: Web Page Not Loading
**Solutions:**
```cpp
// 1. Check server is running
server.begin();
Serial.println("HTTP server started");

// 2. Verify client handling
void handleClient() {
  WiFiClient client = server.available();
  if(client) {
    Serial.println("Client connected");
    // Process request
  }
}
```

### API Response Issues

#### Issue: JSON Data Not Updating
**Solutions:**
```cpp
// 1. Check JSON serialization
StaticJsonDocument<256> doc;
doc["distance"] = sensorData.distance;
doc["timestamp"] = sensorData.timestamp;

// 2. Add CORS headers
server.sendHeader("Access-Control-Allow-Origin", "*");
server.send(200, "application/json", jsonBuffer);
```

## 🔋 Power Management Issues

### Battery Life Problems

#### Issue: Short Battery Life
**Symptoms:**
- Battery drains in hours instead of days
- Device gets hot during operation
- Power consumption higher than expected

**Solutions:**
```cpp
// 1. Enable power saving
WiFi.setSleepMode(WIFI_MODEM_SLEEP);
WiFi.forceSleepBegin();

// 2. Optimize sensor reading frequency
#define POWER_SAVE_INTERVAL 1000  // ms instead of 200
unsigned long lastReading = 0;

void loop() {
  if(millis() - lastReading > POWER_SAVE_INTERVAL) {
    readSensors();
    lastReading = millis();
  }
}

// 3. Use deep sleep (ESP8266)
ESP.deepSleep(0);  // Sleep indefinitely
ESP.deepSleep(microsecondsToWakeupTime(5 * 60 * 1000000));  // 5 minutes
```

## 🧪 Testing and Debugging

### Debug Mode Setup
```cpp
// Enable comprehensive debugging
#define DEBUG_ALL true

#if DEBUG_ALL
  #define DEBUG_SERIAL true
  #define DEBUG_SENSORS true
  #define DEBUG_WIFI true
  #define DEBUG_MEMORY true
#endif

void debugPrint(const char* message) {
  #if DEBUG_SERIAL
    Serial.println(message);
  #endif
}
```

### Signal Analysis
```cpp
// Use oscilloscope to check signals
// 1. Ultrasonic trigger pulse (should be clean 10μs)
// 2. Ultrasonic echo return (clean digital signal)
// 3. Motor control signals (proper PWM)
// 4. Power supply stability (low ripple)
```

## 📞 Getting Additional Help

### When All Else Fails

1. **Reset to Known Good State**
   - Flash original firmware
   - Revert configuration changes
   - Test with minimal setup

2. **Isolate Components**
   - Test each component separately
   - Use known-good components for substitution
   - Document which component fails

3. **Check Environment**
   - Try different USB ports
   - Test with different power supply
   - Eliminate electrical interference

4. **Community Support**
   - Check [GitHub Issues](https://github.com/harshitworkmain/drishtiguide/issues)
   - Search for similar problems
   - Ask questions with detailed information

### Contact Information
For additional support, reach out to:
- **Email**: harshit.workmain@gmail.com
- **GitHub**: https://github.com/harshitworkmain/drishtiguide/issues
- **Documentation**: [Project Wiki](https://github.com/harshitworkmain/drishtiguide/wiki)

### What to Include in Support Requests
1. **System Information**
   - ESP board type and version
   - Arduino/PlatformIO version
   - Operating system
   - Power supply details

2. **Problem Description**
   - What you're trying to achieve
   - What actually happens
   - Error messages or serial output

3. **Troubleshooting Steps**
   - What you've already tried
   - Results of each step
   - Any observations during testing

4. **Hardware Details**
   - Component model numbers
   - Wiring diagram/photo
   - Configuration settings

---

*Last updated: 2025-02-01*
*Author: Harshit Singh*