// Copyright (c) 2026 Cesanta Software Limited
// All rights reserved

#define MDASH_APP_NAME "esp32-mdash-example"
#include "mDash.h"

#include "esp_event.h"
#include "esp_netif.h"
#include "esp_netif_ip_addr.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"

// WiFi network credentials
#define WIFI_NETWORK "MyWiFiNetworkName"
#define WIFI_PASSWORD "MyWiFiPassword"
#define DEVICE_PASSWORD "mDashDeviceToken"

// WiFi and IP event handler: connect, reconnect on disconnect, log the IP
static void wifi_event_handler(void *arg, esp_event_base_t base, int32_t id,
                               void *data) {
  (void) arg;
  if (base == WIFI_EVENT && id == WIFI_EVENT_STA_START) {
    esp_wifi_connect();
  } else if (base == WIFI_EVENT && id == WIFI_EVENT_STA_DISCONNECTED) {
    esp_wifi_connect();
  } else if (base == IP_EVENT && id == IP_EVENT_STA_GOT_IP) {
    ip_event_got_ip_t *event = (ip_event_got_ip_t *) data;
    MG_INFO(("WiFi connected, IP: " IPSTR, IP2STR(&event->ip_info.ip)));
  }
}

// Initialise WiFi in station mode and start connecting to WIFI_NETWORK
static void wifi_connect(void) {
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_sta();

  ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                             wifi_event_handler, NULL));
  ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                             wifi_event_handler, NULL));

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  wifi_config_t wc = {
      .sta = {.ssid = WIFI_NETWORK, .password = WIFI_PASSWORD},
  };
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wc));
  ESP_ERROR_CHECK(esp_wifi_start());  // connects via wifi_event_handler
}

// Return true if the default WiFi STA interface has an IP address
static bool wifi_is_connected(void) {
  esp_netif_ip_info_t ip;
  esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
  return netif != NULL &&
         esp_netif_get_ip_info(netif, &ip) == ESP_OK && ip.ip.addr != 0;
}

void app_main(void) {
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);

  wifi_connect();
  while (!wifi_is_connected()) {
    vTaskDelay(pdMS_TO_TICKS(500));
    MG_INFO(("Trying WiFi %s ...", WIFI_NETWORK));
  }
  mDashBegin(DEVICE_PASSWORD);
  for (;;) mDashPoll();
}
