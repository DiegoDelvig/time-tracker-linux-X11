#define _POSIX_C_SOURCE 200809L

#include "ticker.h"
#include "tracker.h"

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STATS_PATH "/tmp/stats.dat"

static volatile sig_atomic_t g_stop = 0;

static void on_signal(int sig) {
  (void)sig;
  g_stop = 1;
}

static int install_handlers(void) 
{
  struct sigaction sa;

  memset(&sa, 0, sizeof sa);
  sa.sa_handler = on_signal;
  sigemptyset(&sa.sa_mask);

  if (sigaction(SIGINT, &sa, NULL) == -1 ||
      sigaction(SIGTERM, &sa, NULL) == -1) 
  {
    return -1;
  }

  return 0;
}

static void on_tick(unsigned n, void *userdata) 
{
  tracker_t *t = userdata;
  int status;

  (void)n;

  status = tracker_tick(t);

  if (status == -1) 
  {
    fprintf(stderr, "erreur");
  }
}

static void print_stats(const stats_t *s) 
{
  size_t i;

  for (i = 0; i < stats_count(s); i++) 
  {
    const char *sna = stats_name_at(s, i);
    unsigned long ssa = stats_seconds_at(s, i);

    printf("nom : %s -> %lu\n", sna, ssa);
  }
}

int main(void) 
{
    tracker_t *t;
    int status;

    if (install_handlers() == -1) 
    {
        fprintf(stderr, "install_handler");
        return EXIT_FAILURE;
    }

    t = tracker_create();
    if (t == NULL)
    {
        fprintf(stderr, "creation trakcer\n");
        return EXIT_FAILURE;
    }

    if (tracker_load(t, STATS_PATH) == -1)
    {
        fprintf(stderr, "tracker_load %s\n", strerror(errno));
        tracker_destroy(t);
        return EXIT_FAILURE;
    }

    status = ticker_run_until(&g_stop, on_tick, t);

    if (tracker_save(t, STATS_PATH) == -1) 
    {
      strerror(errno);
      status = -1;
    }

    print_stats(tracker_stats(t));
    tracker_destroy(t);
    return (status == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
