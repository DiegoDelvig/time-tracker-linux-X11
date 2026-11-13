#include "../src/stats.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
	stats_t *s = stats_create();
	char name[32];
	size_t i;

	assert(s != NULL);
	assert(stats_add(s, "firefox", 5) == 0);
	assert(stats_add(s, "bash", 2) == 0);
	assert(stats_add(s, "firefox", 3) == 0);
	assert(stats_count(s) == 2);
	assert(strcmp(stats_name_at(s, 0), "firefox") == 0);
	assert(stats_seconds_at(s, 0) == 8);

	assert(stats_name_at(s, 99) == NULL);
	assert(stats_seconds_at(s, 99) == 0);
	assert(stats_add(NULL, "x", 1) == -1);
	assert(stats_add(s, NULL, 1) == -1);

	for (i = 0; i < 1000; i++)
	{
		snprintf(name, sizeof name, "app%zu", i);
		assert(stats_add(s, name, 1) == 0);
	}
	assert(stats_count(s) == 1002);

	stats_destroy(s);
	puts("stats : OK");
	return 0;
}
