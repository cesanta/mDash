// This is mdash.net device simulator on POSIX systems.

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "mDash.h"

static int s_stop;

static void sighandler(int sig) {
  MLOG(LL_CRIT, "Got signal %d, exiting...", sig);
  s_stop = 1;
}

// This shadow delta callback sets reported equal to desired, thus
// clearing the delta - whatever it is, without performing any actual action.
static void onDelta(struct jsonrpc_request *r) {
  const char *s = NULL;
  int len = 0;
  mjson_find(r->params, r->params_len, "$.state", &s, &len);
  if (s) mDashShadowUpdate("{%Q:{%Q:%.*s}}", "state", "reported", len, s);
}

int main(int argc, char *argv[]) {
  const char *wifi = "", *pass = NULL, *report_interval = "5";

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--pass") == 0) {
      pass = argv[++i];
    } else if (strcmp(argv[i], "--server") == 0) {
      mDashConfigSet("server.name", argv[++i]);
    } else if (strcmp(argv[i], "--port") == 0) {
      mDashConfigSet("server.port", argv[++i]);
    } else if (strcmp(argv[i], "--log-level") == 0) {
      mDashSetLogLevel(atoi(argv[++i]));
    } else if (strcmp(argv[i], "--report-interval") == 0) {
      report_interval = argv[++i];
    } else if (strcmp(argv[i], "--ap") == 0) {
      wifi = NULL;  // if WiFi is NULL, mDash lib starts HTTP server
    } else {
      MLOG(LL_CRIT, "Invalid option: [%s]\n", argv[i]);
      MLOG(LL_CRIT, "Usage: %s --pass DEVICE_PASSWORD", argv[0]);
      return 1;
    }
  }

  if (pass == NULL) {
    MLOG(LL_CRIT, "%s", "Please specify --pass DEVICE_PASSWORD");
    return 1;
  }

  mDashBeginWithWifi(NULL, wifi, NULL, pass);
  jsonrpc_export("Shadow.Delta", onDelta, NULL);

  srand(time(0));
  sleep(1);
  signal(SIGINT, sighandler);
  signal(SIGTERM, sighandler);
  signal(SIGPIPE, SIG_IGN);

  while (s_stop == 0) {
    mDashNotify("DB.Save", "{%Q:%u}", "ram", mDashGetFreeRam());
    sleep(atoi(report_interval));
  }

  return 0;
}
