#define _POSIX_C_SOURCE 200809L

#include "ticker.h"
#include "tracker.h"
#include "paths.h"

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SAVE_INTERVAL 60u

typedef struct {
  tracker_t *t;
  const char *path;
} app_t;

static volatile sig_atomic_t g_stop = 0;

static void on_signal(int sig) {
  (void)sig;
  g_stop = 1;
}

static int install_handlers(void) {
  struct sigaction sa;

  memset(&sa, 0, sizeof sa);
  sa.sa_handler = on_signal;
  sigemptyset(&sa.sa_mask);

  if (sigaction(SIGINT, &sa, NULL) == -1 ||
      sigaction(SIGTERM, &sa, NULL) == -1) {
    return -1;
  }

  return 0;
}

static void on_tick(unsigned n, void *userdata) {
  app_t *app = userdata;

  if (tracker_tick(app->t) == -1) {
    fprintf(stderr, "tracker_tick %s\n", strerror(errno));
  }

  if (n % SAVE_INTERVAL == 0) {
    if (tracker_save(app->t, app->path) == -1) {
      fprintf(stderr, "tracker_save %s\n", strerror(errno));
    }
  }
}

static void print_stats(const stats_t *s) {
  size_t i;

  for (i = 0; i < stats_count(s); i++) {
    const char *sna = stats_name_at(s, i);
    unsigned long ssa = stats_seconds_at(s, i);

    printf("nom : %s -> %lu\n", sna, ssa);
  }
}

int main(void) {
    tracker_t *t;
    app_t app;
    char path[512];
    int status;

    if (paths_stats_file(path, sizeof path) == -1) {
        fprintf(stderr, "paths: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    if (install_handlers() == -1) {
        fprintf(stderr, "install_handler");
        return EXIT_FAILURE;
    }

    t = tracker_create();
    if (t == NULL) {
        fprintf(stderr, "creation trakcer\n");
        return EXIT_FAILURE;
    }

    if (tracker_load(t, path) == -1) {
        fprintf(stderr, "tracker_load %s\n", strerror(errno));
        tracker_destroy(t);
        return EXIT_FAILURE;
    }

    app.t = t;
    app.path = path;

    status = ticker_run_until(&g_stop, on_tick, &app);

    if (tracker_save(t, path) == -1) {
        fprintf(stderr, "tracker_save %s\n", strerror(errno));
        status = -1;
    }

    print_stats(tracker_stats(t));
    tracker_destroy(t);
    return (status == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
