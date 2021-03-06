#ifndef GRAPH_H_
#define GRAPH_H_

#include <stdlib.h>
#include <stdio.h>

// nodo del grafo
struct graph_node {
  int deg; // grado del nodo
};

typedef struct graph_node graph_node;

// lato del grafo
struct graph_edge {
  int flag; // flag=1 se il lato è presente, flag=0 altrimenti
  double cost; // costo del lato
};

typedef struct graph_edge graph_edge;

// grafo
struct graph {
  int n; // numero di nodi del grafo;
  graph_node* V; // array di nodi del grafo
  graph_edge* E; // array di lati del grafo
};

typedef struct graph graph;

// nota: anche se la struttura del grafo prevede la presenza di loop, i controlli nelle funzioni escludono la creazione di loop

// def: un grafo G è "cancellato" se la successione di operazioni di inizializzazione e cancellazione su di esso è del tipo: I, D, D, ..., D, I, D, D, ..., D, ..., I, D, D, ..., D
// def: un grafo G è "inizializzato" se la successione di operazioni di inizializzazione e cancellazione su di esso è del topo: I, D, D, ..., D, I, D, D, ..., D, ..., I
// def: un grafo G è "non-inizializzato" se su di esso non è stata eseguita alcuna operazione di inizializzazione o cancellazione

// inizializza il grafo
// Hp: G non-inizializzato oppure cancellato
// Hp: n > 0 *
void graph_init(graph* G, int n);

// cancella il grafo
// Hp: G inizializzato oppure cancellato
void graph_delete(graph* G);

// copia il primo grafo nel secondo grafo
// Hp: FROM inizializzato
// Hp: TO inizializzato o cancellato
void graph_copy(graph* FROM, graph* TO);

// inserisce un lato
// inserisce il lato {u, v} e imposta il suo costo c({u, v})=cost
// Hp: G inizializzato
// Hp: 1 <= u, v <= n *
// Hp: u != v *
// Hp: u e v non sono adiacenti *
void graph_insert_edge(graph* G, int u, int v, double cost);

// elimina un lato
// elimina il lato {u, v}
// Hp: G inizializzato
// Hp: 1 <= u, v <= n *
// Hp: u != v *
// Hp: u e v sono adiacenti *
void graph_remove_edge(graph* G, int u, int v);

// imposta il costo di un lato
// imposta c({u, v})=cost
// Hp: G inizializzato
// Hp: 1 <= u, v <= n *
// Hp: u != v *
// Hp: u e v sono adiacenti *
void graph_set_edge_cost(graph* G, int u, int v, double cost);

// ritorna il costo di un lato
// ritorna c({u, v})
// Hp: G inizializzato
// Hp: 1 <= u, v <= n *
// Hp: u != v *
// Hp: u e v sono adiacenti *
double graph_get_edge_cost(graph* G, int u, int v);

// ritorna il grado di un nodo
// Hp: G inizializzato
// Hp: 1 <= v <= n *
int graph_get_node_deg(graph* G, int v);

// ritorna TRUE se c'è un lato tra i due nodi, ritorna FALSE altrimenti
// Hp: G inizializzato
// Hp: 1 <= u, v <= (*G).n *
// Hp: u != v *
int graph_adjacent_nodes(graph* G, int u, int v);

// ritorna il costo del grafo
// Hp: G inizializzato
double graph_get_cost(graph* G);

#endif
