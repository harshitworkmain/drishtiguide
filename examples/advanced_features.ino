#include <ESP8266WiFi.h>
#include <espnow.h>
#include "config.h"

// Advanced sensor data structure with additional fields
struct AdvancedSensorData {
    int distance;
    uint32_t timestamp;
    uint8_t batteryLevel;
    float temperature;
    float humidity;
    uint16_t lightLevel;
    uint8_t signalStrength;
    uint8_t errorFlags;
};

// System state structure
struct SystemState {
    bool operational;
    uint32_t uptime;
    uint32_t lastTransmission;
    uint16_t transmissionCount;
    uint8_t errorCount;
    uint8_t currentMode;
};

// Global variables
AdvancedSensorData sensorData;
SystemState systemState;
bool sendSuccess = false;
uint32_t lastHeartbeat = 0;

// Function prototypes
void initializeAdvanced();
void performSelfTest();
void enterLowPowerMode();
void exitLowPowerMode();
void readAdvancedSensors();
void processSensorData();
void handleSystemErrors();
void sendHeartbeat();

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    delay(1000);
    
    Serial.println("=== DrishtiGuide Advanced Transmitter ===");
    Serial.println("Version: 2.0");
    Serial.println("Features: Multi-sensor, Advanced Detection");
    
    initializeAdvanced();
    performSelfTest();
    
    // System ready notification
    Serial.println("System ready - Starting normal operation");
    
    // Startup sequence
    for (int i = 0; i < 3; i++) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(200);
        digitalWrite(LED_BUILTIN, LOW);
        delay(200);
    }
}

void loop() {
    // Update system state
    systemState.uptime = millis();
    
    // Read all sensors
    readAdvancedSensors();
    
    // Process and filter data
    processSensorData();
    
    // Transmit data
    transmitAdvancedData();
    
    // Send periodic heartbeat
    if (millis() - lastHeartbeat > HEARTBEAT_INTERVAL_MS) {
        sendHeartbeat();
        lastHeartbeat = millis();
    }
    
    // Handle system errors
    handleSystemErrors();
    
    // Power management
    managePower();
    
    delay(SAMPLE_INTERVAL_MS);
}

void initializeAdvanced() {
    // Initialize basic pins
    pinMode(TRIGGER_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(BATTERY_MONITOR_PIN, INPUT);
    pinMode(LIGHT_SENSOR_PIN, INPUT);
    
    // Initialize additional sensors (placeholder for real sensors)
    // initializeTemperatureSensor();
    // initializeHumiditySensor();
    
    // Setup WiFi and ESP-NOW
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    
    if (esp_now_init() != 0) {
        Serial.println("ESP-NOW initialization failed");
        systemState.errorCount++;
        return;
    }
    
    esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
    esp_now_register_send_cb(onAdvancedDataSent);
    
    // Add receiver with encryption (optional)
    uint8_t key[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                     0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10};
    esp_now_add_peer(RECEIVER_MAC_ADDRESS, ESP_NOW_ROLE_SLAVE, 1, key, 16);
    
    // Initialize system state
    systemState = {
        .operational = true,
        .uptime = 0,
        .lastTransmission = 0,
        .transmissionCount = 0,
        .errorCount = 0,
        .currentMode = MODE_NORMAL
    };
    
    // Initialize sensor data
    memset(&sensorData, 0, sizeof(sensorData));
    
    digitalWrite(TRIGGER_PIN, LOW);
    
    Serial.println("Advanced transmitter initialized");
}

void performSelfTest() {
    Serial.println("Performing system self-test...");
    
    bool testPassed = true;
    
    // Test ultrasonic sensor
    if (testUltrasonicSensor()) {
        Serial.println("✓ Ultrasonic sensor: OK");
    } else {
        Serial.println("✗ Ultrasonic sensor: FAILED");
        testPassed = false;
    }
    
    // Test battery monitoring
    if (testBatteryMonitor()) {
        Serial.println("✓ Battery monitor: OK");
    } else {
        Serial.println("✗ Battery monitor: FAILED");
        testPassed = false;
    }
    
    // Test ESP-NOW
    if (testESPNow()) {
        Serial.println("✓ ESP-NOW: OK");
    } else {
        Serial.println("✗ ESP-NOW: FAILED");
        testPassed = false;
    }
    
    // Test memory
    if (testMemory()) {
        Serial.println("✓ Memory: OK");
    } else {
        Serial.println("✗ Memory: FAILED");
        testPassed = false;
    }
    
    if (testPassed) {
        Serial.println("Self-test PASSED");
        systemState.operational = true;
    } else {
        Serial.println("Self-test FAILED - Entering safe mode");
        systemState.operational = false;
        systemState.currentMode = MODE_SAFE;
    }
}

bool testUltrasonicSensor() {
    // Test if ultrasonic sensor responds
    digitalWrite(TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGGER_PIN, LOW);
    
    long duration = pulseIn(ECHO_PIN, HIGH, 30000);
    return (duration > 0 && duration < 30000);
}

bool testBatteryMonitor() {
    int reading = analogRead(BATTERY_MONITOR_PIN);
    return (reading > 0 && reading < 1024);
}

bool testESPNow() {
    return (esp_now_init() == 0);
}

bool testMemory() {
    uint32_t freeHeap = ESP.getFreeHeap();
    return (freeHeap > 1000); // At least 1KB free
}

void readAdvancedSensors() {
    // Read distance
    readDistance();
    
    // Read battery
    readBattery();
    
    // Read temperature (simulate for now)
    sensorData.temperature = readTemperature();
    
    // Read humidity (simulate for now)
    sensorData.humidity = readHumidity();
    
    // Read light level
    sensorData.lightLevel = analogRead(LIGHT_SENSOR_PIN);
    
    // Read signal strength
    sensorData.signalStrength = WiFi.RSSI();
    
    // Update timestamp
    sensorData.timestamp = millis();
    
    // Update error flags
    updateErrorFlags();
}

void readDistance() {
    long duration, distance;
    
    digitalWrite(TRIGGER_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGGER_PIN, LOW);
    
    duration = pulseIn(ECHO_PIN, HIGH, TIMEOUT_MS);
    distance = duration * 0.034 / 2;
    
    if (distance >= MIN_DISTANCE_CM && distance <= MAX_DISTANCE_CM) {
        sensorData.distance = (int)distance;
    } else {
        sensorData.distance = INVALID_READING;
    }
}

void readBattery() {
    int batteryRaw = analogRead(BATTERY_MONITOR_PIN);
    float voltage = batteryRaw * (3.3 / 1024.0) * VOLTAGE_DIVIDER_RATIO;
    sensorData.batteryLevel = (uint8_t)(map(voltage * 100, 300, 420, 0, 100));
    sensorData.batteryLevel = constrain(sensorData.batteryLevel, 0, 100);
}

float readTemperature() {
    // Placeholder for real temperature sensor
    // Replace with actual sensor reading
    return 20.0 + (rand() % 100) / 10.0;
}

float readHumidity() {
    // Placeholder for real humidity sensor
    // Replace with actual sensor reading
    return 40.0 + (rand() % 400) / 10.0;
}

void processSensorData() {
    // Apply filtering and validation
    static int distanceReadings[FILTER_SIZE];
    static uint8_t readingIndex = 0;
    
    // Add current reading to buffer
    distanceReadings[readingIndex] = sensorData.distance;
    readingIndex = (readingIndex + 1) % FILTER_SIZE;
    
    // Calculate median distance
    int sortedReadings[FILTER_SIZE];
    memcpy(sortedReadings, distanceReadings, sizeof(distanceReadings));
    sortArray(sortedReadings, FILTER_SIZE);
    
    sensorData.distance = sortedReadings[FILTER_SIZE / 2];
    
    // Validate other sensors
    if (sensorData.temperature < -40 || sensorData.temperature > 85) {
        sensorData.errorFlags |= ERROR_TEMPERATURE;
    }
    
    if (sensorData.humidity < 0 || sensorData.humidity > 100) {
        sensorData.errorFlags |= ERROR_HUMIDITY;
    }
}

void transmitAdvancedData() {
    if (!systemState.operational) {
        return;
    }
    
    esp_err_t result = esp_now_send(RECEIVER_MAC_ADDRESS, 
                                 (uint8_t *)&sensorData, 
                                 sizeof(sensorData));
    
    if (result == ESP_OK) {
        systemState.lastTransmission = millis();
        systemState.transmissionCount++;
        
        if (DEBUG_ENABLED) {
            Serial.printf("Transmitted: Dist=%dcm, Batt=%d%%, Temp=%.1f°C\n",
                          sensorData.distance, sensorData.batteryLevel, sensorData.temperature);
        }
    } else {
        systemState.errorCount++;
        Serial.printf("Transmission failed: Error %d\n", result);
    }
}

void onAdvancedDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
    sendSuccess = (sendStatus == 0);
    
    if (!sendSuccess) {
        systemState.errorCount++;
    }
}

