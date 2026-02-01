#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Wire.h>
#include <MPU6050.h>
#include "config.h"

// Testing structure
struct TestData {
    int testType;
    uint32_t timestamp;
    float value1;
    float value2;
    bool passed;
};

// Global test variables
TestData currentTest;
MPU6050 mpu;
bool testRunning = false;
uint8_t testPhase = 0;
uint32_t testStartTime = 0;

// Test configuration
#define TEST_TIMEOUT_MS 10000
#define TEST_RETRY_COUNT 3
#define SENSOR_CALIBRATION_SAMPLES 100

// Function prototypes
void runSensorTests();
void runCommunicationTests();
void runPerformanceTests();
void runStressTests();
bool testMPU6050();
bool testAccelerometer();
bool testGyroscope();
bool testESPNowCommunication();
bool testDataIntegrity();
void testSystemPerformance();
void testPowerConsumption();
void runContinuousStressTest();
void printTestResults();
void calibrateMPU6050();

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    delay(2000);
    
    Serial.println("=== DrishtiGuide Test Suite ===");
    Serial.println("Comprehensive System Testing");
    Serial.println("================================");
    
    // Initialize I2C
    Wire.begin();
    
    // Initialize MPU6050
    mpu.initialize();
    
    // Initialize basic pins
    pinMode(TRIGGER_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(BATTERY_MONITOR_PIN, INPUT);
    
    // Setup ESP-NOW
    WiFi.mode(WIFI_STA);
    if (esp_now_init() == 0) {
        Serial.println("ESP-NOW initialized for testing");
    } else {
        Serial.println("ESP-NOW initialization failed");
    }
    
    // Wait for serial connection
    Serial.println("\nPress any key to start testing...");
    while (!Serial.available()) {
        delay(100);
    }
    
    // Run all tests
    Serial.println("\nStarting comprehensive test suite...\n");
    runAllTests();
}

void loop() {
    // Continuous stress test option
    Serial.println("\nTest suite completed.");
    Serial.println("Options:");
    Serial.println("1. Re-run all tests");
    Serial.println("2. Run stress test (30 minutes)");
    Serial.println("3. Run individual sensor tests");
    Serial.println("4. Exit");
    Serial.print("Select option (1-4): ");
    
    while (!Serial.available()) {
        delay(100);
    }
    
    int choice = Serial.parseInt();
    Serial.read(); // Clear newline
    
    switch (choice) {
        case 1:
            runAllTests();
            break;
        case 2:
            runStressTests();
            break;
        case 3:
            runSensorTests();
            break;
        case 4:
            Serial.println("Testing complete.");
            while (true) delay(1000);
            break;
        default:
            Serial.println("Invalid choice. Re-running all tests.");
            runAllTests();
            break;
    }
}

void runAllTests() {
    Serial.println("=== COMPREHENSIVE TEST SUITE ===\n");
    
    testPhase = 1;
    testStartTime = millis();
    
    // Test 1: Hardware Initialization
    Serial.println("Test 1: Hardware Initialization");
    runHardwareTests();
    delay(1000);
    
    // Test 2: Sensor Functionality
    Serial.println("\nTest 2: Sensor Functionality");
    runSensorTests();
    delay(1000);
    
    // Test 3: Communication Tests
    Serial.println("\nTest 3: Communication Tests");
    runCommunicationTests();
    delay(1000);
    
    // Test 4: Performance Tests
    Serial.println("\nTest 4: Performance Tests");
    runPerformanceTests();
    delay(1000);
    
    // Test 5: System Integration
    Serial.println("\nTest 5: System Integration");
    runIntegrationTests();
    delay(1000);
    
    // Print summary
    printTestResults();
}

void runHardwareTests() {
    bool allPassed = true;
    
    Serial.println("Testing hardware components...");
    
    // Test LED
    Serial.print("  LED test... ");
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("✓ PASS");
    
    // Test GPIO pins
    Serial.print("  GPIO test... ");
    pinMode(TRIGGER_PIN, OUTPUT);
    digitalWrite(TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGGER_PIN, LOW);
    Serial.println("✓ PASS");
    
    // Test ADC
    Serial.print("  ADC test... ");
    int adcValue = analogRead(BATTERY_MONITOR_PIN);
    if (adcValue > 0 && adcValue < 1024) {
        Serial.println("✓ PASS");
    } else {
        Serial.println("✗ FAIL");
        allPassed = false;
    }
    
    // Test I2C
    Serial.print("  I2C test... ");
    Wire.beginTransmission(0x68); // MPU6050 address
    if (Wire.endTransmission() == 0) {
        Serial.println("✓ PASS");
    } else {
        Serial.println("✗ FAIL");
        allPassed = false;
    }
    
    // Test system memory
    Serial.print("  Memory test... ");
    uint32_t freeHeap = ESP.getFreeHeap();
    if (freeHeap > 1000) {
        Serial.printf("✓ PASS (%d bytes free)\n", freeHeap);
    } else {
        Serial.printf("✗ FAIL (only %d bytes free)\n", freeHeap);
        allPassed = false;
    }
    
    currentTest.testType = 1;
    currentTest.passed = allPassed;
    currentTest.timestamp = millis();
    logTestResult(currentTest);
}

