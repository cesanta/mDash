// Copyright (c) 2019 Cesanta Software Limited
// All rights reserved

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define mDashInit(x, y) mDashInitEx((x), (y), __FILE__)

void mDashInitEx(const char *id, const char *password, const char *name);
void mDashSetLogLevel(int logLevel);
void mDashPublish(const char *topic, const char *message);
void mDashSubscribe(const char *topic);

#ifdef __cplusplus
}
#endif
