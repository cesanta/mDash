// Copyright (c) 2019 Cesanta Software Limited
// All rights reserved

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define mDashStartWithWifi(a, b, c, d) \
  mDashInitWithWifi((a), (b), (c), (d), __FILE__)
#define mDashStart(a, b) mDashInit((a), (b), __FILE__)

void mDashInitWithWifi(const char *wifi_name, const char *wifi_pass,
                       const char *device_id, const char *device_pass,
                       const char *app_name);
void mDashInit(const char *id, const char *pass, const char *app_name);
bool mDashPublish(const char *topic, const char *message);
void mDashSubscribe(const char *topic);
void mDashExport(const char *name, void (*cb)(void *, void *), void *cbdata);
void mDashReturnSuccess(void *ctx, const char *json_fmt, ...);
void mDashReturnError(void *ctx, const char *error_message);
double mDashGetDouble(void *ctx, const char *json_path, double default_val);
int mDashGetString(void *ctx, const char *json_path, char *to, int len);
int mDashGetBase64(void *ctx, const char *json_path, char *to, int len);
int mDashGetBool(void *ctx, const char *json_path, int default_val);
void mDashSetLogLevel(int logLevel);  // 0 - no logs, 3 - debug logs
const char *mDashGetDeviceID(void);

#ifdef __cplusplus
}
#endif
