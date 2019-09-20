// Copyright (c) 2019 Cesanta Software Limited
// All rights reserved

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#if ARDUINO
#define __TOSTR(x) #x
#define STR(x) __TOSTR(x)
#ifndef ARDUINO_BOARD
#define ARDUINO_BOARD "?"
#endif
#define MDASH_FRAMEWORK "a-" STR(ARDUINO) "-" ARDUINO_BOARD
#else
#define MDASH_FRAMEWORK "idf"
#endif

#ifndef MDASH_APP_NAME
#define MDASH_APP_NAME __FILE__
#endif

#define MDASH_BUILD_TIME __DATE__ "-" __TIME__

#define mDashStartWithWifi(a, b, c, d)                                    \
  mDashInitWithWifi((a), (b), (c), (d), MDASH_APP_NAME, MDASH_BUILD_TIME, \
                    MDASH_FRAMEWORK)

#define mDashStart(a, b) \
  mDashInit((a), (b), MDASH_APP_NAME, MDASH_BUILD_TIME, MDASH_FRAMEWORK)

#define mDashBegin()                                                          \
  mDashInitWithWifi(NULL, NULL, NULL, NULL, MDASH_APP_NAME, MDASH_BUILD_TIME, \
                    MDASH_FRAMEWORK)

// mDash states
enum mDashConnState { MDASH_NO_IP, MDASH_AP_IP, MDASH_STA_IP, MDASH_CONNECTED };

// mDash housekeeping
void mDashInitWithWifi(const char *wifi_name, const char *wifi_pass,
                       const char *device_id, const char *device_pass,
                       const char *app_name, const char *build_time,
                       const char *framework);
void mDashInit(const char *device_id, const char *device_pass,
               const char *app_name, const char *build_time,
               const char *framework);

int mDashGetState(void);
void mDashSetLogLevel(int logLevel);
void mDashSetServer(const char *, int);
const char *mDashGetDeviceID(void);
const char *mDashGetSdkVersion(void);
unsigned long mDashGetFreeRam(void);
struct mjson_out;
int mjson_printf(struct mjson_out *, const char *, ...);

// Events
enum {
  MDASH_EVENT_CONN_STATE = 0,  // event_data: enum mDashConnState
  MDASH_EVENT_USER = 100,      // Starting number for user-based events
};
typedef void (*evh_t)(void *event_data, void *userdata);
void mDashRegisterEventHandler(int event, evh_t fn, void *userdata);
int mDashTriggerEvent(int event, void *event_data);

// Logging API
enum { LL_NONE, LL_CRIT, LL_INFO, LL_DEBUG };
#define MLOG(ll, fmt, ...) mlog(ll, __func__, (fmt), __VA_ARGS__)
void mlog(int ll, const char *fn, const char *fmt, ...);

// Provisioning API
void mDashCLI(unsigned char input_byte);

// Notification API
int mDashNotify(const char *name, const char *fmt, ...);
#define mDashShadowUpdate(fmt, ...) \
  mDashNotify("Dash.Shadow.Update", (fmt), __VA_ARGS__)
#define mDashSave(fmt, ...) mDashNotify("Store", (fmt), __VA_ARGS__)

// RPC API
void mDashExport(const char *name, void (*cb)(void *, void *), void *cbdata);
void mDashReturnSuccess(void *ctx, const char *json_fmt, ...);
void mDashReturnError(void *ctx, const char *error_message);
const char *mDashGetParams(void *ctx);

// JSON API
int mDashGetNum(const char *json, const char *json_path, double *value);
int mDashGetStr(const char *json, const char *json_path, char *dst, int len);
int mDashGetBase64(const char *json, const char *json_path, char *dst, int len);
int mDashGetBool(const char *json, const char *json_path, int *);
int mDashGetTok(const char *json, const char *json_path, const char **, int *);
char *mDashToJson(const char *json_fmt, ...);

// Configuration API
int mDashConfigGet(const char *name, char *buf, int bufsize);
int mDashConfigSet(const char *name, const char *value);
void mDashCLI(unsigned char input_byte);

#ifdef __cplusplus
}
#endif
