#include <mDash.h>

// This sketch demonstrates how to use mdash.net device shadow. Functionality:
//  - report a (fake) temperature value to the shadow every second,
// 	- set LED on or off, based on the `state.desired.on` shadow key.
//    monitor shadow delta for `state.desired.on` shadow key. If that key
//    is changed by a user, let the LED on or off, according to the value.
//
// See device shadow tutorial at:
// https://forum.mdash.net/t/remote-control-via-device-shadow-overview/18

#define LED_PIN 5

#define WIFI_NAME "YOUR_WIFI_NETWORK"
#define WIFI_PASS "YOUR_WIFI_PASSWORD"
#define DEVICE_ID "DEVICE_ID"
#define DEVICE_TOKEN "DEVICE_TOKEN"

static const char *s_topic = DEVICE_ID "/shadow/update";  // Shadow update topic

// This function is called when a shadow delta arrives from mdash.net
// A shadow delta is a JSON object, a difference between `state.desired`
// and `state.reported`. For example:
// 					{ "state": { "led" : true } }
// This delta says that a user set `state.desired.led=true`, but our current
// is `false`. We need to parse the delta, fetch the value for `led`,
// set our current state to this value, and report the sate to clear the delta.
static void onShadowDelta(const char *topic, const char *message) {
  static int led = 0;
  double v = 0;
  printf("DELTA: %s\n", message);
  if (mDashGetBool(message, "$.state.led", &v)) {
    led = v;                     // Set LED variable to the desired value
    pinMode(LED_PIN, OUTPUT);    // And synchronise
    digitalWrite(LED_PIN, led);  // the hardware
  }
  mDashPublish(s_topic, "{%Q:{%Q:{%Q:%B}}}", "state", "reported",
               "led", led);
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
