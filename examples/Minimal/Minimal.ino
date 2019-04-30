#include <WiFi.h>
#include <mDash.h>

#define WIFI_NAME "YOUR_WIFI_NAME"
#define WIFI_PASS "YOUR_WIFI_PASSWORD"
#define DEVICE_ID "MDASH_DEVICE_ID"
#define DEVICE_TOKEN "MDASH_DEVICE_TOKEN"

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_NAME, WIFI_PASS);
  Serial.print("\nConnecting to wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.print("connected\n");

  mDash.begin(DEVICE_ID, DEVICE_TOKEN);
}

void loop() {
  delay(1000);
}
