#include <Wire.h>
#include <MPU6050.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <WiFi.h>
#include <WebServer.h>
#include <math.h>

// ================= MPU6050 =================
MPU6050 mpu;

// ================= GPS =====================
TinyGPSPlus gps;
HardwareSerial gpsSerial(2);   // UART2

// ================= BUZZER ==================
#define BUZZER_PIN 27   // SAFE ESP32 PIN

// ================= WIFI ====================
const char* ssid = "BlindStick_AP";
const char* password = "12345678";
WebServer server(80);

// ================= FALL PARAMS =============
const float FALL_LOW_G  = 0.3;
const float FALL_HIGH_G = 2.8;
const unsigned long WINDOW_MS   = 300;
const unsigned long COOLDOWN_MS = 1000;

// ================= TIMERS ==================
unsigned long lastLowTime = 0;
unsigned long lastFallTime = 0;
unsigned long lastMovementTime = 0;
unsigned long lastBuzzToggle = 0;

// ================= STATES ==================
bool inLowWindow = false;
bool fallDetected = false;
bool inactivityTriggered = false;

// ================= BUZZER FSM ==============
bool buzzerActive = false;
int buzzerSteps = 0;
unsigned long buzzerTimer = 0;

// ================= GPS DATA ================
double currentLat = 12.8406;
double currentLon = 80.1534;
String lastFallTimeStr = "N/A";

// ================= BUZZER CONTROL ==========
void startBuzzer(int beeps) {
  buzzerActive = true;
  buzzerSteps = beeps * 2;
  buzzerTimer = millis();
  digitalWrite(BUZZER_PIN, HIGH);
}

void handleBuzzer() {
  if (!buzzerActive) return;

  if (millis() - buzzerTimer >= 300) {
    buzzerTimer = millis();
    digitalWrite(BUZZER_PIN, !digitalRead(BUZZER_PIN));
    buzzerSteps--;

    if (buzzerSteps <= 0) {
      buzzerActive = false;
      digitalWrite(BUZZER_PIN, LOW);
    }
  }
}

// ================= SETUP ===================
void setup() {
  Serial.begin(115200);

  Wire.begin(21, 22);
  mpu.initialize();

  gpsSerial.begin(9600, SERIAL_8N1, 16, 17);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  WiFi.softAP(ssid, password);

  server.on("/gps", []() {
    if (gps.location.isValid()) {
      currentLat = gps.location.lat();
      currentLon = gps.location.lng();
    }

    String json = "{";
    json += "\"latitude\":\"" + String(currentLat, 6) + "\",";
    json += "\"longitude\":\"" + String(currentLon, 6) + "\",";
    json += "\"fallTime\":\"" + lastFallTimeStr + "\"}";
    server.send(200, "application/json", json);
  });

  server.begin();
}

// ================= LOOP ====================
void loop() {
  server.handleClient();
  handleBuzzer();

  while (gpsSerial.available()) gps.encode(gpsSerial.read());

  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  float axg = ax / 16384.0;
  float ayg = ay / 16384.0;
  float azg = az / 16384.0;

  float totalAcc = sqrt(axg*axg + ayg*ayg + azg*azg);
  unsigned long now = millis();

  static float lastAcc = 1.0;
  if (fabs(totalAcc - lastAcc) > 0.05) {
    lastMovementTime = now;
    inactivityTriggered = false;
  }
  lastAcc = totalAcc;

  // ===== FALL DETECTION =====
  if (now - lastFallTime > COOLDOWN_MS) {
    if (!inLowWindow && totalAcc < FALL_LOW_G) {
      inLowWindow = true;
      lastLowTime = now;
    } 
    else if (inLowWindow) {
      if (totalAcc > FALL_HIGH_G) {
        fallDetected = true;
        inLowWindow = false;
      }
      if (now - lastLowTime > WINDOW_MS) inLowWindow = false;
    }
  }

  // ===== FALL EVENT =====
  if (fallDetected) {
    lastFallTime = now;
    lastMovementTime = now;

    startBuzzer(2);

    unsigned long t = millis() / 1000;
    char buf[16];
    sprintf(buf, "%02lu:%02lu:%02lu",
            (t/3600)%24, (t/60)%60, t%60);
    lastFallTimeStr = buf;

    fallDetected = false;
    inactivityTriggered = true;
    lastBuzzToggle = now;
  }

  // ===== INACTIVITY ALERT =====
  if (inactivityTriggered && now - lastMovementTime > 10000) {
    if (now - lastBuzzToggle > 10000) {
      lastBuzzToggle = now;
      startBuzzer(5);
    }
  }
}
