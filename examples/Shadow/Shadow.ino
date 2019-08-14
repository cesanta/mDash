// See step-by-step guide at:
// https://mdash.net/docs/quickstart/arduino.md

#define MDASH_APP_NAME "mDashApp"
#include <mDash.h>

static int ledStatus = 0;  // Initially, LED is off. Mapped to shadow key `led`.
static int ledPin = 5;     // Default LED pin. Mapped to shadow key `pin`.

static void reportShadowState() {
  mDashShadowUpdate(
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
  printf("Topic: %s, message: %s\n", topic, message);
  if (mDashGetNum(message, "$.state.pin", &dv)) ledPin = dv;
  mDashGetBool(message, "$.state.led", &ledStatus);
  pinMode(ledPin, OUTPUT);          // Synchronise
  digitalWrite(ledPin, ledStatus);  // the hardware
  reportShadowState();              // And report, clearing the delta
}

void setup() {
  Serial.begin(115200);
  mDashBegin();
  mDashShadowDeltaSubscribe(onShadowDelta);          // handle delta
  mDashOn((void (*)(void *)) reportShadowState, 0);  // report on connect

  // Until connected to the cloud, enable provisioning over serial
  while (mDashGetState() != MDASH_CONNECTED)
    if (Serial.available() > 0) mDashCLI(Serial.read());
}

void loop() {
  delay(5 * 1000);
  // Save current free RAM to the database - for graphing
  String topic = String("db/") + mDashGetDeviceID() + "/ram";
  mDashPublish(topic.c_str(), "{%Q:%lu}", "free_ram", mDashGetFreeRam());
  reportShadowState();  // Report current shadow state
}
