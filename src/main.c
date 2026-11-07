#include "ticker.h"
#include "tracker.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STATS_PATH "/tmp/stats.dat"

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
	int status;
	tracker_t *t = tracker_create();

	if (t == NULL)
	{
		fprintf(stderr, "creation tracker");
		return EXIT_FAILURE;
	}
	
	if (tracker_load(t, STATS_PATH) == -1)
	{
		fprintf(stderr, "tracker_load");
		tracker_destroy(t);
		return EXIT_FAILURE;
	}

	status = ticker_run(10, on_tick, t);

	if (tracker_save(t, STATS_PATH) == -1)
	{
		fprintf(stderr, "tracker_save");
		status = -1;
	}
	
	print_stats(tracker_stats(t));

	tracker_destroy(t);
	
	return (status == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
