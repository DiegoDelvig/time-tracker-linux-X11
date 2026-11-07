#include "tracker.h"

#include "proc_info.h"
#include "stats.h"
#include "x11_probe.h"
#include "proc_info.h"

#include <stdlib.h>

struct tracker
{
    x11_probe_t *probe;
    stats_t *stats;
};

tracker_t *tracker_create(void)
{
    tracker_t *t;
    
    t = calloc(1, sizeof *t);
    if (t == NULL)
    {
        return NULL;
    }
    
    t->probe = x11_probe_create();
    if (t->probe == NULL)
    {
        free(t);
        return NULL;
    }
    
    t->stats = stats_create();
    if (t->stats == NULL)
    {
        x11_probe_destroy(t->probe);
        free(t);
        return NULL;
    }
    
    return t;
}

void tracker_destroy(tracker_t *t)
{
    if (t == NULL)
    {
        return;
    }
    
    stats_destroy(t->stats);
    x11_probe_destroy(t->probe);
    free(t);
}

int tracker_tick(tracker_t *t)
{
    pid_t pid;
    char name[64];
    int status;
    
    if (t == NULL)
    {
        return -1;
    }
    
    status = x11_probe_active_pid(t->probe, &pid);
    
    if (status == 1)
    {
        return 0;
    }
    
    if (status == -1)
    {
        return -1;
    }
    
    if (status != 0)
    {
        return (status == 1) ? 0 : -1;
    }
    
    if (proc_info_get_name(pid, name, sizeof name) == 1)
    {
        return 0;
    }
    
    return stats_add(t->stats, name, 1);
}

const stats_t *tracker_stats(const tracker_t *t)
{    
    if (t == NULL)
    {
        return NULL;
    }
    
    return t->stats;
}
