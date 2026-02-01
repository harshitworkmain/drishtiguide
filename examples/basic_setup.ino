#include <ESP8266WiFi.h>
#include <espnow.h>
#include "config.h"

// Structure for sensor data
struct SensorData {
    int distance;
    uint32_t timestamp;
    uint8_t batteryLevel;
    float temperature;
};

// Global variables
SensorData sensorData;
bool sendSuccess = false;
uint8_t retryCount = 0;

// ESP-NOW receiver MAC address
uint8_t receiverAddress[] = RECEIVER_MAC_ADDRESS;

// Function prototypes
void setupPins();
void setupESPNow();
void readSensors();
void transmitData();
void onDataSent(uint8_t *mac_addr, uint8_t sendStatus);

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    Serial.println("=== DrishtiGuide Transmitter ===");
    
    setupPins();
    setupESPNow();
    
    // Initialize sensor data
    sensorData = {0, 0, 100, 20.0};
    
    // Startup indicator
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    
    Serial.println("Transmitter initialized successfully");
}

void loop() {
    readSensors();
    transmitData();
    
    // Status LED blink
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    
    delay(SAMPLE_INTERVAL_MS);
}

void setupPins() {
    // Ultrasonic sensor pins
    pinMode(TRIGGER_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    
    // Status LED
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    
    // Battery monitoring
    pinMode(BATTERY_MONITOR_PIN, INPUT);
    
    // Initialize trigger pin
    digitalWrite(TRIGGER_PIN, LOW);
}

void setupESPNow() {
    // Set device as Wi-Fi station
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    
    // Initialize ESP-NOW
    if (esp_now_init() != 0) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    
    // Set ESP-NOW role
    esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
    
    // Register send callback
    esp_now_register_send_cb(onDataSent);
    
    // Add receiver peer
    esp_now_add_peer(receiverAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
    
    Serial.println("ESP-NOW initialized");
    Serial.printf("Receiver MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
                  receiverAddress[0], receiverAddress[1], receiverAddress[2],
                  receiverAddress[3], receiverAddress[4], receiverAddress[5]);
}

void readSensors() {
    // Read ultrasonic distance
    long duration, distance;
    
    // Send 10μs trigger pulse
    digitalWrite(TRIGGER_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGGER_PIN, LOW);
    
    // Read echo pulse
    duration = pulseIn(ECHO_PIN, HIGH, TIMEOUT_MS);
    
    // Calculate distance (cm)
    distance = duration * 0.034 / 2;
    
    // Validate distance reading
    if (distance >= MIN_DISTANCE_CM && distance <= MAX_DISTANCE_CM) {
        sensorData.distance = (int)distance;
    } else {
        // Use last valid reading or default
        if (sensorData.distance == 0) sensorData.distance = 100;
    }
    
    // Read battery level
    int batteryRaw = analogRead(BATTERY_MONITOR_PIN);
    float batteryVoltage = batteryRaw * (3.3 / 1024.0) * VOLTAGE_DIVIDER_RATIO;
    sensorData.batteryLevel = (uint8_t)(map(batteryVoltage * 100, 300, 420, 0, 100));
    sensorData.batteryLevel = constrain(sensorData.batteryLevel, 0, 100);
    
    // Simulate temperature reading (replace with actual sensor)
    sensorData.temperature = 20.0 + (rand() % 100) / 10.0;
    
    // Update timestamp
    sensorData.timestamp = millis();
    
    // Debug output
    if (DEBUG_ENABLED) {
        Serial.printf("Distance: %dcm, Battery: %d%%, Temp: %.1f°C\n",
                      sensorData.distance, sensorData.batteryLevel, sensorData.temperature);
    }
}

void transmitData() {
    sendSuccess = false;
    retryCount = 0;
    
    while (!sendSuccess && retryCount < MAX_RETRIES) {
        esp_err_t result = esp_now_send(receiverAddress, (uint8_t *)&sensorData, sizeof(sensorData));
        
        if (result == ESP_OK) {
            if (DEBUG_ENABLED) {
                Serial.printf("Data sent (attempt %d)\n", retryCount + 1);
            }
            break;
        } else {
            Serial.printf("Send error: %d (attempt %d)\n", result, retryCount + 1);
            delay(RETRY_DELAY_MS);
            retryCount++;
        }
    }
    
    if (!sendSuccess && retryCount >= MAX_RETRIES) {
        Serial.println("Failed to send data after maximum retries");
        
        // Blink LED rapidly to indicate error
        for (int i = 0; i < 5; i++) {
            digitalWrite(LED_BUILTIN, HIGH);
            delay(100);
            digitalWrite(LED_BUILTIN, LOW);
            delay(100);
        }
    }
}

void onDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
    sendSuccess = (sendStatus == 0);
    
    if (DEBUG_ENABLED) {
        char macStr[18];
        snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
                 mac_addr[0], mac_addr[1], mac_addr[2],
                 mac_addr[3], mac_addr[4], mac_addr[5]);
        
        Serial.printf("Last packet sent to: %s - Status: %s\n",
                      macStr, sendStatus == 0 ? "Success" : "Fail");
    }
}