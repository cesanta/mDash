#include <mDash.h>

#define LED_PIN 2

#define WIFI_NAME "YOUR_WIFI_NETWORK"
#define WIFI_PASS "YOUR_WIFI_PASSWORD"
#define DEVICE_ID "DEVICE_ID"
#define DEVICE_TOKEN "DEVICE_TOKEN"

static void onShadowDelta(const char *topic, const char *message) {
  printf("Got shadow delta: %s -> %s\n", topic, message);
}

void setup() {
  Serial.begin(115200);
  mDashStartWithWifi(WIFI_NAME, WIFI_PASS, DEVICE_ID, DEVICE_TOKEN);
  mDashSubscribe(DEVICE_ID "/shadow/delta", onShadowDelta);
}

void loop() {
  delay(1000);
}
