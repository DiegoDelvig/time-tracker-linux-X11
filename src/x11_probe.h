#ifndef X11_PROBE_H
#define X11_PROBE_H

#include <sys/types.h>

typedef struct x11_probe x11_probe_t;

/* Ouvre la connexion au serveur X.
 * Retourne un pointeur à libérer avec x11_probe_destroy(),
 * ou NULL si le serveur est injoignable ou si l'allocation échoue. */
x11_probe_t *x11_probe_create(void);

/* Ferme la connexion et libère probe. Accepte NULL (ne fait rien). */
void x11_probe_destroy(x11_probe_t *probe);

/* Écrit dans *pid le PID du processus de la fenêtre active.
 * Retourne  0 si OK,
 *           1 si aucune fenêtre active ou si elle n'expose pas de PID
 *             (cas normal, pas une erreur),
 *          -1 si probe ou pid est NULL, ou si la requête X échoue.
 * *pid n'est modifié que si la valeur de retour est 0. */
int x11_probe_active_pid(x11_probe_t *probe, pid_t *pid);

#endif
