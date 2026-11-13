#include "../src/storage.h"
#include "../src/stats.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#define PATH     "/tmp/timetracker_test.dat"
#define TMP_PATH PATH ".tmp"

/* Écrit content tel quel dans PATH (pour fabriquer des fichiers à la main). */
static void write_file(const char *content)
{
	FILE *f = fopen(PATH, "w");

	assert(f != NULL);
	fputs(content, f);
	assert(fclose(f) == 0);
}

static void test_roundtrip(void)
{
	stats_t *a = stats_create();
	stats_t *b = stats_create();

	assert(a != NULL && b != NULL);
	assert(stats_add(a, "firefox", 120) == 0);
	assert(stats_add(a, "bash", 45) == 0);
	assert(stats_add(a, "kate", 7) == 0);

	assert(storage_save(a, PATH) == 0);
	assert(storage_load(b, PATH) == 0);

	assert(stats_count(b) == 3);
	assert(strcmp(stats_name_at(b, 0), "firefox") == 0);
	assert(stats_seconds_at(b, 0) == 120);
	assert(strcmp(stats_name_at(b, 1), "bash") == 0);
	assert(stats_seconds_at(b, 1) == 45);
	assert(strcmp(stats_name_at(b, 2), "kate") == 0);
	assert(stats_seconds_at(b, 2) == 7);

	/* load AJOUTE : recharger le même fichier double les secondes. */
	assert(storage_load(b, PATH) == 0);
	assert(stats_count(b) == 3);
	assert(stats_seconds_at(b, 0) == 240);

	stats_destroy(a);
	stats_destroy(b);
	remove(PATH);
}

static void test_missing_file(void)
{
	stats_t *s = stats_create();

	assert(s != NULL);
	remove(PATH);
	assert(storage_load(s, PATH) == 0);   /* premier lancement : pas une erreur */
	assert(stats_count(s) == 0);

	stats_destroy(s);
}

static void test_malformed_lines(void)
{
	stats_t *s = stats_create();

	assert(s != NULL);
	write_file("5\tkate\n"
	           "garbage sans tabulation\n"
	           "\tpas de nombre\n"
	           "12abc\n"
	           "8\t\n"                     /* nom vide */
	           "\n"                        /* ligne vide */
	           "7\tbrave\n");

	assert(storage_load(s, PATH) == 0);
	assert(stats_count(s) == 2);
	assert(strcmp(stats_name_at(s, 0), "kate") == 0);
	assert(stats_seconds_at(s, 0) == 5);
	assert(strcmp(stats_name_at(s, 1), "brave") == 0);
	assert(stats_seconds_at(s, 1) == 7);

	stats_destroy(s);
	remove(PATH);
}

static void test_no_tmp_left(void)
{
	stats_t *s = stats_create();
	FILE *f;

	assert(s != NULL);
	assert(stats_add(s, "kate", 1) == 0);
	assert(storage_save(s, PATH) == 0);

	f = fopen(TMP_PATH, "r");
	assert(f == NULL);
	assert(errno == ENOENT);

	stats_destroy(s);
	remove(PATH);
}

static void test_save_overwrites(void)
{
	stats_t *s = stats_create();
	stats_t *r = stats_create();

	assert(s != NULL && r != NULL);
	write_file("999\tancien\n");

	assert(stats_add(s, "nouveau", 3) == 0);
	assert(storage_save(s, PATH) == 0);
	assert(storage_load(r, PATH) == 0);
	assert(stats_count(r) == 1);
	assert(strcmp(stats_name_at(r, 0), "nouveau") == 0);

	stats_destroy(s);
	stats_destroy(r);
	remove(PATH);
}

static void test_errors(void)
{
	stats_t *s = stats_create();

	assert(s != NULL);

	errno = 0;
	assert(storage_save(NULL, PATH) == -1);
	assert(errno == EINVAL);
	assert(storage_save(s, NULL) == -1);
	assert(storage_load(NULL, PATH) == -1);
	assert(storage_load(s, NULL) == -1);

	/* Dossier inexistant : l'écriture doit échouer proprement. */
	assert(storage_save(s, "/dossier_inexistant/x.dat") == -1);

	stats_destroy(s);
}

int main(void)
{
	test_roundtrip();
	test_missing_file();
	test_malformed_lines();
	test_no_tmp_left();
	test_save_overwrites();
	test_errors();

	puts("storage : OK");
	return 0;
}
