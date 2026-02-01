#include <ESP8266WiFi.h>
#include <espnow.h>

// Define motor pins
#define MOTOR1 D1
#define MOTOR2 D2
#define MOTOR3 D3
#define MOTOR4 D4
#define MOTOR5 D5

// Structure for receiving data
typedef struct struct_message {
  int distance;
} struct_message;

struct_message incomingData;

// ESP-NOW receive callback
void OnDataRecv(uint8_t * mac, uint8_t *incomingDataBytes, uint8_t len) {
  memcpy(&incomingData, incomingDataBytes, sizeof(incomingData));

  int d = incomingData.distance;
  Serial.print("Distance Received: ");
  Serial.println(d);

  // Reset all motors
  digitalWrite(MOTOR1, LOW);
  digitalWrite(MOTOR2, LOW);
  digitalWrite(MOTOR3, LOW);
  digitalWrite(MOTOR4, LOW);
  digitalWrite(MOTOR5, LOW);

  // Distance-to-haptic mapping
  if (d <= 100 && d > 80) {
    digitalWrite(MOTOR1, HIGH);
  }
  else if (d <= 80 && d > 60) {
    digitalWrite(MOTOR1, HIGH);
    digitalWrite(MOTOR2, HIGH);
  }
  else if (d <= 60 && d > 40) {
    digitalWrite(MOTOR1, HIGH);
    digitalWrite(MOTOR2, HIGH);
    digitalWrite(MOTOR3, HIGH);
  }
  else if (d <= 40 && d > 20) {
    digitalWrite(MOTOR1, HIGH);
    digitalWrite(MOTOR2, HIGH);
    digitalWrite(MOTOR3, HIGH);
    digitalWrite(MOTOR4, HIGH);
  }
  else if (d <= 20) {
    digitalWrite(MOTOR1, HIGH);
    digitalWrite(MOTOR2, HIGH);
    digitalWrite(MOTOR3, HIGH);
    digitalWrite(MOTOR4, HIGH);
    digitalWrite(MOTOR5, HIGH);
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(MOTOR1, OUTPUT);
  pinMode(MOTOR2, OUTPUT);
  pinMode(MOTOR3, OUTPUT);
  pinMode(MOTOR4, OUTPUT);
  pinMode(MOTOR5, OUTPUT);

  // Set device as Wi-Fi station
  WiFi.mode(WIFI_STA);

  // Initialize ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Set ESP-NOW role
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);

  // Register receive callback
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // All logic handled via ESP-NOW callback
}
