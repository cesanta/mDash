// Custom RPC function example.
// Read more on RPC at https://mdash.net/docs/userguide/remote_control.md
// JSON-RPC API is documented at: https://mongoose.ws/documentation/
//
// - Install mDash library:
//   - Select "Sketch" &rarr; "Include Library" &rarr; "Manage Libraries"
//   - In the search field, type "mDash" and press Enter
//   - Click on "Install" to install the library
// - Select "Tools" → "Board" → "ESP32 Dev Module"
// - Select "Tools" → "Partitioning Scheme" → "Minimal SPIFFS"
// - Select "Tools" → "Port" → your serial port
// - Click on "Upload" button to build and flash the firmware

#define MDASH_APP_NAME "RpcApp"
#include <mDash.h>

#include <WiFi.h>

#define WIFI_NETWORK "MyWifiNetworkName"
#define WIFI_PASSWORD "MyWifiPassword"
#define DEVICE_PASSWORD "mDashDeviceToken"

static void rpc_gpio_write(struct mg_rpc_req *r) {
  long pin = mg_json_get_long(r->frame, "$.params.pin", -1);
  long val = mg_json_get_long(r->frame, "$.params.val", -1);
  if (pin < 0 || val < 0) {
    mg_rpc_err(r, 500, "%Q", "pin and val required");
  } else {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, val);
    mg_rpc_ok(r, "true");
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);
  mDashBegin(DEVICE_PASSWORD);
  mg_rpc_add(&g_rpcs, mg_str("GPIO.Write"), rpc_gpio_write, NULL);
}

void loop() {
  delay(3000);
}
