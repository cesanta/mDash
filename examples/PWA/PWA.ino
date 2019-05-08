#include <mDash.h>

#define LED_PIN 5

#define WIFI_NAME "YOUR_WIFI_NETWORK"
#define WIFI_PASS "YOUR_WIFI_PASSWORD"
#define DEVICE_ID "DEVICE_ID"
#define DEVICE_TOKEN "DEVICE_TOKEN"

static const char *s_topic = DEVICE_ID "/shadow/update";  // Shadow update topic

static void onShadowDelta(const char *topic, const char *message) {
  static int led = 0, cnt = 0;
  double v = 0;
  printf("DELTA: %s\n", message);
  if (mDashGetBool(message, "$.state.lights", &led)) {
    pinMode(LED_PIN, OUTPUT);    // A toggle button has been changed state.
    digitalWrite(LED_PIN, led);  // Set the LED pin accordingly
  }
  if (mDashGetNum(message, "$.state.reboot", &v)) {
    if (cnt == 0) {     // If the reboot counter is not initialised
      cnt = v;          // Then initialise it to the current value in a shadow
    } else {            // Otherwise, the button was pressed on the app
      *(int *) 0 = 42;  // therefore crash by writing to the zero address
    }
  }
  mDashPublish(s_topic, "{%Q:{%Q:{%Q:%B,%Q:%d}}}", "state", "reported",
               "lights", led, "reboot", cnt);
}

void setup() {
  Serial.begin(115200);
  mDashStartWithWifi(WIFI_NAME, WIFI_PASS, DEVICE_ID, DEVICE_TOKEN);
  mDashSubscribe(DEVICE_ID "/shadow/delta", onShadowDelta);
}

void loop() {
  delay(1000);  // Sleep 1 second, then report a (simulated) temperature
  mDashPublish(s_topic, "{%Q:{%Q:{%Q:%g}}}", "state", "reported", "temp",
               20 + (double) rand() / RAND_MAX * 5.0);
}