#include "storage.h"

#include "stats.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

int storage_save(const stats_t *s, const char *path)
{
    char tmp_path[512];
    FILE *f;
    size_t i;
    
    if (s == NULL || path == NULL)
    {
        errno = EINVAL;
        return -1;
    }
    
    int n = snprintf(tmp_path, sizeof tmp_path, "%s.tmp", path);
    
    if (n < 0 || (size_t)n >= sizeof tmp_path)
    {
        errno = ENAMETOOLONG;
        return -1;
    }
    
    f = fopen(tmp_path, "w");
    if (f == NULL)
    {
        return -1;
    }
    
    for (i = 0; i < stats_count(s); i++)
    {
        if (fprintf(f, "%lu\t%s\n", stats_seconds_at(s, i), stats_name_at(s, i)) < 0)
        {
            int saved_errno = errno;
            fclose(f);
            remove(tmp_path);
            errno = saved_errno;
            return -1;
        }
    }
    
    if (fclose(f) != 0)
    {
        remove(tmp_path);
        return -1;
    }
    
    if (rename(tmp_path, path) == -1)
    {
        remove(tmp_path);
        return -1;
    }
    
    return 0; // OK
}
    
    
int storage_load(stats_t *s, const char *path)
{
    FILE *f;
    char line[256];
    char *end;
    unsigned long secondes;
    char *nom;
    
    if (s == NULL || path == NULL)
    {
        errno = EINVAL;
        return -1;
    }
    
    f = fopen(path, "r");
    if (f == NULL)
    {
        if (errno == ENOENT)
        {
            return 0;
        }
        return -1;
    }
    
    while (fgets(line, sizeof line, f) != NULL)
    {
        line[strcspn(line, "\n")] = '\0';
        secondes = strtoul(line, &end, 10);
        
        if (*end != '\t' || end == line)
        {
            continue;
        }
        
        nom = end + 1;
        
        if (*nom == '\0')
        {
            continue;
        }
        
        if (stats_add(s, nom, secondes) == -1)
        {
            fclose(f);
            return -1;
        }
    }
        
    fclose(f);
    return 0;
        
}
