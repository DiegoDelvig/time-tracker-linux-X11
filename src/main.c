#include "ticker.h"
#include "tracker.h"

#include <stdio.h>
#include <stdlib.h>

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
	tracker_t *t = tracker_create();
	int status;
	
	if (t == NULL)
	{
		printf("Erreur création tracker\n");
		return EXIT_FAILURE;
	}
	
	status = ticker_run(10, on_tick, t);
	print_stats(tracker_stats(t));
	tracker_destroy(t);
	
	if (status == -1)
	{
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}
