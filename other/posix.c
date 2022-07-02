// This is mdash.net device simulator on POSIX systems.

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "mongoose.h"

#include "mDash.h"
#include "mjson.h"

static int s_stop;

static void sighandler(int sig) {
  MG_ERROR(("Got signal %d, exiting...", sig));
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

static void onConfigGet(struct jsonrpc_request *r) {
  jsonrpc_return_success(r, "%s", "{}");
}

static void onJsEval(struct jsonrpc_request *r) {
  jsonrpc_return_success(r, "%s", "{}");
}

int main(int argc, char *argv[]) {
  const char *wifi = "", *pass = NULL, *url = NULL, *report_interval = "5";

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--pass") == 0) {
      pass = argv[++i];
    } else if (strcmp(argv[i], "--url") == 0) {
      url = argv[++i];
    } else if (strcmp(argv[i], "--log-level") == 0) {
      mDashSetLogLevel(strdup(argv[++i]));
    } else if (strcmp(argv[i], "--report-interval") == 0) {
      report_interval = argv[++i];
    } else if (strcmp(argv[i], "--ap") == 0) {
      wifi = NULL;  // if WiFi is NULL, mDash lib starts HTTP server
    } else {
      MG_ERROR(("Invalid option: [%s]\n", argv[i]));
      MG_ERROR(("Usage: %s --pass DEVICE_PASSWORD", argv[0]));
      return 1;
    }
  }

  if (pass == NULL) {
    MG_ERROR(("%s", "Please specify --pass DEVICE_PASSWORD"));
    return 1;
  }

  mDashBeginWithWifi(NULL, wifi, NULL, pass);
  jsonrpc_export("Shadow.Delta", onDelta );
  jsonrpc_export("Config.Get", onConfigGet );
  jsonrpc_export("JS.Eval", onJsEval);
  if (url != NULL) mDashSetURL(url);

  srand(time(0));
  sleep(1);
  signal(SIGINT, sighandler);
  signal(SIGTERM, sighandler);
  signal(SIGPIPE, SIG_IGN);

  while (s_stop == 0) {
    char now[40];
    time_t t = time(NULL);
    strftime(now, sizeof(now), "%Y-%m-%d %H:%M:%S", gmtime(&t));

    // Store the same metric (free ram) using two methods - immediate
    // notification, and pipelining via flash.
    mDashNotify("DB.Store", "[%Q,%Q,%Q,%u]", "query1", now, "ram1",
                mDashGetFreeRam());
    mDashStore("query1", "[%Q,%Q,%u]", now, "ram2", mDashGetFreeRam());
    mDashShadowUpdate("{%Q:{%Q:{%Q:%Q}}}", "state", "reported", "now", now);

    sleep(atoi(report_interval));
  }

  return 0;
}
