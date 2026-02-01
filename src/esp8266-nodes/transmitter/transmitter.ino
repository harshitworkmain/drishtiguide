#include <ESP8266WiFi.h>
#include <espnow.h>

#define TRIG_PIN D6
#define ECHO_PIN D5

// Structure for sending data
typedef struct struct_message {
  int distance;
} struct_message;

struct_message myData;

// MAC address of Receiver ESP (replace with your receiver’s MAC)
uint8_t receiverAddress[] = {0x24, 0x6F, 0x28, 0x12, 0x34, 0x56};

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0) {
    Serial.println("Delivery success");
  } else {
    Serial.println("Delivery fail");
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Set device as Wi-Fi station
  WiFi.mode(WIFI_STA);

  // Initialize ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Set ESP-NOW role
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);

  // Register callback
  esp_now_register_send_cb(OnDataSent);

  // Add receiver peer
  esp_now_add_peer(receiverAddress,
                   ESP_NOW_ROLE_SLAVE,
                   1,
                   NULL,
                   0);
}

void loop() {
  // Trigger ultrasonic pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Measure echo duration
  long duration = pulseIn(ECHO_PIN, HIGH);

  // Calculate distance in cm
  int distance = duration * 0.034 / 2;

  // Assign data
  myData.distance = distance;

  // Send data via ESP-NOW
  esp_now_send(receiverAddress,
               (uint8_t *) &myData,
               sizeof(myData));

  Serial.print("Distance Sent (cm): ");
  Serial.println(distance);

  delay(200);
}
