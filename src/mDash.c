// Copyright (c) 2019-2022 Cesanta Software Limited
// All rights reserved
//
// This software is dual-licensed: you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation. For the terms of this
// license, see http://www.gnu.org/licenses/
//
// You are free to use this software under the terms of the GNU General
// Public License, but WITHOUT ANY WARRANTY; without even the implied
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// Alternatively, you can license this software under a commercial
// license, as set out in https://www.mongoose.ws/licensing/
//
// SPDX-License-Identifier: GPL-2.0 or commercial

#include "mDash.h"

static const char *s_mDashAppName = "arduino";
static const char *s_mDashFramework = "?";
static const char *s_mDashPassword = NULL;
static const char *s_mDashPublicKey = NULL;
static const char *s_mDashBuildTime = NULL;
static const char *s_mDashURL = "wss://mdash.net/api/v2/rpc";
static int s_mDashLogLevel = MG_LL_INFO;
static int s_mDashState = MDASH_STATE_NETWORK_LOST;
static struct mg_connection *s_conn;         // Cloud connection
static struct mg_connection *s_sntp = NULL;  // SNTP connection
static struct mg_connection *s_http = NULL;  // HTTP server connection
static struct mg_mgr s_mgr;                  // Mongoose event manager
static time_t s_start_time = 0;              // Starting timestamp
static size_t s_ota_size = 0;                // Firmware size
static size_t s_ota_written = 0;             // Written firmware size
struct mg_rpc *g_rpcs;                       // Registered RPC handlers

static const char *s_pem =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw\n"
    "TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n"
    "cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4\n"
    "WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu\n"
    "ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY\n"
    "MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc\n"
    "h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+\n"
    "0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U\n"
    "A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW\n"
    "T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH\n"
    "B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC\n"
    "B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv\n"
    "KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn\n"
    "OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn\n"
    "jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw\n"
    "qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI\n"
    "rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV\n"
    "HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq\n"
    "hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL\n"
    "ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ\n"
    "3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK\n"
    "NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5\n"
    "ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur\n"
    "TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC\n"
    "jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc\n"
    "oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq\n"
    "4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA\n"
    "mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d\n"
    "emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=\n"
    "-----END CERTIFICATE-----\n";

#if defined(ESP_PLATFORM)  //////////////////////////////////// ESP32 specific
#include <esp_mac.h>
#include <esp_ota_ops.h>
#include <esp_spiffs.h>
#include <freertos/timers.h>
#define HTTP_URL "http://0.0.0.0:80"
#define MDASH_ARCH "esp32"
#define CPU_CORE 0
#define FS_ROOT "/spiffs"
#define MDASH_CONFIG_FILE_NAME FS_ROOT "/mdash.cfg"
static const esp_partition_t *s_update_partition;
static esp_ota_handle_t s_update_handle;

