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
	
