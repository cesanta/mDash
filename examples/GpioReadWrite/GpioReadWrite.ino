// GPIO read/write via the RPC mechanism.
// Read more on RPC at https://mdash.net/docs/userguide/remote_control.md
// JSON-RPC API is documented at: https://github.com/cesanta/mjson
//
// - Install mDash library:
//   - Select "Sketch" &rarr; "Include Library" &rarr; "Manage Libraries"
//   - In the search field, type "mDash" and press Enter
//   - Click on "Install" to install the library
// - Select "Tools" → "Board" → "ESP32 Dev Module"
// - Select "Tools" → "Partitioning Scheme" → "Minimal SPIFFS"
// - Select "Tools" → "Port" → your serial port
// - Click on "Upload" button to build and flash the firmware

#define MDASH_APP_NAME "GpioControlApp"
#include <mDash.h>

#include <WiFi.h>

#define WIFI_NETWORK "MyWifiNetworkName"
#define WIFI_PASSWORD "MyWifiPassword"
#define DEVICE_PASSWORD "mDashDeviceToken"

static void handleGpioRead(struct jsonrpc_request *r) {
  double pin;
  if (mjson_get_number(r->params, r->params_len, "$.pin", &pin)) {
    pinMode(pin, INPUT);
    jsonrpc_return_success(r, "%d", digitalRead(pin));
  } else {
    jsonrpc_return_error(r, JSONRPC_ERROR_BAD_PARAMS, "pin required", NULL);
  }
}

static void handleGpioWrite(struct jsonrpc_request *r) {
  double pin, value;
  if (mjson_get_number(r->params, r->params_len, "$.pin", &pin) &&
      mjson_get_number(r->params, r->params_len, "$.value", &value)) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, value);
    jsonrpc_return_success(r, "true");
  } else {
    jsonrpc_return_error(r, JSONRPC_ERROR_BAD_PARAMS, "pin, value required", 0);
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);
  mDashBegin(DEVICE_PASSWORD);
  jsonrpc_export("GPIO.Read", handleGpioRead, NULL);
  jsonrpc_export("GPIO.Write", handleGpioWrite, NULL);
}

void loop() {
  delay(3000);
  mDashNotify("DB.Save", "{%Q:%lu}", "ram", mDashGetFreeRam());  // Report data
}
