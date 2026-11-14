#include "../src/paths.h"

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

int main(void)
{
    char path[512];
    char tiny[8];
    struct stat st;

    // Via HOME
    system("rm -rf /tmp/fakehome");
    assert(mkdir("/tmp/fakehome", 0700) == 0);
    setenv("XDG_DATA_HOME", "", 1);
    setenv("HOME", "/tmp/fakehome", 1);
    assert(paths_stats_file(path, sizeof path) == 0);
    assert(strcmp(path, "/tmp/fakehome/.local/share/timetracker/stats.dat") == 0);
    assert(stat("/tmp/fakehome/.local/share/timetracker", &st) == 0);
    assert(S_ISDIR(st.st_mode));

    // dossier déjà là
    assert(paths_stats_file(path, sizeof path) == 0);

    // XDG_DATA_HOME prioritaire
    setenv("XDG_DATA_HOME", "/tmp/fakexdg", 1);
    assert(paths_stats_file(path, sizeof path) == 0);
    assert(strcmp(path, "/tmp/fakexdg/timetracker/stats.dat") == 0);

    // erreurs
    assert(paths_stats_file(NULL, 10) == -1 && errno == EINVAL);
    assert(paths_stats_file(path, 0) == -1 && errno == EINVAL);
    assert(paths_stats_file(tiny, sizeof tiny) == -1 && errno == ENAMETOOLONG);
    setenv("XDG_DATA_HOME", "", 1);
    unsetenv("HOME");
    assert(paths_stats_file(path, sizeof path) == -1 && errno == EINVAL);

    system("rm -rf /tmp/fakehome /tmp/fakexdg");
    puts("paths : OK");
    return 0;
}