static void enable_wdt(void) {
  esp_task_wdt_add(xTaskGetIdleTaskHandleForCPU(CPU_CORE));
}
static void disable_wdt(void) {
  esp_task_wdt_delete(xTaskGetIdleTaskHandleForCPU(CPU_CORE));
}
static unsigned long mDashGetFreeRam(void) {
  return xPortGetFreeHeapSize();
}
static void reboot_with_delay(int delay_ms) {
  void *h = xTimerCreate("reboot", pdMS_TO_TICKS(delay_ms), 0, NULL,
                         (void (*)(void *)) esp_restart);
  xTimerStart(h, 0);
}
static SemaphoreHandle_t s_sem = NULL;
static void MDashMutexInit(void) {
  s_sem = xSemaphoreCreateMutex();
}
static void MDashMutexLock(void) {
  xSemaphoreTakeRecursive(s_sem, portMAX_DELAY);
}
static void MDashMutexUnlock(void) {
  xSemaphoreGiveRecursive(s_sem);
}
static bool mount_fs(const char *path) {
  disable_wdt();
  esp_vfs_spiffs_conf_t conf = {
      .base_path = path,
      .max_files = 20,
      .format_if_mount_failed = true,
  };
  int res = esp_vfs_spiffs_register(&conf);
  enable_wdt();
  MG_INFO(("FS -> %s: %d", conf.base_path, res));
  return res == ESP_OK;
}
static uint64_t get_chip_id(void) {
  uint64_t id = 0;
  // efuse_hal_get_mac((uint8_t *) &id);
  esp_read_mac((uint8_t *) &id, ESP_MAC_WIFI_STA);
  return id;
}
#else  /////////////////////////////////////////////////////// POSIX specific
#define HTTP_URL "http://0.0.0.0:8080"
#define xTaskCreatePinnedToCore(fn, b, c, d, e, f, g)
#define MDASH_ARCH "posix"
#define FS_ROOT "/tmp/spiffs"
#define MDASH_CONFIG_FILE_NAME FS_ROOT "/mdash.cfg"
static unsigned long mDashGetFreeRam(void) {
  return 42;
}
static void reboot_with_delay(int delay_ms) {
  MG_INFO(("outta reboot in %d milliseconds", delay_ms));
}
static void MDashMutexInit(void) {
}
static void MDashMutexLock(void) {
}
static void MDashMutexUnlock(void) {
}
static bool mount_fs(const char *path) {
  if (path != NULL) mkdir(path, 0644);
  return true;
}
static int esp_reset_reason(void) {
  return 0;
}
static uint64_t get_chip_id(void) {
  return 0;
}
#endif  ///////////////////////////////////////////// END of plaform-specific

static const char *get_reset_reason(void) {
  switch (esp_reset_reason()) {
#if defined(ESP_PLATFORM)
    case ESP_RST_POWERON:
      return "power-on";
    case ESP_RST_EXT:
      return "external pin";
    case ESP_RST_SW:
      return "soft reset";
    case ESP_RST_PANIC:
      return "crash";
    case ESP_RST_INT_WDT:
      return "interrupt watchdog (software or hardware)";
    case ESP_RST_TASK_WDT:
      return "task watchdog";
    case ESP_RST_WDT:
      return "other watchdog";
    case ESP_RST_DEEPSLEEP:
      return "deep sleep wakeup";
    case ESP_RST_BROWNOUT:
      return "Brownout (software or hardware)";
    case ESP_RST_SDIO:
      return "Reset over SDIO";
#endif
    default:
      return "unknown";
  }
}

static void asctimetoiso(char *buf, const char *ts) {
  const char *month = "01";
  if (ts == NULL || strlen(ts) < 20) ts = "Jan  1 1970-00:00:00";
  if (ts[0] == 'F') {
    month = "02";
  } else if (ts[0] == 'M') {
    month = ts[2] == 'y' ? "05" : "03";
  } else if (ts[0] == 'A') {
    month = ts[1] == 'p' ? "04" : "08";
  } else if (ts[0] == 'J') {
    month = ts[1] == 'a' ? "01" : ts[2] == 'n' ? "06" : "07";
  } else if (ts[0] == 'S') {
    month = "09";
  } else if (ts[0] == 'O') {
    month = "10";
  } else if (ts[0] == 'N') {
    month = "11";
  } else if (ts[0] == 'D') {
    month = "12";
  }

  memcpy(buf, ts + 7, 4);
  memcpy(buf + 4, month, 2);
  buf[6] = ts[4] == ' ' ? '0' : ts[4];
  buf[7] = ts[5];
  buf[8] = '-';
  memcpy(buf + 9, ts + 12, 2);
  memcpy(buf + 11, ts + 15, 2);
  memcpy(buf + 13, ts + 18, 2);
  buf[15] = '\0';
}

static void rpc_get_info(struct mg_rpc_req *r) {
  unsigned long uptime = time(NULL) - s_start_time;
  uint64_t chipid = get_chip_id();
  char buf[21];
  asctimetoiso(buf, s_mDashBuildTime);
  mg_rpc_ok(r,
            "{%Q:\"%s-%s\",%Q:%Q,%Q:%Q,%Q:%Q,%Q:%lu,%Q:%lu,%Q:%Q,%Q:%H,%Q:%Q}",
            "fw_version", MDASH_VERSION, s_mDashFramework, "arch", MDASH_ARCH,
            "fw_id", buf, "app", s_mDashAppName, "uptime", uptime, "ram_free",
            mDashGetFreeRam(), "reboot_reason", get_reset_reason(), "id",
            sizeof(chipid), &chipid, "public_key",
            s_mDashPublicKey == NULL ? "" : s_mDashPublicKey);
}

