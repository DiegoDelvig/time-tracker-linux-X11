#include "ticker.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

static void print_tick(unsigned n, void *userdata)
{
	(void)userdata;
	printf("TICK %u\n", n);
}

int main(void)
{
	printf("%ld\n", (long)(getpid()));

	if (ticker_run(5, print_tick, NULL) == -1)
	{
		return EXIT_FAILURE;
	}

	return 0;
}

