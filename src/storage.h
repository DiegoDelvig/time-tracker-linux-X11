#ifndef STORAGE_H
#define STORAGE_H

#include "stats.h"

/* Écrit la table dans le fichier path, un enregistrement par ligne :
 *   <secondes>\t<nom>\n
 * L'écriture est atomique : le fichier est d'abord écrit dans
 * "<path>.tmp" puis renommé. En cas d'échec, l'ancien fichier
 * reste intact.
 * Retourne 0 si OK, -1 sinon (errno positionné). */
int storage_save(const stats_t *s, const char *path);

/* Lit le fichier path et ajoute ses enregistrements à s.
 * Fichier absent : retourne 0, s inchangée (premier lancement).
 * Ligne mal formée : ignorée.
 * Retourne 0 si OK, -1 si erreur d'E/S ou d'allocation. */
int storage_load(stats_t *s, const char *path);

#endif
