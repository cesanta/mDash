// Copyright (c) 2019 Cesanta Software Limited
// All rights reserved

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define mDashInit(x, y) mDashInitEx((x), (y), __FILE__)
typedef void (*mDashRpcCallback_t)(void *ctx, void *udata);

void mDashInitEx(const char *id, const char *password, const char *name);
void mDashPublish(const char *topic, const char *message);
void mDashSubscribe(const char *topic);
void mDashExport(const char *name, mDashRpcCallback_t cb, void *cbdata);
void mDashReturnSuccess(void *ctx, const char *json_fmt, ...);
void mDashReturnError(void *ctx, const char *error_message);
double mDashGetDouble(void *ctx, const char *json_path, double default_val);
int mDashGetString(void *ctx, const char *json_path, char *to, int len);
int mDashGetBase64(void *ctx, const char *json_path, char *to, int len);
int mDashGetBool(void *ctx, const char *json_path, int default_val);
void mDashSetLogLevel(int logLevel);

#ifdef __cplusplus
}
#endif
