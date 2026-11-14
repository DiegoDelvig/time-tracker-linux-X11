#include "paths.h"
#include "render.h"
#include "stats.h"
#include "storage.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define DEFAULT_WIDTH 80u

/* Largeur du terminal en colonnes ; 80 si stdout n'est pas un terminal. */
static unsigned terminal_width(void)
{
	struct winsize ws;

	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
	{
		return DEFAULT_WIDTH;
	}
	return ws.ws_col;
}

int main(void)
{
	char path[512];
	stats_t *s;
	int status = EXIT_SUCCESS;

	if (paths_stats_file(path, sizeof path) == -1)
	{
		fprintf(stderr, "paths: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	s = stats_create();
	if (s == NULL)
	{
		fprintf(stderr, "stats_create: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	if (storage_load(s, path) == -1)
	{
		fprintf(stderr, "lecture de %s: %s\n", path, strerror(errno));
		status = EXIT_FAILURE;
	}
	else if (render_jauges(stdout, s, terminal_width()) == -1)
	{
		fprintf(stderr, "affichage impossible\n");
		status = EXIT_FAILURE;
	}

	stats_destroy(s);
	return status;
}
