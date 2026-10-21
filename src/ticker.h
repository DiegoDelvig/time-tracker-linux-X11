#ifndef TICKER_H
#define TICKER_H

/* Fonction appelée à chaque tick, n = num de ticks
 * *userdata = pointeur opaque fournit par l'appelant */
typedef void (*ticker_cb) (unsigned int n, void* userdata);

/* appelle on_tick une fois par sec, -> 0 si OK
 *                                   -> -1 si KO
 */
int ticker_run(unsigned count, ticker_cb on_tick, void* userdata);
#endif
