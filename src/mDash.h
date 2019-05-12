// Copyright (c) 2019 Cesanta Software Limited
// All rights reserved

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define mDashStartWithWifi(a, b, c, d) \
  mDashInitWithWifi((a), (b), (c), (d), __FILE__, __DATE__ "-" __TIME__)
#define mDashStart(a, b) mDashInit((a), (b), __FILE__, __DATE__ "-" __TIME__)

enum { MDASH_EV_CONNECTED, MDASH_EV_DISCONNECTED };

// mDash housekeeping
void mDashInitWithWifi(const char *wifi_name, const char *wifi_pass,
                       const char *device_id, const char *device_pass,
                       const char *app_name, const char *build_time);
void mDashInit(const char *, const char *, const char *, const char *);
void mDashOn(void (*fn)(int, void *), void *);
void mDashSetLogLevel(int logLevel);
const char *mDashGetDeviceID(void);

// MQTT API
int mDashPublish(const char *topic, const char *message_fmt, ...);
void mDashSubscribe(const char *topic, void (*fn)(const char *, const char *));

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

#ifdef __cplusplus
}
#endif
