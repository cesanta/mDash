#include <mDash.h>

#define WIFI_NAME "YOUR_WIFI_NETWORK"
#define WIFI_PASS "YOUR_WIFI_PASSWORD"
#define DEVICE_ID "DEVICE_ID"
#define DEVICE_TOKEN "DEVICE_TOKEN"

void setup() {
  Serial.begin(115200);
  mDashStartWithWifi(WIFI_NAME, WIFI_PASS, DEVICE_ID, DEVICE_TOKEN);
}

void loop() {
  delay(120 * 1000);  // Sleep 2 minutes
  mDashPublish("db/data/" DEVICE_ID, "{\"free_ram\":%d}",  // Report free memory
               mDashGetFreeRam());  // could also be sensor values, etc
}
