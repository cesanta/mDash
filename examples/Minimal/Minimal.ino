#define MDASH_APP_NAME "myApp"
#include <mDash.h>

void setup() {
  Serial.begin(115200);
  mDashBegin();
  while (mDashGetState() != MDASH_CONNECTED)
    if (Serial.available() > 0) mDashCLI(Serial.read());
}

void loop() {
  delay(100);
}
