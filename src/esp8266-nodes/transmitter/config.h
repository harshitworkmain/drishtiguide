#ifndef CONFIG_H
#define CONFIG_H

// ================= Hardware Pin Configuration =================
#define TRIG_PIN D6
#define ECHO_PIN D5

// ================= ESP-NOW Settings =================
// MAC address of Receiver ESP (update with your receiver's MAC)
uint8_t receiverAddress[] = {0x24, 0x6F, 0x28, 0x12, 0x34, 0x56};

// ================= Sensor Settings =================
#define MAX_DISTANCE 400        // Maximum detection distance (cm)
#define MIN_DISTANCE 2          // Minimum detection distance (cm)
#define SAMPLE_RATE 200         // Sampling interval (ms)
#define TIMEOUT_DURATION 30000  // Ultrasonic sensor timeout (μs)

// ================= Communication Settings =================
#define RETRY_COUNT 3           // Number of transmission retries
#define PACKET_SIZE 32          // ESP-NOW packet size
#define CHANNEL 1               // WiFi channel for ESP-NOW

// ================= Power Management =================
#define DEEP_SLEEP_ENABLED false     // Enable deep sleep mode
#define SLEEP_DURATION 60            // Sleep duration (seconds) if enabled
#define BATTERY_MONITOR_PIN A0        // Analog pin for battery monitoring
#define LOW_BATTERY_THRESHOLD 3.0    // Low battery voltage threshold

// ================= Debug Settings =================
#define DEBUG_ENABLED true       // Enable serial debug output
#define BAUD_RATE 115200        // Serial communication speed

// ================= Calibration Settings =================
#define SPEED_OF_SOUND 0.034    // Speed of sound correction factor
#define DISTANCE_OFFSET 0       // Distance measurement offset (cm)
#define FILTER_ENABLED true     // Enable median filter for readings
#define FILTER_WINDOW 5         // Median filter window size

// ================= Performance Optimization =================
#define TRANSMISSION_POWER 82   // RF transmit power (0-82 dBm)
#define DATA_RATE 1             // ESP-NOW data rate (0-3)

#endif // CONFIG_H