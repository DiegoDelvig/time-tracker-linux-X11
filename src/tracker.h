#ifndef TRACKER_H
#define TRACKER_H

#include "stats.h"

typedef struct tracker tracker_t;

/* Crée un tracker avec une table vide. NULL si X est injoignable
 * ou si l'allocation échoue. À libérer avec tracker_destroy(). */
tracker_t *tracker_create(void);

/* Libère le tracker, sa sonde X et sa table. Accepte NULL. */
void tracker_destroy(tracker_t *t);

/* À appeler une fois par seconde : ajoute 1 seconde à l'application
 * active. Pas de fenêtre active ou processus disparu : rien n'est
 * ajouté, ce n'est pas une erreur.
 * Retourne 0 si OK, -1 si erreur X ou d'allocation. */
int tracker_tick(tracker_t *t);

/* Accès en lecture seule à la table, valide jusqu'à tracker_destroy. */
const stats_t *tracker_stats(const tracker_t *t);

/* Charge le fichier path dans la table du tracker (les secondes
 * s'ajoutent à l'existant). Fichier absent : retourne 0.
 * Retourne 0 si OK, -1 si erreur d'E/S ou d'allocation. */
int tracker_load(tracker_t *t, const char *path);


/* Sauvegarde la table du tracker dans path (écriture atomique).
 * Retourne 0 si OK, -1 sinon (errno positionné). */
int tracker_save(const tracker_t *t, const char *path);

#endif
