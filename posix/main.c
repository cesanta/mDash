// Copyright (c) 2019-2022 Cesanta Software Limited
// All rights reserved
//
// This is mdash.net device simulator on POSIX systems.

#include "mDash.h"

static int s_sig;
static void sighandler(int sig) {
  s_sig = sig;
}

int main(int argc, char *argv[]) {
  const char *pass = NULL;  // *url = NULL, *report_interval = "5";
  int loglevel = -1;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-pass") == 0) {
      pass = argv[++i];
    } else if (strcmp(argv[i], "-v") == 0) {
      loglevel = atoi(argv[++i]);
    } else if (strcmp(argv[i], "-ap") == 0) {
      // wifi = NULL;  // if WiFi is NULL, mDash lib starts HTTP server
    } else {
      MG_ERROR(("Invalid option: [%s]\n", argv[i]));
      MG_ERROR(("Usage: %s --pass MDASH_DEVICE_KEY", argv[0]));
      return 1;
    }
  }

  if (pass == NULL || pass[0] == '\0') {
    MG_ERROR(("%s", "Please specify --pass MDASH_DEVICE_KEY"));
    return 1;
  }

  signal(SIGINT, sighandler);
  signal(SIGTERM, sighandler);
  signal(SIGPIPE, SIG_IGN);

  // mg_boot_timestamp_ms = time(0) * 1000;
  mDashBegin(pass);
  if (loglevel > 0) mg_log_set(loglevel);
  while (s_sig == 0) mDashPoll();
  MG_INFO(("Got signal %d, exiting...", s_sig));

  return 0;
}
