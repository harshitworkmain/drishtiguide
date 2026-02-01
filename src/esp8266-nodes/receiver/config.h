#ifndef CONFIG_H
#define CONFIG_H

// ================= Hardware Pin Configuration =================
#define MOTOR1 D1
#define MOTOR2 D2
#define MOTOR3 D3
#define MOTOR4 D4
#define MOTOR5 D5

// ================= Haptic Feedback Settings =================
#define NUM_MOTORS 5
#define MOTOR_PULSE_DURATION 100    // Motor activation duration (ms)
#define MOTOR_COOLDOWN 50           // Minimum time between activations (ms)

// Distance-to-Haptic Mapping (cm)
#define LEVEL_1_MIN 80
#define LEVEL_1_MAX 100
#define LEVEL_2_MIN 60
#define LEVEL_2_MAX 80
#define LEVEL_3_MIN 40
#define LEVEL_3_MAX 60
#define LEVEL_4_MIN 20
#define LEVEL_4_MAX 40
#define LEVEL_5_MAX 20

// ================= ESP-NOW Settings =================
#define CHANNEL 1               // WiFi channel for ESP-NOW
#define PACKET_SIZE 32          // ESP-NOW packet size

// ================= Power Management =================
#define DEEP_SLEEP_ENABLED false     // Enable deep sleep mode
#define SLEEP_DURATION 60            // Sleep duration (seconds) if enabled
#define BATTERY_MONITOR_PIN A0        // Analog pin for battery monitoring
#define LOW_BATTERY_THRESHOLD 3.0    // Low battery voltage threshold

// ================= Debug Settings =================
#define DEBUG_ENABLED true       // Enable serial debug output
#define BAUD_RATE 115200        // Serial communication speed

// ================= Safety Features =================
#define WATCHDOG_TIMEOUT 5000    // Watchdog timer timeout (ms)
#define MAX_INACTIVITY_TIME 10000 // Maximum time without data (ms)
#define EMERGENCY_VIBRATION true  // Emergency vibration on data loss

// ================= Performance Optimization =================
#define MOTOR_PWM_FREQ 1000      // PWM frequency for motors (Hz)
#define TRANSMISSION_POWER 82    // RF transmit power (0-82 dBm)

// ================= Calibration Settings =================
#define MOTOR_TEST_DURATION 200  // Motor test duration (ms)
#define MOTOR_TEST_INTERVAL 500  // Interval between motor tests (ms)

#endif // CONFIG_H