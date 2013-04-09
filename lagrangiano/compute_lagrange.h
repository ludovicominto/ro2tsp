#ifndef COMPUTE_LAGRANGE_H_
#define COMPUTE_LAGRANGE_H_

#include "graph.h"

/* calcola e ritorna un (buon) lower bound per TSP su grafo G utilizzando l'algoritmo subgradiente - moltiplicatori lagrangiani: in pratica calcola il miglior valore della funzione lagrangiana associata al TSP su G;
 *
 * - G: grafo euclideo;
 * - ub: upper bound al costo di un ciclo hamiltoniano su G; la situazione ideale è ub = costo(ciclo hamiltoniano ottimo per G);
 * - k: numero di iterazioni senza miglioramento del valore della funzione lagrangiana dopo il quale la ricerca del massimo viene interrotta;
 * - alpha: parametro per ridurre opportunamente il passo di avanzamento nel corso delle iterazioni;
 * - h: numero di iterazioni senza miglioramento del valore della funzione lagrangiana dopo il quale alpha viene dimezzato;
 * - max_iter: numero massimo di iterazioni dopo il quale la ricerca del massimo della funzione lagrangiana viene interrotta;
 *
 * - nota1: l'algoritmo subgradiente trova il miglior (maggiore) valore della funzione lagrangiana associata al TSP su G asintoticamente: in questa implementazione la ricerca viene interrotta dopo al più un numero 'max_iter' di iterazioni, e il valore trovato potrebbe non essere il valore asintotico;
 * - nota2: il miglior (maggiore) valore della funzione lagrangiana corrisponde al valore di un ciclo hamiltoniano ottimo per G.
 */
double compute_lagrange(graph* G, double ub, int ub_max_iter, double alpha, int alpha_max_iter, int max_iter);

/* funzione identica a compute_lagrange ma aggiunte istruzioni per stampa valori e disegno grafico.
 */
double compute_and_plot_lagrange(graph* G, double ub, int ub_max_iter, double alpha, int alpha_max_iter, int max_iter);

#endif