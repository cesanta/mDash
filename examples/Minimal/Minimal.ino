#define MDASH_APP_NAME "myApp"
#include <mDash.h>

// Flash this firmware to the device, then configure WiFi and cloud credentials
// by following steps 6,7,8,9 of the quick start quide:
// https://mdash.net/docs/quickstart/arduino.md#6-register-new-device

void setup() {
  Serial.begin(115200);
  mDashBegin();

  // Until connected to the cloud, enable provisioning over serial
  while (mDashGetState() != MDASH_CONNECTED)
    if (Serial.available() > 0) mDashCLI(Serial.read());
}

void loop() {
  delay(100);
}
