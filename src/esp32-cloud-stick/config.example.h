#ifndef CLOUD_STICK_CONFIG_H
#define CLOUD_STICK_CONFIG_H

// ================= WiFi Credentials =================
// Replace with your local WiFi access point credentials
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// ================= Firebase Realtime Database =================
// Firebase RTDB URL and Web API / Legacy Database Secret
#define FIREBASE_HOST "your-project-id-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "your-firebase-database-secret-or-token"

// Firebase Database Paths
#define FB_PATH_STICK "/stick"
#define FB_PATH_LATITUDE "/stick/latitude"
#define FB_PATH_LONGITUDE "/stick/longitude"
#define FB_PATH_FALL_DETECTED "/stick/fallDetected"
#define FB_PATH_LAST_FALL_TIME "/stick/lastFallTime"

// ================= Hardware Pin Definitions =================
// Ultrasonic Sensor (HC-SR04)
#define TRIG_PIN 32
#define ECHO_PIN 33

// Indicators & Actuators
#define OBSTACLE_LED_PIN 27
#define BUZZER_PIN 4

// MPU6050 6-Axis IMU (I2C)
#define MPU_SDA_PIN 21
#define MPU_SCL_PIN 22

// GPS Module (NEO-M8N / NEO-6M on HardwareSerial 2)
#define GPS_RX_PIN 16
#define GPS_TX_PIN 17
#define GPS_BAUD_RATE 9600

// ================= Thresholds & Timing =================
// Obstacle Detection Threshold
#define OBSTACLE_DISTANCE_MIN_CM 0.0
#define OBSTACLE_DISTANCE_MAX_CM 10.0 // Distance < 10cm lights up obstacle LED

// Kinematic Fall Detection Thresholds
#define FALL_LOW_G 0.3                // Free-fall low gravity threshold
#define FALL_HIGH_G 2.8               // Impact high acceleration threshold
#define FALL_WINDOW_MS 300            // Maximum window between low-g and high-g
#define FALL_COOLDOWN_MS 1000         // Cooldown period after fall

// Inactivity & Emergency Timing
#define INACTIVITY_LIMIT_MS 10000     // Inactivity alert if stationary for > 10s after fall
#define INACTIVITY_BEEP_INTERVAL_MS 10000
#define FIREBASE_SYNC_INTERVAL_MS 20000 // Periodic background sync (20 seconds)

// Default Coordinates (Fallbacks before first satellite lock)
#define DEFAULT_LATITUDE 12.8406
#define DEFAULT_LONGITUDE 80.1534

#endif // CLOUD_STICK_CONFIG_H

