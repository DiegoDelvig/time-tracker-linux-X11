#include "ticker.h"
#include <unistd.h>

int ticker_run(unsigned count, ticker_cb on_tick, void *userdata)
{
	if (on_tick == NULL)
	{
		return -1;
	}

	for (unsigned int i = 1; i <= count; i++)
	{
		on_tick(i, userdata);
		sleep(1); // sleep APRES le comeback
	}
	return 0;
}

int ticker_run_until(const volatile sig_atomic_t *stop, ticker_cb on_tick, void *userdata)
{
	unsigned n = 1;

 	if (stop == NULL || on_tick == NULL)
	{
		return -1;
	}

	while (!*stop)
	{
		on_tick(n, userdata);
		n++;
		sleep(1);
	}

	return 0;
}
	
