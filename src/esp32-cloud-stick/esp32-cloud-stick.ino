#include <Wire.h>
#include <MPU6050.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <math.h>

// Required for Firebase ESP Client
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// Configuration parameters
#include "config.h"

// === Subsystem Objects ===
MPU6050 mpu;
TinyGPSPlus gps;
HardwareSerial gpsSerial(2);
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig fbConfig;

// === Sensor & State Variables ===
long duration = 0;
float distance = 0.0;

unsigned long lastLowTime = 0;
unsigned long lastFallTime = 0;
bool inLowWindow = false;
bool fallDetected = false;

unsigned long lastMovementTime = 0;
bool inactivityTriggered = false;

unsigned long lastBuzzToggle = 0;
double currentLat = DEFAULT_LATITUDE;
double currentLon = DEFAULT_LONGITUDE;
String lastFallTimeStr = "N/A";
bool fallAlertActive = false; // Status for Firebase

unsigned long lastFirebaseSync = 0;
unsigned long lastCloudPoll = 0;

// === Audible Alert Routine ===
void beep(int times, int onTime, int offTime) {
  for (int i = 0; i < times; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(onTime);
    digitalWrite(BUZZER_PIN, LOW);
    delay(offTime);
    yield();
  }
}

// === Upload Telemetry to Firebase RTDB ===
void uploadToFirebase() {
  if (Firebase.ready()) {
    if (gps.location.isValid()) {
      currentLat = gps.location.lat();
      currentLon = gps.location.lng();
    }
    
    Firebase.RTDB.setFloat(&fbdo, FB_PATH_LATITUDE, currentLat);
    Firebase.RTDB.setFloat(&fbdo, FB_PATH_LONGITUDE, currentLon);
    Firebase.RTDB.setBool(&fbdo, FB_PATH_FALL_DETECTED, fallAlertActive);
    Firebase.RTDB.setString(&fbdo, FB_PATH_LAST_FALL_TIME, lastFallTimeStr);
    Serial.println("[Firebase] Telemetry Sync Successful");
  }
}

