#ifndef PROC_INFO_H
#define PROC_INFO_H

#include <stddef.h>
#include <sys/types.h>

/* Écrit dans buf le nom du processus pid, terminé par '\0', sans '\n'.
 * buf est fourni par l'appelant et fait size octets (terminateur inclus).
 * Si le nom est trop long, il est tronqué (buf reste valide).
 *
 * Retourne 0 si OK.
 * Retourne -1 en cas d'erreur, avec errno positionné :
 *   EINVAL si buf est NULL ou size vaut 0,
 *   sinon l'erreur du système (ENOENT si le processus n'existe pas...).
 * En cas d'erreur, le contenu de buf est indéfini. */
int proc_info_get_name(pid_t pid, char *buf, size_t size);

#endif /* PROC_INFO_H */