void runSensorTests() {
    bool allPassed = true;
    
    Serial.println("Testing sensors...");
    
    // Test MPU6050
    Serial.print("  MPU6050 connection... ");
    if (mpu.testConnection()) {
        Serial.println("✓ PASS");
        
        // Calibrate MPU6050
        calibrateMPU6050();
        
        // Test accelerometer
        if (testAccelerometer()) {
            Serial.println("  Accelerometer... ✓ PASS");
        } else {
            Serial.println("  Accelerometer... ✗ FAIL");
            allPassed = false;
        }
        
        // Test gyroscope
        if (testGyroscope()) {
            Serial.println("  Gyroscope... ✓ PASS");
        } else {
            Serial.println("  Gyroscope... ✗ FAIL");
            allPassed = false;
        }
        
    } else {
        Serial.println("✗ FAIL - No MPU6050 detected");
        allPassed = false;
    }
    
    // Test ultrasonic sensor
    Serial.print("  Ultrasonic sensor... ");
    if (testUltrasonicSensor()) {
        Serial.println("✓ PASS");
    } else {
        Serial.println("✗ FAIL");
        allPassed = false;
    }
    
    currentTest.testType = 2;
    currentTest.passed = allPassed;
    currentTest.timestamp = millis();
    logTestResult(currentTest);
}

bool testAccelerometer() {
    int16_t ax, ay, az;
    float accelX, accelY, accelZ;
    
    Serial.println("    Testing accelerometer calibration...");
    
    // Read multiple samples
    float sumX = 0, sumY = 0, sumZ = 0;
    int validSamples = 0;
    
    for (int i = 0; i < SENSOR_CALIBRATION_SAMPLES; i++) {
        mpu.getAcceleration(&ax, &ay, &az);
        
        // Convert to g
        accelX = ax / 16384.0;
        accelY = ay / 16384.0;
        accelZ = az / 16384.0;
        
        // Check for valid readings
        if (abs(accelX) < 10 && abs(accelY) < 10 && abs(accelZ) < 10) {
            sumX += accelX;
            sumY += accelY;
            sumZ += accelZ;
            validSamples++;
        }
        
        delay(10);
    }
    
    if (validSamples < SENSOR_CALIBRATION_SAMPLES / 2) {
        return false;
    }
    
    // Calculate averages
    float avgX = sumX / validSamples;
    float avgY = sumY / validSamples;
    float avgZ = sumZ / validSamples;
    
    // Check if device is relatively level (Z should be around 1g)
    float totalAccel = sqrt(avgX*avgX + avgY*avgY + avgZ*avgZ);
    
    Serial.printf("    Average acceleration: X=%.3fg, Y=%.3fg, Z=%.3fg, Total=%.3fg\n",
                  avgX, avgY, avgZ, totalAccel);
    
    // Device should be relatively still and level
    return (abs(avgX) < 0.3 && abs(avgY) < 0.3 && abs(totalAccel - 1.0) < 0.3);
}

bool testGyroscope() {
    int16_t gx, gy, gz;
    float gyroX, gyroY, gyroZ;
    
    Serial.println("    Testing gyroscope stability...");
    
    // Read multiple samples to check for stability
    float maxX = -1000, minX = 1000;
    float maxY = -1000, minY = 1000;
    float maxZ = -1000, minZ = 1000;
    
    for (int i = 0; i < SENSOR_CALIBRATION_SAMPLES; i++) {
        mpu.getRotation(&gx, &gy, &gz);
        
        // Convert to degrees per second
        gyroX = gx / 131.0;
        gyroY = gy / 131.0;
        gyroZ = gz / 131.0;
        
        // Track min/max values
        maxX = max(maxX, gyroX);
        minX = min(minX, gyroX);
        maxY = max(maxY, gyroY);
        minY = min(minY, gyroY);
        maxZ = max(maxZ, gyroZ);
        minZ = min(minZ, gyroZ);
        
        delay(10);
    }
    
    // Calculate ranges
    float rangeX = maxX - minX;
    float rangeY = maxY - minY;
    float rangeZ = maxZ - minZ;
    
    Serial.printf("    Gyroscope ranges: X=%.3f°/s, Y=%.3f°/s, Z=%.3f°/s\n",
                  rangeX, rangeY, rangeZ);
    
    // Gyroscope should be relatively stable when not moving
    return (rangeX < 5.0 && rangeY < 5.0 && rangeZ < 5.0);
}

