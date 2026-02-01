#ifndef FALL_DETECTION_H
#define FALL_DETECTION_H

#include <Arduino.h>
#include "sensors.h"

// Fall detection states
typedef enum {
    FALL_STATE_NORMAL,
    FALL_STATE_LOW_G,
    FALL_STATE_HIGH_G,
    FALL_STATE_DETECTED,
    FALL_STATE_COOLDOWN
} FallDetectionState;

// Fall event structure
typedef struct {
    uint32_t timestamp;
    float maxAcceleration;
    float minAcceleration;
    uint16_t duration;
    bool isEmergency;
} FallEvent;

class FallDetector {
private:
    FallDetectionState currentState;
    FallEvent lastFall;
    
    // Timing variables
    uint32_t lowGStartTime;
    uint32_t lastFallTime;
    uint32_t lastMovementTime;
    
    // Configuration parameters
    float lowGThreshold;
    float highGThreshold;
    uint32_t detectionWindow;
    uint32_t cooldownPeriod;
    
    // Detection buffers
    float accelHistory[50];
    uint8_t accelHistoryIndex;
    uint8_t accelHistoryCount;
    
    // Private methods
    void updateAccelHistory(float accel);
    float getFilteredAcceleration();
    void transitionToState(FallDetectionState newState);
    void handleLowGDetected();
    void handleHighGDetected();
    void handleFallDetected();

public:
    FallDetector();
    
    // Configuration
    void setThresholds(float lowG, float highG);
    void setTiming(uint32_t windowMs, uint32_t cooldownMs);
    void reset();
    
    // Main detection logic
    bool update(SensorData sensorData);
    bool isFallDetected();
    FallEvent getLastFall();
    
    // State management
    FallDetectionState getCurrentState();
    bool isInCooldown();
    uint32_t getTimeSinceLastFall();
    
    // Advanced features
    void enableLearningMode(bool enable);
    void setCustomSensitivity(float sensitivity);
    bool isValidFallPattern();
    
    // Monitoring and diagnostics
    void printDetectionStatus();
    void logFallEvent(const FallEvent& fall);
    uint8_t getFallCount();
    void resetFallCount();
    
    // Safety features
    void setEmergencyCallback(void (*callback)(FallEvent));
    void setWarningCallback(void (*callback)(float));
    bool shouldTriggerEmergency();
    
    // Calibration and testing
    void calibrateThresholds();
    void runSelfTest();
    void simulateFall();
};

extern FallDetector fallDetector;

#endif // FALL_DETECTION_H