#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <Wire.h>
#include <MPU6050.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>

// Sensor data structure
struct SensorData {
    // MPU6050 data
    float accelX, accelY, accelZ;
    float gyroX, gyroY, gyroZ;
    float totalAccel;
    
    // GPS data
    double latitude, longitude;
    float altitude, speed, course;
    uint8_t satellites;
    bool gpsValid;
    
    // System data
    float batteryVoltage;
    uint32_t timestamp;
};

class SensorManager {
private:
    MPU6050 mpu;
    TinyGPSPlus gps;
    HardwareSerial* gpsSerial;
    SensorData currentData;
    
    // Calibration values
    float accelOffset[3] = {0, 0, 0};
    float gyroOffset[3] = {0, 0, 0};
    
    // Private methods
    void calibrateMPU6050();
    void readMPU6050();
    void readGPS();
    void readBattery();
    void filterSensorData();

public:
    SensorManager();
    
    // Initialization
    bool initialize();
    bool testMPU6050();
    bool testGPS();
    bool calibrateSensors();
    
    // Data acquisition
    void update();
    SensorData getData();
    bool isDataValid();
    
    // GPS specific
    bool hasGPSFix();
    double getLatitude();
    double getLongitude();
    float getSpeed();
    uint8_t getSatelliteCount();
    
    // MPU specific
    float getTotalAcceleration();
    bool isMoving(float threshold = 0.1);
    float getTiltAngle();
    
    // Battery monitoring
    float getBatteryVoltage();
    uint8_t getBatteryPercentage();
    bool isBatteryLow();
    
    // Diagnostics
    void printSensorStatus();
    void runDiagnostics();
};

extern SensorManager sensorManager;

#endif // SENSORS_H