static void rpc_reboot(struct mg_rpc_req *r) {
  mg_rpc_ok(r, "true");
  reboot_with_delay(1000);
}

static void rpc_sys_set(struct mg_rpc_req *r) {
  char *name = mg_json_get_str(r->frame, "$.params.name");
  char *value = mg_json_get_str(r->frame, "$.params.value");
  if (name == NULL) {
    mg_rpc_err(r, 400, "%Q", "name and value expected");
  } else {
    int result = mDashConfigSet(name, value);
    mg_rpc_ok(r, "{%Q:%d}", "result", result);
  }
  free(name);
  free(value);
}

static void rpc_fs_format(struct mg_rpc_req *r) {
#if defined(ESP_PLATFORM)
  disable_wdt();
  esp_err_t err = esp_spiffs_format(NULL);
  enable_wdt();
  if (err) {
    mg_rpc_err(r, 500, "\"spiffs_format err %d\"", err);
  } else {
    mg_rpc_ok(r, "true");
  }
#else
  mg_rpc_ok(r, "true");
#endif
}

static void rpc_fs_mount(struct mg_rpc_req *r) {
  // char path[20] = "/";
  char *path = mg_json_get_str(r->frame, "$.params.path");
#if defined(ESP_PLATFORM)
  if (esp_spiffs_mounted(NULL)) {
    mg_rpc_err(r, 500, "%Q", "already mounted");
  } else if (mount_fs(path == NULL ? FS_ROOT : path)) {
    mg_rpc_ok(r, "true");
  } else {
    mg_rpc_err(r, 500, "%Q", "mount failed");
  }
#else
  mg_rpc_ok(r, "true");
#endif
  free(path);
}

static void rpc_fs_put(struct mg_rpc_req *r) {
  bool append = false;
  char *name = mg_json_get_str(r->frame, "$.params.filename");
  int len = 0, ofs = mg_json_get(r->frame, "$.params.data", &len);
  mg_json_get_bool(r->frame, "$.params.append", &append);
  if (len <= 0 || r->frame.ptr[ofs] != '"' || name == NULL) {
    mg_rpc_err(r, 500, "%Q", "data and filename required");
  } else {
    char path[MG_PATH_MAX];
    FILE *fp;
    mg_snprintf(path, sizeof(path), "%s/%s", FS_ROOT, name);
    if ((fp = fopen(path, append ? "ab" : "wb")) == NULL) {
      mg_rpc_err(r, 500, "\"fopen(%q): %d\"", path, errno);
    } else {
      char *data = (char *) &r->frame.ptr[ofs];
      int n = mg_base64_decode(data + 1, len - 2, data);  // Decode in-place
      if ((int) fwrite(data, 1, n, fp) != n) {
        mg_rpc_err(r, 500, "\"fwrite(%q) %d, errno %d\"", path, n, errno);
      } else {
        mg_rpc_ok(r, "{%Q:%Q,%Q:%d}", "filename", name, "written", n);
      }
      fclose(fp);
    }
  }
  free(name);
}

static void rpc_fs_get(struct mg_rpc_req *r) {
  long offset = mg_json_get_long(r->frame, "$.params.offset", 0);
  long len = mg_json_get_long(r->frame, "$.params.len", 512);
  char *chunk = NULL;
  char *name = mg_json_get_str(r->frame, "$.params.filename");
  if (name == NULL) {
    mg_rpc_err(r, 500, "%Q", "filename required");
  } else if ((chunk = (char *) malloc(len)) == NULL) {
    mg_rpc_err(r, 500, "%Q", "OOM");
  } else {
    FILE *fp = NULL;
    char path[MG_PATH_MAX];
    mg_snprintf(path, sizeof(path), "%s/%s", FS_ROOT, name);
    if ((fp = fopen(path, "rb")) == NULL) {
      mg_rpc_err(r, 500, "\"fopen(%q): %d\"", path, errno);
    } else {
      fseek(fp, offset, SEEK_SET);
      long n = fread(chunk, 1, len, fp);
      fseek(fp, 0, SEEK_END);
      long total = ftell(fp);
      mg_rpc_ok(r, "{%Q:%Q,%Q:%ld,%Q:%ld,%Q:%V}", "filename", name, "offset",
                offset, "left", total - (n + offset), "data", (int) n, chunk);
      fclose(fp);
    }
  }
  free(chunk);
  free(name);
}