// === Check Cloud for Emergency Resolution (Bidirectional Sync) ===
// When a caregiver clicks "RESOLVE EMERGENCY" on the Vercel dashboard,
// Firebase updates /stick/fallDetected = false. This function detects
// the resolution and stops the local alarm state.
void checkCloudEmergencyStatus() {
  if (Firebase.ready() && fallAlertActive) {
    if (Firebase.RTDB.getBool(&fbdo, FB_PATH_FALL_DETECTED)) {
      bool cloudFallStatus = fbdo.boolData();
      if (!cloudFallStatus) {
        Serial.println("[Cloud] Emergency resolved remotely by caregiver via web dashboard.");
        fallAlertActive = false;
        inactivityTriggered = false;
        digitalWrite(BUZZER_PIN, LOW);
        beep(1, 100, 100); // Confirmation chirp
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== DrishtiGuide Cloud Stick Initializing ===");

  // I2C for MPU6050
  Wire.begin(MPU_SDA_PIN, MPU_SCL_PIN);
  mpu.initialize();
  if (mpu.testConnection()) {
    Serial.println("[Hardware] MPU6050 connected successfully");
  } else {
    Serial.println("[Hardware] Warning: MPU6050 connection failed");
  }

  // GPS Serial
  gpsSerial.begin(GPS_BAUD_RATE, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);

  // Pin modes
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(OBSTACLE_LED_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(OBSTACLE_LED_PIN, LOW);

  // WiFi Station Mode Connection
  Serial.printf("[WiFi] Connecting to %s", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  int wifiAttempts = 0;
  while (WiFi.status() != WL_CONNECTED && wifiAttempts < 30) {
    delay(500);
    Serial.print(".");
    wifiAttempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n[WiFi] Connected successfully. IP: " + WiFi.localIP().toString());
  } else {
    Serial.println("\n[WiFi] Warning: Connection timeout. Continuing in offline mode...");
  }

  // Firebase Realtime Database Initialization
  fbConfig.host = FIREBASE_HOST;
  fbConfig.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&fbConfig, &auth);
  Firebase.reconnectWiFi(true);

  // Startup chirp
  beep(1, 200, 100);
  Serial.println("=== DrishtiGuide Cloud Stick Ready ===");
}

void loop() {
  // 1. GPS NMEA Stream Processing
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }

  // 2. Ultrasonic Obstacle Detection
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30ms timeout
  distance = (duration > 0) ? (duration * 0.034 / 2.0) : 400.0;

  // Obstacle proximity warning (Distance < 10cm)
  if (distance > OBSTACLE_DISTANCE_MIN_CM && distance < OBSTACLE_DISTANCE_MAX_CM) {
    digitalWrite(OBSTACLE_LED_PIN, HIGH);
  } else {
    digitalWrite(OBSTACLE_LED_PIN, LOW);
  }

  // 3. MPU6050 Motion Kinematics & Fall Detection
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  float ax_g = ax / 16384.0;
  float ay_g = ay / 16384.0;
  float az_g = az / 16384.0;
  float totalAcc = sqrt(ax_g * ax_g + ay_g * ay_g + az_g * az_g);

  unsigned long now = millis();

  // Dynamic Movement Detection
  static float lastAcc = 1.0;
  if (fabs(totalAcc - lastAcc) > 0.05) {
    lastMovementTime = now;
    if (!fallDetected) {
      inactivityTriggered = false;
    }
  }
  lastAcc = totalAcc;

  // Dual-Threshold Kinematic Fall Detection
  if (now - lastFallTime >= FALL_COOLDOWN_MS) {
    if (!inLowWindow) {
      if (totalAcc < FALL_LOW_G) {
        inLowWindow = true;
        lastLowTime = now;
      }
    } else {
      if (totalAcc > FALL_HIGH_G) {
        fallDetected = true;
        inLowWindow = false;
      } else if (now - lastLowTime > FALL_WINDOW_MS) {
        inLowWindow = false;
      }
    }
  }

  // 4. Action Triggered on Fall Event
  if (fallDetected) {
    fallAlertActive = true;
    lastFallTime = now;
    lastMovementTime = now;

    Serial.println("[ALERT] Fall detected! Triggering local alarm & cloud push...");
    beep(2, 300, 300);

    // Format Timestamp
    unsigned long secondsSinceStart = millis() / 1000;
    char buf[16];
    sprintf(buf, "%02lu:%02lu:%02lu", (secondsSinceStart / 3600) % 24, (secondsSinceStart / 60) % 60, secondsSinceStart % 60);
    lastFallTimeStr = String(buf);

    uploadToFirebase(); // Instant cloud upload
    fallDetected = false;
    inactivityTriggered = true;
    lastBuzzToggle = now;
  }

  // 5. Post-Fall Inactivity Monitoring Alarm
  if (inactivityTriggered && (now - lastMovementTime > INACTIVITY_LIMIT_MS)) {
    if (now - lastBuzzToggle >= INACTIVITY_BEEP_INTERVAL_MS) {
      lastBuzzToggle = now;
      Serial.println("[ALERT] Inactivity detected following fall event!");
      beep(2, 300, 300);
      uploadToFirebase();
    }
  }

  // 6. Check for Remote Emergency Resolution (Every 3 seconds if alert active)
  if (fallAlertActive && (now - lastCloudPoll > 3000)) {
    lastCloudPoll = now;
    checkCloudEmergencyStatus();
  }

  // 7. Periodic Background Firebase Sync (Every 20 seconds)
  if (now - lastFirebaseSync > FIREBASE_SYNC_INTERVAL_MS) {
    lastFirebaseSync = now;
    uploadToFirebase();
  }

  delay(50);
  yield();
}

