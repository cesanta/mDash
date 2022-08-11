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

#pragma once

#include "mongoose.h"

#define MDASH_VERSION "1.2.16"

#ifdef __cplusplus
extern "C" {
#endif

// Get current connection state
int mDashGetState(void);
#define MDASH_STATE_NETWORK_LOST 0
#define MDASH_STATE_NETWORK_AP 1
#define MDASH_STATE_NETWORK_CONNECTED 2
#define MDASH_STATE_CLOUD_CONNECTED 3

// Store data
int mDashStore(const char *topic, const char *json_fmt, ...);

// Send notification frame to mDash
int mDashNotify(const char *name, const char *fmt, ...);
#define mDashShadowUpdate(fmt, ...) \
  mDashNotify("Dash.Shadow.Update", (fmt), __VA_ARGS__)

int mDashConfigGet(const char *name, char *buf, int bufsize);
int mDashConfigSet(const char *name, const char *value);
int mDashConfigReset(void);
void mDashPoll(void);

// Registered RPC handlers. Use mg_rpc_add() to add new handlers
extern struct mg_rpc *g_rpcs;

void mDashInit(const char *device_id, const char *device_pass,
               const char *app_name, const char *build_time,
               const char *framework);
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
#define mDashBegin(b) \
  mDashInit(NULL, (b), MDASH_APP_NAME, MDASH_BUILD_TIME, MDASH_FRAMEWORK)

#ifdef __cplusplus
}
#endif