static void rpc_fs_rename(struct mg_rpc_req *r) {
  char *src = mg_json_get_str(r->frame, "$.params.src");
  char *dst = mg_json_get_str(r->frame, "$.params.dst");
  if (src == NULL || dst == NULL) {
    mg_rpc_err(r, 500, "%Q", "src and dst missing");
  } else {
    char a[MG_PATH_MAX], b[MG_PATH_MAX];
    mg_snprintf(a, sizeof(a), "%s/%s", FS_ROOT, src);
    mg_snprintf(b, sizeof(b), "%s/%s", FS_ROOT, dst);
    remove(b);
    if (rename(a, b) != 0) {
      mg_rpc_err(r, 500, "\"rename(%s, %s) failed: %d\"", a, a, errno);
    } else {
      mg_rpc_ok(r, "true");
    }
  }
  free(src);
  free(dst);
}

static void rpc_fs_remove(struct mg_rpc_req *r) {
  char *name = mg_json_get_str(r->frame, "$.params.filename");
  if (name == NULL) {
    mg_rpc_err(r, 500, "%Q", "filename required");
  } else {
    char path[MG_PATH_MAX];
    mg_snprintf(path, sizeof(path), "%s/%s", FS_ROOT, name);
    if (remove(path) != 0) {
      mg_rpc_err(r, 500, "", "\"remove(%s) failed: %d\"", path, errno);
    } else {
      mg_rpc_ok(r, "true");
    }
  }
  free(name);
}

static size_t fslister(mg_pfn_t pfn, void *pfn_data, va_list *ap) {
  DIR *dirp = va_arg(*ap, DIR *);
  struct dirent *dp;
  const char *comma = "";
  size_t n = 0;
  while ((dp = readdir(dirp)) != NULL) {
    if (strcmp((const char *) dp->d_name, ".") == 0 ||
        strcmp((const char *) dp->d_name, "..") == 0) {
      continue;
    }
    n += mg_rprintf(pfn, pfn_data, "%s%Q", comma, dp->d_name);
    comma = ",";
  }
  return n;
}

static void rpc_fs_list(struct mg_rpc_req *r) {
  DIR *dirp = opendir(FS_ROOT);
  if (dirp == NULL) {
    mg_rpc_err(r, 500, "%Q", "cannot open fs root");
  } else {
    mg_rpc_ok(r, "[%M]", fslister, dirp);
    closedir(dirp);
  }
}

int mDashNotify(const char *name, const char *fmt, ...) {
  int res = 0;
  MDashMutexLock();
  va_list ap;
  if (s_conn != NULL) {
    struct mg_iobuf io = {0, 0, 0, 512};
    mg_rprintf(mg_pfn_iobuf, &io, "{%Q:%Q,%Q:", "method", name, "params");
    va_start(ap, fmt);
    mg_vrprintf(mg_pfn_iobuf, &io, fmt, &ap);
    va_end(ap);
    mg_rprintf(mg_pfn_iobuf, &io, "}");
    if (io.buf != NULL) {
      mg_ws_send(s_conn, (char *) io.buf, io.len, WEBSOCKET_OP_TEXT);
    }
    mg_iobuf_free(&io);
    res = 1;
  }
  MDashMutexUnlock();
  return res;
}

