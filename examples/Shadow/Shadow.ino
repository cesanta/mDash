// See step-by-step guide at:
// https://mdash.net/docs/quickstart/arduino.md

#define MDASH_APP_NAME "mDashApp"
#include <mDash.h>

static int ledStatus = 0;  // Initially, LED is off. Mapped to shadow key `led`.
static int ledPin = 5;     // Default LED pin. Mapped to shadow key `pin`.

static void reportShadowState() {
  mDashShadowUpdate("{\"state\":{\"reported\":{\"led\":%B,\"pin\":%d}}}",
                    ledStatus, ledPin);
}

// This function is called when a shadow delta arrives from mdash.net
// A shadow delta is a JSON object, a difference between `state.desired`
// and `state.reported`. For example, this is how a delta may look like:
// 					{ "state": { "led" : true } }
// This delta says that a user set `state.desired.led=true`, but
// `state.reported.led=false`, thus a difference (delta) exists.
// We need to parse the delta JSON string, fetch the value for `led`, and make
// `state.reported` equal to the `state.desired`. That clears the delta.
static void onShadowDelta(void *ctx, void *userdata) {
  const char *params = mDashGetParams(ctx);
  double dv;
  if (mDashGetNum(params, "$.state.pin", &dv)) ledPin = dv;
  mDashGetBool(params, "$.state.led", &ledStatus);
  pinMode(ledPin, OUTPUT);          // Synchronise
  digitalWrite(ledPin, ledStatus);  // the hardware
  reportShadowState();              // And report, clearing the delta
}

// When we're reconnected, report our current state to shadow
static void onConnStateChange(void *event_data, void *user_data) {
  long connection_state = (long) event_data;
  if (connection_state == MDASH_CONNECTED) reportShadowState();
}

void setup() {
  Serial.begin(115200);
  mDashBegin();
  mDashExport("Shadow.Delta", onShadowDelta, NULL);
  mDashRegisterEventHandler(MDASH_EVENT_CONN_STATE, onConnStateChange, NULL);

  // Until connected to the cloud, enable provisioning over serial
  while (mDashGetState() != MDASH_CONNECTED)
    if (Serial.available() > 0) mDashCLI(Serial.read());
}

void loop() {
  delay(5 * 1000);
  mDashShadowUpdate("{\"state\":{\"reported\":{\"ram_free\":%lu}}}",
                    mDashGetFreeRam());  // Report free RAM periodically
}
