// Copyright (c) 2019 Cesanta Software Limited
// All rights reserved

#pragma once

struct mDash {
  void (*begin)(const char *id, const char *password);
  void (*publish)(const char *topic, const char *message);
  void (*subscribe)(const char *topic);
  void (*set_log_level)(int log_level);
};
extern struct mDash mDash;