static void rpc_ota_begin(struct mg_rpc_req *r) {
  if (s_ota_size) {
    mg_rpc_err(r, 500, "%Q", "OTA already in progress");
  } else {
    double dv;
    if (mg_json_get_num(r->frame, "$.params.size", &dv)) s_ota_size = dv;
#if defined(ESP_PLATFORM)
    disable_wdt();
    s_update_partition = esp_ota_get_next_update_partition(NULL);
    esp_err_t err =
        esp_ota_begin(s_update_partition, OTA_SIZE_UNKNOWN, &s_update_handle);
    enable_wdt();
    MG_INFO(("Starting OTA. partition=%p, err=%d", s_update_partition, err));
#else
    MG_INFO(("Starting OTA: %.*s", (int) r->frame.len, r->frame.ptr));
#endif
    mDashNotify("State.Set", "{%Q:{%Q:0}}", "ota", "percent");
    mg_rpc_ok(r, "true");
  }
}

static void rpc_ota_write(struct mg_rpc_req *r) {
  int len = 0, ofs = 0;
  if (s_ota_size == 0) {
    mg_rpc_err(r, 500, "%Q", "call OTA.Begin");
  } else if ((ofs = mg_json_get(r->frame, "$.params.data", &len)) <= 0 ||
             r->frame.ptr[ofs] != '"') {
    mg_rpc_err(r, 500, "%Q", "data required");
  } else {
    // Decode in-place
    char *data = (char *) &r->frame.ptr[ofs];
    int n = mg_base64_decode(data + 1, len - 2, data);  // Decode in-place

#if defined(ESP_PLATFORM)
    disable_wdt();
    esp_err_t err = esp_ota_write(s_update_handle, data, n);
    enable_wdt();
    if (err != ESP_OK) {
      MG_ERROR(("ota finished, err %d", err));
      mg_rpc_err(r, 500, "\"esp_ota_write: %d\"", err);
      return;
    }
#endif

    s_ota_written += n;
    MG_INFO(("OTA write: got %d bytes", n));
    mDashNotify("State.Set", "{%Q:{%Q:%d}}", "ota", "percent",
                (int) (s_ota_written * 100 / s_ota_size));
    mg_rpc_ok(r, "{%Q:%d}", "queued", n);
  }
}

static void rpc_ota_end(struct mg_rpc_req *r) {
  s_ota_size = s_ota_written = 0;
  mDashNotify("State.Set", "{%Q:{%Q:null}}", "ota", "percent");
  MG_INFO(("OTA finished"));
#if defined(ESP_PLATFORM)
  esp_err_t err = esp_ota_end(s_update_handle);
  if (err != ESP_OK) {
    MG_ERROR(("ota finished, err %d", err));
    mg_rpc_err(r, 500, "\"esp_ota_end: %d\"", err);
    return;
  }
  esp_ota_set_boot_partition(s_update_partition);
  reboot_with_delay(500);
#endif
  mg_rpc_ok(r, "true");
}

static void init_rpcs(void) {
  mg_rpc_add(&g_rpcs, mg_str("RPC.List"), mg_rpc_list, NULL);
  mg_rpc_add(&g_rpcs, mg_str("Sys.GetInfo"), rpc_get_info, NULL);
  mg_rpc_add(&g_rpcs, mg_str("Sys.Reboot"), rpc_reboot, NULL);
  mg_rpc_add(&g_rpcs, mg_str("Sys.Set"), rpc_sys_set, NULL);
  mg_rpc_add(&g_rpcs, mg_str("OTA.Begin"), rpc_ota_begin, NULL);
  mg_rpc_add(&g_rpcs, mg_str("OTA.Write"), rpc_ota_write, NULL);
  mg_rpc_add(&g_rpcs, mg_str("OTA.End"), rpc_ota_end, NULL);
  mg_rpc_add(&g_rpcs, mg_str("FS.List"), rpc_fs_list, NULL);
  mg_rpc_add(&g_rpcs, mg_str("FS.Remove"), rpc_fs_remove, NULL);
  mg_rpc_add(&g_rpcs, mg_str("FS.Rename"), rpc_fs_rename, NULL);
  mg_rpc_add(&g_rpcs, mg_str("FS.Get"), rpc_fs_get, NULL);
  mg_rpc_add(&g_rpcs, mg_str("FS.Put"), rpc_fs_put, NULL);
  mg_rpc_add(&g_rpcs, mg_str("FS.Mount"), rpc_fs_mount, NULL);
  mg_rpc_add(&g_rpcs, mg_str("FS.Format"), rpc_fs_format, NULL);
}

