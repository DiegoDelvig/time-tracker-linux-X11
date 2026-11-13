#include "proc_info.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

int proc_info_get_name(pid_t pid, char *buf, size_t size)
{
	char path[64];
	FILE *f;

    if (buf == NULL || size == 0)
	{
		errno = EINVAL;
		return -1;
	}
	
	int n = snprintf(path, sizeof path, "/proc/%ld/comm", (long)pid);
	
	if (n < 0 || (size_t)n >= sizeof(path))
	{
		errno = ENAMETOOLONG;
		return -1;
	}
	
	f = fopen(path, "r");

    if (f == NULL)
	{
		return -1;
	}
	
	if (fgets(buf, size, f) == NULL)
	{
		int saved = errno;
		fclose(f);
		errno = saved;
		return -1;
	}
		
	buf[strcspn(buf, "\n")] = '\0';
	
	fclose(f);
	return 0;
}
