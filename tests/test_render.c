#include "render.h"
#include "stats.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Rend s dans une chaîne malloc'ée (à libérer) ; NULL si échec. */
static char *render_to_string(const stats_t *s, unsigned width, int *ret)
{
	char *out = NULL;
	size_t len = 0;
	FILE *f = open_memstream(&out, &len);

	assert(f != NULL);
	*ret = render_jauges(f, s, width);
	assert(fclose(f) == 0);   /* indispensable avant de lire out */
	return out;
}

static size_t count_lines(const char *s)
{
	size_t n = 0;

	for (; *s != '\0'; s++)
	{
		if (*s == '\n')
		{
			n++;
		}
	}
	return n;
}

static void test_duration(void)
{
	char buf[32];
	char tiny[3];

	assert(render_duration(0, buf, sizeof buf) == 0 && strcmp(buf, "0s") == 0);
	assert(render_duration(59, buf, sizeof buf) == 0 && strcmp(buf, "59s") == 0);
	assert(render_duration(60, buf, sizeof buf) == 0 && strcmp(buf, "1m 00s") == 0);
	assert(render_duration(185, buf, sizeof buf) == 0 && strcmp(buf, "3m 05s") == 0);
	assert(render_duration(3599, buf, sizeof buf) == 0 && strcmp(buf, "59m 59s") == 0);
	assert(render_duration(3600, buf, sizeof buf) == 0 && strcmp(buf, "1h 00m") == 0);
	assert(render_duration(3665, buf, sizeof buf) == 0 && strcmp(buf, "1h 01m") == 0);

	assert(render_duration(5, NULL, 10) == -1);
	assert(render_duration(5, buf, 0) == -1);
	assert(render_duration(3665, tiny, sizeof tiny) == -1);
}

static void test_empty(void)
{
	stats_t *s = stats_create();
	int ret;
	char *out;

	assert(s != NULL);
	out = render_to_string(s, 80, &ret);
	assert(ret == 0);
	assert(strstr(out, "Aucune donnee.") != NULL);

	free(out);
	stats_destroy(s);
}

static void test_sorted_and_content(void)
{
	stats_t *s = stats_create();
	int ret;
	char *out;
	char *p_big;
	char *p_mid;
	char *p_small;

	assert(s != NULL);
	assert(stats_add(s, "petit", 10) == 0);
	assert(stats_add(s, "gros", 3600) == 0);
	assert(stats_add(s, "moyen", 600) == 0);

	out = render_to_string(s, 80, &ret);
	assert(ret == 0);
	assert(count_lines(out) == 3);

	p_big = strstr(out, "gros");
	p_mid = strstr(out, "moyen");
	p_small = strstr(out, "petit");
	assert(p_big != NULL && p_mid != NULL && p_small != NULL);
	assert(p_big < p_mid && p_mid < p_small);       /* durée décroissante */

	assert(strstr(out, "1h 00m") != NULL);
	assert(strstr(out, "10m 00s") != NULL);
	assert(strstr(out, "\xE2\x96\x88") != NULL);     /* au moins un █ */
	assert(strstr(out, "%") != NULL);

	free(out);
	stats_destroy(s);
}

static void test_widths_and_zero(void)
{
	stats_t *s = stats_create();
	int ret;
	char *out;
	unsigned w;

	assert(s != NULL);
	assert(stats_add(s, "a", 5) == 0);
	assert(stats_add(s, "b", 0) == 0);              /* durée nulle */

	/* Largeurs extrêmes : ne doivent ni planter ni déborder. */
	for (w = 0; w < 300; w += 7)
	{
		out = render_to_string(s, w, &ret);
		assert(ret == 0);
		assert(count_lines(out) == 2);
		free(out);
	}

	stats_destroy(s);

	/* Total nul : pas de division par zéro. */
	s = stats_create();
	assert(s != NULL);
	assert(stats_add(s, "zero", 0) == 0);
	out = render_to_string(s, 80, &ret);
	assert(ret == 0);
	assert(strstr(out, "0%") != NULL);
	free(out);
	stats_destroy(s);
}

static void test_errors(void)
{
	stats_t *s = stats_create();

	assert(s != NULL);
	assert(render_jauges(NULL, s, 80) == -1);
	assert(render_jauges(stdout, NULL, 80) == -1);
	stats_destroy(s);
}

int main(void)
{
	test_duration();
	test_empty();
	test_sorted_and_content();
	test_widths_and_zero();
	test_errors();

	puts("render : OK");
	return 0;
}
