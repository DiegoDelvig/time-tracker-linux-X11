#include "render.h"

#include <stdlib.h>
#include <string.h>

#define MIN_WIDTH  40
#define MIN_BAR    10
#define NAME_COL   16   /* noms de /proc/<pid>/comm : 15 caractères max */
#define DUR_COL    8
/* Colonnes hors jauge : nom + espace + 2 espaces + durée + espace + "100%" */
#define FIXED_COLS (NAME_COL + 1 + 2 + DUR_COL + 1 + 4)
#define MAX_BAR 50

typedef struct
{
	const char *name;        /* appartient à stats : on ne copie pas */
	unsigned long seconds;
} row_t;

/* Comparateur pour qsort : durée décroissante, puis nom (ordre stable). */
static int cmp_rows(const void *a, const void *b)
{
	const row_t *x = a;
	const row_t *y = b;

	if (x->seconds > y->seconds)
	{
		return -1;
	}
	if (x->seconds < y->seconds)
	{
		return 1;
	}
	return strcmp(x->name, y->name);
}

int render_duration(unsigned long seconds, char *buf, size_t size)
{
	int n;

	if (buf == NULL || size == 0)
	{
		return -1;
	}

	if (seconds < 60)
	{
		n = snprintf(buf, size, "%lus", seconds);
	}
	else if (seconds < 3600)
	{
		n = snprintf(buf, size, "%lum %02lus", seconds / 60, seconds % 60);
	}
	else
	{
		n = snprintf(buf, size, "%luh %02lum", seconds / 3600,
		             (seconds % 3600) / 60);
	}

	if (n < 0 || (size_t)n >= size)
	{
		return -1;
	}
	return 0;
}

int render_jauges(FILE *out, const stats_t *s, unsigned width)
{
	row_t *rows;
	size_t n;
	size_t i;
	size_t k;
	unsigned long total = 0;
	unsigned long max;
	unsigned bar;
	int status;

	if (out == NULL || s == NULL)
	{
		return -1;
	}

	n = stats_count(s);
	if (n == 0)
	{
		fputs("Aucune donnee.\n", out);
		return ferror(out) ? -1 : 0;
	}

	rows = malloc(n * sizeof *rows);
	if (rows == NULL)
	{
		return -1;
	}

	for (i = 0; i < n; i++)
	{
		rows[i].name = stats_name_at(s, i);
		rows[i].seconds = stats_seconds_at(s, i);
		total += rows[i].seconds;
	}
	qsort(rows, n, sizeof *rows, cmp_rows);
	max = rows[0].seconds;

	/* width est unsigned : on borne AVANT de soustraire (pas de débordement). */
	if (width < MIN_WIDTH)
	{
		width = MIN_WIDTH;
	}
	bar = width - FIXED_COLS;
	if (bar < MIN_BAR)
	{
		bar = MIN_BAR;
	}

    if (bar > MAX_BAR)
    {
        bar = MAX_BAR;
    }

	for (i = 0; i < n; i++)
	{
		char dur[32];
		unsigned long filled = 0;
		unsigned long pct = 0;

		if (max > 0)
		{
			filled = rows[i].seconds * bar / max;
		}
		if (total > 0)
		{
			pct = rows[i].seconds * 100 / total;
		}
		if (render_duration(rows[i].seconds, dur, sizeof dur) == -1)
		{
			dur[0] = '\0';
		}

		fprintf(out, "%-*s ", NAME_COL, rows[i].name);
		for (k = 0; k < bar; k++)
		{
			fputs((k < filled) ? "\xE2\x96\x88" : "\xE2\x96\x91", out); /* █ ░ */
		}
		fprintf(out, "  %*s %3lu%%\n", DUR_COL, dur, pct);
	}

	status = ferror(out) ? -1 : 0;
	free(rows);
	return status;
}
