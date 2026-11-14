#include "paths.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#define DIR_MAX 512

/* Cree dir s'il n'existe pas. Retourne 0 si OK -1 KO.*/
static int ensure_dir(const char *dir)
{
    

    if (mkdir(dir, 0700) == 0)
    {
        return 0;
    }

    if (errno == EEXIST)
    {
        return 0;
    }
    return -1;
}

/* Ajoute "/name" à la fin de path, puis cree le dossier */
static int append_dir(char *path, size_t size, const char *name)
{
    size_t len = strlen(path);
    int n;

    n = snprintf(path + len, size - len, "/%s", name);

    if (n < 0 || (size_t)n >= size - len)
    {
        errno = ENAMETOOLONG;
        return -1;
    }
    return ensure_dir(path);
}

int paths_stats_file(char *buf, size_t size)
{
    char dir[DIR_MAX];
    const char *xdg;
    const char *home;
    int n;

    if (buf == NULL || size == 0)
    {
        errno = EINVAL;
        return -1;
    }

    xdg = getenv("XDG_DATA_HOME");
    if (xdg != NULL && xdg[0] != '\0')
    {
        n = snprintf(dir, sizeof dir, "%s", xdg);
        if (n < 0 || (size_t)n >= sizeof dir)
        {
            errno = ENAMETOOLONG;
            return -1;
        }
        if (ensure_dir(dir) == -1)
        {
            return -1;
        }
    }
    else
    {
        home = getenv("HOME");
        if (home == NULL || home[0] == '\0')
        {
            errno = EINVAL;
            return -1;
        }

        n = snprintf(dir, sizeof dir, "%s", home);
        if (n < 0 || (size_t)n >= sizeof dir)
        {
            errno = ENAMETOOLONG;
            return -1;
        }
        if (append_dir(dir, sizeof dir, ".local") == -1
                || append_dir(dir, sizeof dir, "share") == -1)
        {
            return -1;
        }
    }

    if (append_dir(dir, sizeof dir, "timetracker") == -1)
    {
        return -1;
    }

    n = snprintf(buf, size, "%s/stats.dat", dir);
    if (n < 0 || (size_t)n >= size)
    {
        errno = ENAMETOOLONG;
        return -1;
    }

    return 0;
}