void managePower() {
    // Check battery level
    if (sensorData.batteryLevel < LOW_BATTERY_THRESHOLD) {
        if (systemState.currentMode != MODE_POWER_SAVE) {
            Serial.println("Low battery - Entering power save mode");
            systemState.currentMode = MODE_POWER_SAVE;
            enterLowPowerMode();
        }
    } else if (sensorData.batteryLevel > RECOVERY_BATTERY_THRESHOLD) {
        if (systemState.currentMode == MODE_POWER_SAVE) {
            Serial.println("Battery recovered - Entering normal mode");
            systemState.currentMode = MODE_NORMAL;
            exitLowPowerMode();
        }
    }
}

void enterLowPowerMode() {
    // Reduce sampling rate
    WiFi.setSleepMode(WIFI_MODEM_SLEEP);
    
    // Lower CPU frequency
    system_update_cpu_freq(80);
    
    // Turn off unnecessary LEDs
    digitalWrite(LED_BUILTIN, LOW);
}

void exitLowPowerMode() {
    // Restore normal operation
    WiFi.setSleepMode(WIFI_NONE_SLEEP);
    system_update_cpu_freq(160);
}

void handleSystemErrors() {
    if (systemState.errorCount > MAX_ERROR_COUNT) {
        Serial.println("Too many errors - System restart");
        ESP.restart();
    }
    
    // Handle specific error flags
    if (sensorData.errorFlags & ERROR_TEMPERATURE) {
        Serial.println("Temperature sensor error");
    }
    
    if (sensorData.errorFlags & ERROR_HUMIDITY) {
        Serial.println("Humidity sensor error");
    }
}

void sendHeartbeat() {
    Serial.printf("Heartbeat - Uptime: %lums, TX: %d, Errors: %d\n",
                  systemState.uptime, systemState.transmissionCount, systemState.errorCount);
}

void updateErrorFlags() {
    sensorData.errorFlags = 0;
    
    if (sensorData.distance == INVALID_READING) {
        sensorData.errorFlags |= ERROR_DISTANCE;
    }
    
    if (sensorData.batteryLevel < 5) {
        sensorData.errorFlags |= ERROR_BATTERY;
    }
}

void sortArray(int* arr, int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}