int mDashConfigGet(const char *name, char *buf, int bufsize) {
  MDashMutexLock();

  char line[256], var[sizeof(line)], val[sizeof(line)];
  FILE *fp = fopen(MDASH_CONFIG_FILE_NAME, "r");
  int result = 0;
  if (fp == NULL) {
    MG_DEBUG(("%s: cannot open config file: %d", name, errno));
    result = -1;  // Fail - cannot open file
    goto out;
  }
  while (fgets(line, sizeof(line), fp) != NULL) {
    if (sscanf(line, "%s %[^\r\n]", var, val) != 2) continue;
    if (var[0] == '#' || var[0] == '\0' || val[0] == '\0') continue;
    if (strcmp(name, var) != 0) continue;
    if ((int) strlen(val) > bufsize) {
      MG_ERROR(("%s: dest buffer is too small", name));
      result = -2;  // Fail - dest buffer too small
      goto out;
    }
    strcpy(buf, val);
    goto out;  // Success
  }
  result = -3;  // Fail - variable not found
out:
  if (fp != NULL) fclose(fp);
  MDashMutexUnlock();
  return result;
}

int mDashConfigSet(const char *name, const char *value) {
  MDashMutexLock();
  char line[128];
  FILE *fp = fopen(MDASH_CONFIG_FILE_NAME, "r");
  FILE *fp2 = fopen(MDASH_CONFIG_FILE_NAME ".tmp", "w");
  if (fp2 == NULL) return -1;  // Fail - cannot open temp file
  if (fp != NULL) {
    int len = strlen(name), found = 0;
    while (fgets(line, sizeof(line), fp) != NULL) {
      if (memcmp(line, name, len) == 0 && line[len] == ' ') {
        if (!found && value && value[0]) fprintf(fp2, "%s %s\n", name, value);
        found = 1;
      } else {
        fprintf(fp2, "%s", line);
      }
    }
    if (!found && value && value[0]) fprintf(fp2, "%s %s\n", name, value);
    fclose(fp);
  } else {
    if (value && value[0]) fprintf(fp2, "%s %s\n", name, value);
  }
  fclose(fp2);
  remove(MDASH_CONFIG_FILE_NAME);
  rename(MDASH_CONFIG_FILE_NAME ".tmp", MDASH_CONFIG_FILE_NAME);
  MDashMutexUnlock();
  return 0;
}

int mDashConfigReset(void) {
  MG_ERROR(("%s", "Resetting config"));
  mDashConfigSet("wifi.sta.ssid", NULL);
  mDashConfigSet("wifi.sta.pass", NULL);
  mDashConfigSet("device.public_key", NULL);
  return 0;
}

#define MDASH_DATA_FILE_NAME FS_ROOT "/mdash.unsent"
#define MDASH_DATA_FILE_SIZE_LIMIT 32768

static void pfn_file(char c, void *data) {
  fputc(c, (FILE *) data);
}

int mDashStore(const char *topic, const char *fmt, ...) {
  FILE *fp = fopen(MDASH_DATA_FILE_NAME, "a");
  if (fp == NULL) return 0;

  fprintf(fp, "%s ", topic);
  va_list ap;
  va_start(ap, fmt);
  mg_vrprintf(pfn_file, fp, fmt, &ap);
  va_end(ap);
  fputc('\n', fp);
  long size = ftell(fp);
  fclose(fp);

  if (size > MDASH_DATA_FILE_SIZE_LIMIT) {
    const char *to = MDASH_DATA_FILE_NAME ".old";
    remove(to);
    rename(MDASH_DATA_FILE_NAME, to);
  }

  mDashNotify("DB.NewData", "true");  // Tell mDash we have unsaved data
  return 1;
}

static char *gen_public_key(char *buf, size_t len) {
  uint8_t tmp[len * 3 / 4 - 3];
  mg_random(tmp, sizeof(tmp));
  mg_base64_encode(tmp, sizeof(tmp), buf);
  return buf;
}