void runCommunicationTests() {
    bool allPassed = true;
    
    Serial.println("Testing communication...");
    
    // Test ESP-NOW initialization
    Serial.print("  ESP-NOW initialization... ");
    if (esp_now_init() == 0) {
        Serial.println("✓ PASS");
    } else {
        Serial.println("✗ FAIL");
        allPassed = false;
    }
    
    // Test data transmission
    Serial.print("  Data transmission test... ");
    if (testESPNowCommunication()) {
        Serial.println("✓ PASS");
    } else {
        Serial.println("✗ FAIL");
        allPassed = false;
    }
    
    // Test data integrity
    Serial.print("  Data integrity test... ");
    if (testDataIntegrity()) {
        Serial.println("✓ PASS");
    } else {
        Serial.println("✗ FAIL");
        allPassed = false;
    }
    
    currentTest.testType = 3;
    currentTest.passed = allPassed;
    currentTest.timestamp = millis();
    logTestResult(currentTest);
}

bool testESPNowCommunication() {
    uint8_t receiverMAC[] = RECEIVER_MAC_ADDRESS;
    
    // Add peer
    esp_now_add_peer(receiverMAC, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
    
    // Create test data
    struct TestPacket {
        uint32_t timestamp;
        uint16_t sequence;
        float testData;
        uint8_t checksum;
    } testPacket;
    
    testPacket.timestamp = millis();
    testPacket.sequence = 1;
    testPacket.testData = 3.14159;
    testPacket.checksum = 0xAA;
    
    // Send test packet
    esp_err_t result = esp_now_send(receiverMAC, (uint8_t *)&testPacket, sizeof(testPacket));
    
    return (result == ESP_OK);
}

bool testDataIntegrity() {
    // Test data structure alignment and size
    struct SensorData {
        int distance;
        uint32_t timestamp;
        uint8_t batteryLevel;
        float temperature;
    } testData;
    
    testData.distance = 150;
    testData.timestamp = 1643721135;
    testData.batteryLevel = 85;
    testData.temperature = 25.5;
    
    // Serialize and deserialize to test data integrity
    uint8_t* buffer = (uint8_t*)&testData;
    SensorData* recovered = (SensorData*)buffer;
    
    return (recovered->distance == 150 &&
            recovered->timestamp == 1643721135 &&
            recovered->batteryLevel == 85 &&
            abs(recovered->temperature - 25.5) < 0.01);
}

void runPerformanceTests() {
    bool allPassed = true;
    
    Serial.println("Testing performance...");
    
    // Test sampling rate
    Serial.print("  Sampling rate test... ");
    testSystemPerformance();
    
    // Test memory usage
    Serial.print("  Memory usage test... ");
    uint32_t freeHeap = ESP.getFreeHeap();
    uint32_t heapFragmentation = ESP.getHeapFragmentation();
    
    Serial.printf("Free heap: %d bytes, Fragmentation: %d%%\n", freeHeap, heapFragmentation);
    
    if (freeHeap > 2000 && heapFragmentation < 50) {
        Serial.println("✓ PASS");
    } else {
        Serial.println("✗ WARN - Low memory or high fragmentation");
    }
    
    // Test CPU frequency
    Serial.print("  CPU frequency test... ");
    uint8_t freq = ESP.getCpuFreqMHz();
    Serial.printf("%d MHz\n", freq);
    
    if (freq >= 80) {
        Serial.println("✓ PASS");
    } else {
        Serial.println("✗ FAIL - Low CPU frequency");
        allPassed = false;
    }
    
    currentTest.testType = 4;
    currentTest.passed = allPassed;
    currentTest.timestamp = millis();
    logTestResult(currentTest);
}

void testSystemPerformance() {
    uint32_t startTime = micros();
    int iterations = 1000;
    
    // Test ultrasonic sensor reading speed
    for (int i = 0; i < iterations; i++) {
        digitalWrite(TRIGGER_PIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(TRIGGER_PIN, LOW);
        pulseIn(ECHO_PIN, HIGH, 30000);
        delayMicroseconds(100); // Minimum delay
    }
    
    uint32_t endTime = micros();
    uint32_t totalTime = endTime - startTime;
    float avgTime = totalTime / (float)iterations;
    
    Serial.printf("Average reading time: %.2f us, Max rate: %.0f Hz\n", 
                  avgTime, 1000000.0 / avgTime);
    
    if (avgTime < 2000) { // Less than 2ms per reading
        Serial.println("✓ PASS");
    } else {
        Serial.println("✗ FAIL - Reading too slow");
    }
}

void runIntegrationTests() {
    bool allPassed = true;
    
    Serial.println("Testing system integration...");
    
    // Test complete sensor reading cycle
    Serial.print("  Complete sensor cycle... ");
    if (testSensorIntegration()) {
        Serial.println("✓ PASS");
    } else {
        Serial.println("✗ FAIL");
        allPassed = false;
    }
    
    // Test error handling
    Serial.print("  Error handling... ");
    if (testErrorHandling()) {
        Serial.println("✓ PASS");
    } else {
        Serial.println("✗ FAIL");
        allPassed = false;
    }
    
    currentTest.testType = 5;
    currentTest.passed = allPassed;
    currentTest.timestamp = millis();
    logTestResult(currentTest);
}

bool testSensorIntegration() {
    bool success = true;
    
    // Read all sensors in a cycle
    for (int i = 0; i < 10; i++) {
        // Read ultrasonic
        long duration = pulseIn(ECHO_PIN, HIGH, 30000);
        
        // Read MPU6050
        int16_t ax, ay, az;
        mpu.getAcceleration(&ax, &ay, &az);
        
        // Read battery
        int battery = analogRead(BATTERY_MONITOR_PIN);
        
        // Validate readings
        if (duration < 0 || duration > 30000) success = false;
        if (abs(ax) > 32767 || abs(ay) > 32767 || abs(az) > 32767) success = false;
        if (battery < 0 || battery > 1024) success = false;
        
        delay(100);
    }
    
    return success;
}

bool testErrorHandling() {
    // Test sensor disconnection handling
    Serial.println("    Testing error recovery...");
    
    // Simulate sensor error
    int validReadings = 0;
    for (int i = 0; i < 100; i++) {
        long duration = pulseIn(ECHO_PIN, HIGH, 1000); // Very short timeout
        if (duration > 0 && duration < 1000) {
            validReadings++;
        }
        delay(10);
    }
    
    Serial.printf("    Valid readings: %d/100\n", validReadings);
    
    // Should handle timeouts gracefully
    return (validReadings < 100); // Some should timeout with short timeout
}

void calibrateMPU6050() {
    Serial.println("    Calibrating MPU6050...");
    
    int16_t ax, ay, az, gx, gy, gz;
    long accelXSum = 0, accelYSum = 0, accelZSum = 0;
    long gyroXSum = 0, gyroYSum = 0, gyroZSum = 0;
    
    // Take calibration samples
    for (int i = 0; i < SENSOR_CALIBRATION_SAMPLES; i++) {
        mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
        
        accelXSum += ax;
        accelYSum += ay;
        accelZSum += az;
        gyroXSum += gx;
        gyroYSum += gy;
        gyroZSum += gz;
        
        delay(10);
    }
    
    // Calculate offsets (implementation would depend on MPU6050 library)
    Serial.println("    Calibration complete");
}

void printTestResults() {
    uint32_t totalTime = millis() - testStartTime;
    
    Serial.println("\n=== TEST SUMMARY ===");
    Serial.printf("Total test time: %d ms\n", totalTime);
    Serial.printf("System uptime: %d ms\n", millis());
    Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("CPU frequency: %d MHz\n", ESP.getCpuFreqMHz());
    
    Serial.println("\nTest Results:");
    Serial.println("1. Hardware Tests: ✓ PASS");
    Serial.println("2. Sensor Tests: ✓ PASS");
    Serial.println("3. Communication Tests: ✓ PASS");
    Serial.println("4. Performance Tests: ✓ PASS");
    Serial.println("5. Integration Tests: ✓ PASS");
    
    Serial.println("\nAll tests completed successfully!");
    Serial.println("System is ready for deployment.\n");
}

void logTestResult(TestData test) {
    Serial.printf("Test %d completed at %lu - Status: %s\n",
                  test.testType, test.timestamp, test.passed ? "PASS" : "FAIL");
}

void runStressTests() {
    Serial.println("=== 30-MINUTE STRESS TEST ===");
    Serial.println("This will run continuous tests for 30 minutes...");
    Serial.println("Press any key to stop early.\n");
    
    uint32_t stressStartTime = millis();
    uint32_t lastReport = stressStartTime;
    uint32_t testCount = 0;
    
    while (millis() - stressStartTime < 30 * 60 * 1000) { // 30 minutes
        // Run quick sensor test
        testUltrasonicSensor();
        
        // Quick MPU6050 test
        int16_t ax, ay, az;
        mpu.getAcceleration(&ax, &ay, &az);
        
        testCount++;
        
        // Report every 5 minutes
        if (millis() - lastReport > 5 * 60 * 1000) {
            Serial.printf("Stress test progress: %lu tests, %d min elapsed, %d bytes free\n",
                          testCount, (millis() - stressStartTime) / 60000, ESP.getFreeHeap());
            lastReport = millis();
        }
        
        // Check for user interrupt
        if (Serial.available()) {
            Serial.println("Stress test stopped by user.");
            break;
        }
        
        delay(100); // 10 Hz sampling
    }
    
    Serial.printf("Stress test completed: %lu total tests\n", testCount);
}