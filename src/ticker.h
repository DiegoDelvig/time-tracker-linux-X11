#ifndef TICKER_H
#define TICKER_H

#include <signal.h>

/* Fonction appelée à chaque tick, n = num de ticks
 * *userdata = pointeur opaque fournit par l'appelant */
typedef void (*ticker_cb) (unsigned int n, void* userdata);

/* appelle on_tick une fois par sec, -> 0 si OK
 *                                   -> -1 si KO
 */
int ticker_run(unsigned count, ticker_cb on_tick, void* userdata);

/* Appelle on_tick une fois par seconde tant que *stop vaut 0.
 * *stop est typiquement mis à 1 par un handler de signal.
 * Sort au plus tard ~1 s après que *stop est passé à 1 (le sommeil
 * est interrompu par le signal).
 * Retourne 0 à la sortie normale, -1 si on_tick ou stop est NULL. */
int ticker_run_until(const volatile sig_atomic_t *stop, ticker_cb on_tick, void *userdata);

#endif