static const char *fromcfg(const char *ptr, const char *var) {
  char buf[256];
  if (mDashConfigGet(var, buf, sizeof(buf)) == 0) ptr = strdup(buf);
  MG_DEBUG(("%s -> [%s]", var, ptr == NULL ? "NULL" : ptr));
  return ptr;
}

static bool s_time_set;
static void sntpcb(struct mg_connection *c, int ev, void *evd, void *fnd) {
  if (ev == MG_EV_SNTP_TIME) {
    uint64_t ms = *(uint64_t *) evd;
    struct timeval tv = {.tv_sec = ms / 1000, .tv_usec = (ms % 1000) * 1000};
    MG_INFO(("SNTP: setting time to %lu.%lu", tv.tv_sec, tv.tv_usec));
    settimeofday(&tv, NULL);
    s_time_set = true;
  } else if (ev == MG_EV_CLOSE) {
    s_sntp = NULL;
  }
  (void) c, (void) fnd;
}

static void dev_cb(struct mg_connection *c, int ev, void *evd, void *fnd) {
  if (ev == MG_EV_CONNECT) {
    if (mg_url_is_ssl(s_mDashURL)) {
      struct mg_str host = mg_url_host(s_mDashURL);
      struct mg_tls_opts opts = {.ca = s_pem, .srvname = host};
      mg_tls_init(s_conn, &opts);
    }
  }
  if (ev == MG_EV_WS_OPEN) {
    MG_INFO(("connected to %s", s_mDashURL));
    s_mDashState = MDASH_STATE_CLOUD_CONNECTED;
    // mDashTriggerEvent(s_mDashState, NULL);
  } else if (ev == MG_EV_WS_MSG) {
    struct mg_ws_message *wm = evd;
    struct mg_str s = wm->data;
    while (s.len > 0 && isspace((uint8_t) s.ptr[s.len - 1])) s.len--;
    MG_INFO(("RPC: %.*s", (int) s.len, s.ptr));
    struct mg_iobuf io = {0, 0, 0, 512};
    struct mg_rpc_req r = {.head = &g_rpcs,
                           .rpc = NULL,
                           .pfn = mg_pfn_iobuf,
                           .pfn_data = &io,
                           .req_data = NULL,
                           .frame = s};
    mg_rpc_process(&r);
    if (io.buf != NULL) {
      MG_INFO(("     %s", io.buf));
      mg_ws_send(c, (const char *) io.buf, io.len, WEBSOCKET_OP_TEXT);
    } else {
      MG_VERBOSE(("%s", "RPC unhandled"));
    }
    mg_iobuf_free(&io);
  } else if (ev == MG_EV_CLOSE) {
    s_conn = NULL;
    MG_INFO(("disconnected, ram %lu", mDashGetFreeRam()));
    if (s_mDashState == MDASH_STATE_CLOUD_CONNECTED)
      s_mDashState = MDASH_STATE_NETWORK_CONNECTED;
    // mDashTriggerEvent(s_mDashState, NULL);
  }
  (void) c, (void) fnd;
}

static int sv(struct mg_str json, const char *path, const char *name) {
  bool set = false;
  char *val = mg_json_get_str(json, path);
  if (val != NULL) {
    mDashConfigSet(name, val);
    free(val);
    set = true;
  }
  return set;
}

static void httpcb(struct mg_connection *c, int ev, void *evd, void *fnd) {
  const char *extra_headers =
      "Access-Control-Allow-Origin: *\r\n"
      "Content-Type: application/json\r\n";
  if (ev == MG_EV_HTTP_MSG) {
    struct mg_http_message *hm = evd;
    if (mg_http_match_uri(hm, "/GetKey")) {
      if (s_mDashPublicKey != NULL) {
        mg_http_reply(c, 200, extra_headers, "{%Q:%Q}\n", "result",
                      s_mDashPublicKey);
      } else {
        mg_http_reply(c, 200, extra_headers, "{%Q:%Q}\n", "error",
                      "key not set");
      }
    } else if (mg_http_match_uri(hm, "/setup") && hm->body.len > 0) {
      if (sv(hm->body, "$.ssid", "wifi.sta.ssid") &&
          sv(hm->body, "$.pass", "wifi.sta.pass")) {
        mg_http_reply(c, 200, extra_headers, "{%Q:true}\n", "result");
        reboot_with_delay(3000);
      } else {
        mg_http_reply(c, 200, extra_headers, "{%Q:%Q}\n", "error",
                      "set ssid and pass");
      }
    } else {
      mg_http_reply(c, 200, extra_headers, "{%Q:%Q}\n", "error",
                    "use /setup endpoint");
    }
  }
  (void) fnd;
}

