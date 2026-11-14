#ifndef RENDER_H
#define RENDER_H

#include "stats.h"

#include <stdio.h>

/* Écrit sur out une ligne par application, triées par durée décroissante :
 *   <nom>  <jauge>  <durée>  <pourcentage>
 * width = largeur du terminal en colonnes (la jauge s'adapte ; en dessous
 * de 40, on utilise 40). Le pourcentage est relatif au total.
 * Table vide : écrit "Aucune donnee." et retourne 0.
 * Retourne 0 si OK, -1 si paramètre NULL ou allocation ratée. */
int render_jauges(FILE *out, const stats_t *s, unsigned width);

/* Écrit dans buf (size octets) une durée lisible : "42s", "3m 05s",
 * "1h 23m". Retourne 0, ou -1 si buf est NULL ou trop petit. */
int render_duration(unsigned long seconds, char *buf, size_t size);

#endif 
