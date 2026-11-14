#ifndef PATHS_H
#define PATHS_H

#include <stddef.h>

/* Ecrit dans buf le chemin du fichier de stats:
 * $XDG_DATA_HOME/timetracker/stats.dat
 * ou, si XDG_DATA_HOME est absent ou vide,
 * $HOME/.local/share/timetracker/stats.dat
 * Crée les dossiers manquants
 * Retourne 0 si OK, -1 KO
 * errno positionné : EINVAL si ni HOME ni XDG_DATA_HOME,
 * ENAMETOOLONG si buf est trop petit.
 */
int paths_stats_file(char *buf, size_t size);

#endif
