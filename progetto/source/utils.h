#ifndef __UTILS_H
#define __UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include "graph.h"

/* constants and variables */

typedef struct _parameters {
	unsigned int no_of_nodes;
	unsigned int seed;
	unsigned short plot;
	unsigned short plotOnlyTree;
	unsigned int heuristic_trials;
	char *tsp_file;
} parameters;

// config file
#define FILE_CONFIG "config"

// define infinity for double
#define INF 100000000.0

/* methods */

// position in the list of costs (matrixGraph->edgeList):
// Corresponding edge (i,j) is in position (binom(i-1, 2) + j)
// in the list of edges - look for 'triangular numbers'.
// Note that since we start counting from 0, the formula
// for triangular numbers should be adjusted accordingly.
// A swap is needed if j > i, but it's fine since graph is directed;
// note that j=i cannot happen, because we don't have self-loops.
// inline int atPosition(int, int);

unsigned long initializeRandom(unsigned int seed);

short parHash(char *);
parameters *getParameters();

/*
 * tspHash
 * - parameter name
 * - parameter value
 *
 * recognizes parameters.
 *
 * returns:
 *  0 : parName = 'name'
 *  1 : (parName, parValue) = ('type', 'tsp')
 *  2 : parName = 'comment'
 *  3 : parName = 'dimension'
 * 41 : (parName, parValue) = ('EDGE_WEIGHT_TYPE', 'EUD_2D')
 * 42 : (parName, parValue) = ('EDGE_WEIGHT_TYPE', 'MAN_2D')
 * 43 : (parName, parValue) = ('EDGE_WEIGHT_TYPE', 'CEIL_2D')
 * 44 : (parName, parValue) = ('EDGE_WEIGHT_TYPE', 'GEO')
 * 51 : (parName, parValue) = ('DISPLAY_DATA_TYPE', 'COORD_DISPLAY')
 * 61 : parName = 'NODE_COORD_SECTION'
 * 62 : parName = 'DISPLAY_DATA_SECTION'
 * -1 : all the other combinations/values
 */
short tspHash(char*, char*);

/*
 * read_tsp_from_file
 * - G : graph to be filled in
 * - pars : parameters (some may be overridden)
 *
 * will override parameters, if needed
 *
 * Assumes file is formatted in the right way.
 * Does not guarantee correct working if this is not true.
 * Flexibility = 0!
 */
void read_tsp_from_file(graph *G, parameters *ppars);

typedef int (*compfn)(const void*, const void*);
//int snbdComp(const void *, const void *);
int snbdComp(node *, node *);
// int sebwComp(const void *, const void *);

#endif