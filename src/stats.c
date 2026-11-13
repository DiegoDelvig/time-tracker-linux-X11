#include "stats.h"

#include <stdlib.h>
#include <string.h>

typedef struct 
{
    char *name;
    unsigned long seconds;
} entry_t;

struct stats
{
    entry_t *items;  /* tableau alloué, capacity cases */
    size_t count;    /* cases utilisées */
    size_t capacity; /* cases allouées */
};

/* Copie s dans un nouveau bloc malloc. NULL si échec. */
static char *dup_string(const char *s)
{
    size_t len = strlen(s) + 1;
    char *out = malloc(len);
    
    if (out == NULL)
    {
        return NULL;
    }
    
    memcpy(out, s, len);
    
    return out;
}

/* Retourne l'index de name, ou -1 si absent. */
static long find(const stats_t *s, const char *name)
{
    size_t i;
    
    for (i = 0; i < s->count; i++)
    {
        if (strcmp(s->items[i].name, name) == 0)
        {
            return i;
        }
    }
    
    return -1;
}

/* Garantit une case libre. Retourne 0 ou -1. */
static int grow_if_needed(stats_t *s)
{
    void *tmp;
    
    if (s->count < s->capacity)
    {
        return 0;
    }
    
    size_t new_cap = s->capacity ? s->capacity * 2 : 4;
    tmp = realloc(s->items, new_cap * sizeof *s->items);
    
    if (tmp == NULL)
    {
        return -1;
    }
    
    s->items = tmp;
    s->capacity = new_cap;
    
    return 0;
}

stats_t *stats_create(void)
{
    stats_t *s = calloc(1, sizeof *s);
    return s;
}

void stats_destroy(stats_t *s)
{
    size_t i;
    
    if (s == NULL)
    {
        return;
    }
    
    for (i = 0; i < s->count; i++)
    {
        free(s->items[i].name);
    }
    
    free(s->items);
    free(s);
}

int stats_add(stats_t *s, const char *name, unsigned long seconds)
{
    long idx;
    char *dup;
    
    if (s == NULL || name == NULL || name[0] == '\0')
    {
        return -1;
    }
    
    idx = find(s, name);
    if (idx >= 0)
    {
        s->items[idx].seconds += seconds;
        return 0;
    }
    
    if (grow_if_needed(s) != 0)
    {
        return -1;
    }
    
    dup = dup_string(name);
    if (dup == NULL)
    {
        return -1;
    }
    
    s->items[s->count].name = dup;
    s->items[s->count].seconds = seconds;
    s->count++;
    
    return 0;
}

size_t stats_count(const stats_t *s)                          
{ 
    if (s == NULL)
    {
        return 0;
    }
    
    return s->count;
}
const char *stats_name_at(const stats_t *s, size_t index)     
{
    if (s == NULL || index >= s->count)
    {
        return NULL;
    }
    
    return s->items[index].name;
}

unsigned long stats_seconds_at(const stats_t *s, size_t index)
{
    if (s == NULL || index >= s->count)
    {
        return 0;
    }
    
    return s->items[index].seconds;
}