static void timercb(void *userdata) {
  struct mg_mgr *mgr = userdata;
  MG_DEBUG(("ram: %lu, %d %p/%p/%p", mDashGetFreeRam(), s_mDashState, s_conn,
            s_http, s_sntp));
  if (s_conn == NULL && s_mDashState == MDASH_STATE_NETWORK_CONNECTED &&
      s_mDashPassword != NULL && s_time_set) {
    s_conn = mg_ws_connect(mgr, s_mDashURL, dev_cb, NULL,
                           "Origin: %s\r\n"
                           "Authorization: Bearer %s\r\n",
                           s_mDashURL, s_mDashPassword);
  }
  if (s_sntp == NULL && (s_mDashState == MDASH_STATE_NETWORK_CONNECTED ||
                         s_mDashState == MDASH_STATE_CLOUD_CONNECTED)) {
    s_sntp = mg_sntp_connect(mgr, NULL, sntpcb, NULL);
  }
  // Trigger periodic STNP reconnect, and time resync, by closing s_sntp
  if (s_sntp && ((time(0) % 3600) == 0)) s_sntp->is_closing = 1;
  if (s_http == NULL && s_mDashState == MDASH_STATE_NETWORK_AP) {
    s_http = mg_http_listen(mgr, HTTP_URL, httpcb, NULL);
  }
}

void mDashPoll(void) {
  MDashMutexLock();
  mg_mgr_poll(&s_mgr, 50);
  MDashMutexUnlock();
}

void mDashTask(void *data) {
  for (;;) mDashPoll();
  (void) data;
}

int mDashGetState(void) {
  return s_mDashState;
}

static void doinit(const char *id, const char *pass, const char *name,
                   const char *ts, const char *framework) {
  mount_fs(FS_ROOT);  // Do it first, to enable configuration API
  s_start_time = time(NULL);
  s_mDashPassword = fromcfg(pass, "device.pass");
  s_mDashPublicKey = fromcfg(NULL, "device.public_key");
  if (s_mDashPublicKey == NULL) {
    char buf[40];
    s_mDashPublicKey = strdup(gen_public_key(buf, sizeof(buf)));
    mDashConfigSet("device.public_key", s_mDashPublicKey);
  }
  s_mDashBuildTime = ts;
  const char *p = strrchr(name, '/');
  if (p == NULL) p = strrchr(name, '\\');
  if (p != NULL) name = p + 1;
  s_mDashAppName = name;
  s_mDashFramework = framework;
  s_mDashURL = (char *) fromcfg(s_mDashURL, "mdash.url");

  char ll[2] = "2";  // Default log level
  if (s_mDashLogLevel == 2 &&
      mDashConfigGet("debug.level", ll, sizeof(ll)) == 0) {
    MG_INFO(("Setting log level (from config) to: %s", ll));
    s_mDashLogLevel = atoi(ll);
  }
  mg_log_set(s_mDashLogLevel);
  init_rpcs();
  mg_mgr_init(&s_mgr);
  mg_timer_add(&s_mgr, 1000, MG_TIMER_REPEAT | MG_TIMER_RUN_NOW, timercb,
               &s_mgr);
  MG_INFO(("Initialised, id %s", id));
}

void mDashInit(const char *id, const char *pass, const char *name,
               const char *ts, const char *framework) {
  s_mDashState = MDASH_STATE_NETWORK_CONNECTED;
  MDashMutexInit();
  MDashMutexLock();
  doinit(id, pass, name, ts, framework);
  xTaskCreatePinnedToCore(&mDashTask, "mDashTask", 16384, 0, 5, 0, CPU_CORE);
  MDashMutexUnlock();
}
