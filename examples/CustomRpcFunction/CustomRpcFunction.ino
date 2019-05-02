#include <WiFi.h>
#include <mDash.h>

#define WIFI_NAME "YOUR_WIFI_NETWORK"
#define WIFI_PASS "YOUR_WIFI_PASSWORD"
#define DEVICE_ID "DEVICE_ID"
#define DEVICE_TOKEN "DEVICE_TOKEN"

// RPC function that adds two numbers, "a" and "b", which should be
// passed as parameters: {"a": 1.23, "b": 4.56}
// See https://forum.mdash.net/t/rpc-overview/36 for more information
static void cb(void *ctx, void *cbdata) {
  double a = mDashGetDouble(ctx, "$.a", -1);
  double b = mDashGetDouble(ctx, "$.b", -1);
  if (a == -1 || b == -1) {
    mDashReturnError(ctx, "%Q", "expecting params {\"a\": ..., \"b\": ...}");
  } else {
    mDashReturnSuccess(ctx, "{%Q: %f}", "result", a + b);
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_NAME, WIFI_PASS);
  Serial.print("\nConnecting to wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.print("connected\n");

  mDashInit(DEVICE_ID, DEVICE_TOKEN);
  mDashExport("Sum", cb, NULL);
}

void loop() {
  delay(1000);
}
