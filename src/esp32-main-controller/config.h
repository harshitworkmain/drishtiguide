#ifndef CONFIG_H
#define CONFIG_H

// ================= WiFi Configuration =================
#define WIFI_AP_SSID "BlindStick_AP"
#define WIFI_AP_PASSWORD "12345678"
#define WIFI_CHANNEL 1
#define WIFI_MAX_CLIENTS 4

// ================= Hardware Pin Definitions =================
// MPU6050 IMU
#define MPU_SDA_PIN 21
#define MPU_SCL_PIN 22

// GPS Module
#define GPS_RX_PIN 16
#define GPS_TX_PIN 17

// Buzzer
#define BUZZER_PIN 27

// LEDs
#define STATUS_LED_PIN 2
#define ERROR_LED_PIN 4
#define GPS_LED_PIN 12

// ================= Fall Detection Parameters =================
#define FALL_LOW_G 0.3
#define FALL_HIGH_G 2.8
#define FALL_WINDOW_MS 300
#define FALL_COOLDOWN_MS 1000

// ================= Inactivity Monitoring =================
#define INACTIVITY_TIMEOUT_MS 10000
#define INACTIVITY_ALERT_INTERVAL_MS 10000

// ================= Buzzer Configuration =================
#define BUZZER_BEEP_DURATION_MS 300
#define EMERGENCY_BEEP_COUNT 5
#define NORMAL_BEEP_COUNT 2

// ================= GPS Configuration =================
#define GPS_BAUD_RATE 9600
#define GPS_UPDATE_INTERVAL_MS 1000
#define GPS_TIMEOUT_MS 5000

// ================= Web Server Configuration =================
#define WEB_SERVER_PORT 80
#define API_RATE_LIMIT_MS 100
#define WEB_TIMEOUT_MS 5000

// ================= System Performance =================
#define MAIN_LOOP_DELAY_MS 10
#define MPU_SAMPLE_RATE_MS 100
#define WEB_CLIENT_TIMEOUT 5000

// ================= Power Management =================
#define DEEP_SLEEP_ENABLED false
#define BATTERY_MONITOR_PIN A0
#define LOW_BATTERY_THRESHOLD 3.0
#define BATTERY_CHECK_INTERVAL_MS 30000

// ================= Debug and Logging =================
#define DEBUG_ENABLED true
#define SERIAL_BAUD_RATE 115200
#define LOG_BUFFER_SIZE 512

// ================= Safety and Reliability =================
#define WATCHDOG_TIMEOUT_MS 5000
#define MAX_RESTART_COUNT 3
#define ERROR_RECOVERY_DELAY_MS 5000

// ================= Calibration Settings =================
#define MPU_ACCEL_SCALE 16384.0
#define MPU_GYRO_SCALE 131.0
#define GRAVITY_OFFSET 1.0

#endif // CONFIG_H