#define WIFI_NAME "YOUR_WIFI_NETWORK"
#define WIFI_PASS "YOUR_WIFI_PASSWORD"
#define DEVICE_ID "DEVICE_ID"
#define DEVICE_TOKEN "DEVICE_TOKEN"

#include <mDash.h>

// This sketch demonstrates how to use mdash.net device shadow:
// 	- set LED on or off, based on the `state.desired.led` shadow key
//  - report free RAM value to the database periodically
//
// See device shadow tutorial at:
// https://forum.mdash.net/t/remote-control-via-device-shadow-overview/18

static int ledStatus = 0;  // Initially, LED is off. Mapped to shadow key `led`.
static int ledPin = 5;     // Default LED pin. Mapped to shadow key `pin`.

static void reportShadowState() {
  mDashPublish(DEVICE_ID "/shadow/update",  // Shadow update topic
               "{\"state\":{\"reported\":{\"led\":%B,\"pin\":%d,\"ram\":%lu}}}",
               ledStatus, ledPin, mDashGetFreeRam());
}

// This function is called when a shadow delta arrives from mdash.net
// A shadow delta is a JSON object, a difference between `state.desired`
// and `state.reported`. For example, this is how a delta may look like:
// 					{ "state": { "led" : true } }
// This delta says that a user set `state.desired.led=true`, but
// `state.reported.led=false`, thus a difference (delta) exists.
// We need to parse the delta JSON string, fetch the value for `led`, and make
// `state.reported` equal to the `state.desired`. That clears the delta.
static void onShadowDelta(const char *topic, const char *message) {
  double dv;
  printf("DELTA: %s\n", message);
  if (mDashGetNum(message, "$.state.pin", &dv)) ledPin = dv;
  mDashGetBool(message, "$.state.led", &ledStatus);
  pinMode(ledPin, OUTPUT);          // Synchronise
  digitalWrite(ledPin, ledStatus);  // the hardware
  reportShadowState();              // And report, clearing the delta
}

void setup() {
  Serial.begin(115200);
  mDashSetServer("192.168.1.86", 1883);
  mDashStartWithWifi(WIFI_NAME, WIFI_PASS, DEVICE_ID, DEVICE_TOKEN);
  mDashSubscribe(DEVICE_ID "/shadow/delta", onShadowDelta);  // handle delta
  mDashOn((void (*)(int, void *)) reportShadowState, 0);  // report on connect
}

void loop() {
  delay(5 * 1000);
  // Save current free RAM to the database - for graphing
  mDashPublish("db/" DEVICE_ID "/ram", "%lu", mDashGetFreeRam());
  reportShadowState();  // Report current shadow state
}
