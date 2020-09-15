// This Arduino sketch demonstrates how to use Serial CLI to configure your
// device. Devices that are configured via CLI (or via WiFi Access Point)
// store their credentials in the config file, rather than hardcoding them
// into firmware code. That allows to have a fleet of devices, and OTA them
// using the same firmware.
//
// - Install mDash library:
//   - Select "Sketch" &rarr; "Include Library" &rarr; "Manage Libraries"
//   - In the search field, type "mDash" and press Enter
//   - Click on "Install" to install the library
// - Select "Tools" → "Board" → "ESP32 Dev Module"
// - Select "Tools" → "Partitioning Scheme" → "Minimal SPIFFS"
// - Select "Tools" → "Port" → your serial port
// - Click on "Upload" button to build and flash the firmware
// 
// Start Serial monitor, enter the following commands:
//   set wifi.sta.ssid YOUR_WIFI_NETWORK
//   set wifi.sta.pass YOUR_WIFI_PASSWORD
//   set device.pass MDASH_DEVICE_PASSWORD
//   reboot
//
// When done, your device should become online on mdash.net.
// See https://mdash.net/docs/ for more.

#define MDASH_APP_NAME "CliApp"
#include <mDash.h>

#include <WiFi.h>

static void initWiFi(const char *ssid, const char *pass) {
  if (ssid != NULL) WiFi.begin((char *) ssid, pass);  // WiFi is configured
  if (ssid == NULL) WiFi.softAP("CliAP", pass);  // Not configured, start AP
}

void setup() {
  Serial.begin(115200);
  mDashBeginWithWifi(initWiFi, NULL, NULL, NULL);
}

void loop() {
  if (Serial.available() > 0) mDashCLI(Serial.read());
}
