#ifndef STATS_H
#define STATS_H
#include <stddef.h>

typedef struct stats stats_t;

/* Retourne une table vide, à libérer avec stats_destroy(), ou NULL si échec. */
stats_t *stats_create(void);

/* Libère la table et tous les noms. Accepte NULL. */
void stats_destroy(stats_t *s);

/* Ajoute seconds à l'application name (créée si absente). name est copié :
 * l'appelant garde la propriété de sa chaîne.
 * Retourne 0 si OK, -1 si s ou name est NULL, ou si l'allocation échoue.
 * En cas d'échec, la table reste valide et inchangée. */
int stats_add(stats_t *s, const char *name, unsigned long seconds);

/* Nombre d'application */
size_t stats_count(const stats_t *s);

/* Accès par index (0 <= index < stats_count). Hors limites : NULL / 0.
 * Le pointeur retourné par stats_name_at reste valide jusqu'à stats_destroy. */
const char *stats_name_at(const stats_t *s, size_t index);
unsigned long stats_seconds_at(const stats_t *s, size_t index);

#endif
