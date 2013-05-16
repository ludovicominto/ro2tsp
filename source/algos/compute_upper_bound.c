#include "compute_upper_bound.h"

int compute_upper_bound(graph* G, cycle* C, int algo, double* ub) {

  if (algo == NEAREST_NEIGHBOUR) {

    int i, status, flag;
    double min, tour_cost;
    int n = G->n;

    cycle_delete(C);
    cycle_init(C, n);

    cycle C_tmp;
    cycle_init(&C_tmp, n);
    flag = 0;
    for (i = 1; i <= n; i++) {

      status = heur_nearest_neighbour(G, &C_tmp, i, &tour_cost);
      if (flag == 0 && status == SUCCESS) {
	cycle_copy(&C_tmp, C);
	min = tour_cost;
	flag = 1;
      }
      else if (flag == 1 && status == SUCCESS && tour_cost < min) {
	cycle_copy(&C_tmp, C);
	min = tour_cost;
      }

    }

    if (flag == 1) {
      *ub = min;
      cycle_delete(&C_tmp);
      return SUCCESS;
    }
    else {
      cycle_delete(&C_tmp);
      return FAILURE;
    }

  }

  else if (algo == NEAREST_NEIGHBOUR_2_OPT) {

    int i, status, flag;
    double min, tour_cost;
    int n = G->n;

    cycle_delete(C);
    cycle_init(C, n);

    cycle C_tmp;
    cycle_init(&C_tmp, n);
    flag = 0;
    for (i = 1; i <= n; i++) {

      status = heur_nearest_neighbour(G, &C_tmp, i, &tour_cost); // always returns successfully if this case is executed at the root node

      if (status == 1) {

	status = heur_2_opt(G, &C_tmp, tour_cost, &tour_cost); // always returns successfully
      
	if (flag == 0 && status == SUCCESS) {
	  cycle_copy(&C_tmp, C);
	  min = tour_cost;
	  flag = 1;
	}
	else if (flag == 1 && status == SUCCESS && tour_cost < min) {
	  cycle_copy(&C_tmp, C);
	  min = tour_cost;
	}

      }

    }

    if (flag == 1) {
      *ub = min;
      cycle_delete(&C_tmp);
      return SUCCESS;
    }
    else {
      cycle_delete(&C_tmp);
      return FAILURE;
    }

  }

  else if (algo == RANDOM_CYCLES) {

    int i, status, flag;
    double min, tour_cost;
    int n = G->n;

    cycle_delete(C);
    cycle_init(C, n);

    cycle C_tmp;
    cycle_init(&C_tmp, n);
    flag = 0;
    for (i = 1; i <= NUM_TRIALS_RANDOM_CYCLES; i++) {

      status = generate_random_cycle(G, &C_tmp, &tour_cost); // always returns successfully

      if (flag == 0 && status == SUCCESS) {
	cycle_copy(&C_tmp, C);
	min = tour_cost;
	flag = 1;
      }
      else if (flag == 1 && status == SUCCESS && tour_cost < min) {
	cycle_copy(&C_tmp, C);
	min = tour_cost;
      }

    }

    if (flag == 1) {
      *ub = min;
      cycle_delete(&C_tmp);
      return SUCCESS;
    }
    else {
      cycle_delete(&C_tmp);
      return FAILURE;
    }
  }

  else if (algo == RANDOM_CYCLES_2OPT) {

    int i, status, flag;
    double min, tour_cost;
    int n = G->n;

    cycle_delete(C);
    cycle_init(C, n);

    cycle C_tmp;
    cycle_init(&C_tmp, n);
    flag = 0;
    for (i = 1; i <= NUM_TRIALS_RANDOM_CYCLES_2OPT; i++) {

      status = generate_random_cycle(G, &C_tmp, &tour_cost); // always returns successfully

      if (status == 1) {

	status = heur_2_opt(G, &C_tmp, tour_cost, &tour_cost); // always returns successfully

	if (flag == 0 && status == SUCCESS) {
	  cycle_copy(&C_tmp, C);
	  min = tour_cost;
	  flag = 1;
	}
	else if (flag == 1 && status == SUCCESS && tour_cost < min) {
	  cycle_copy(&C_tmp, C);
	  min = tour_cost;
	}

      }

    }

    if (flag == 1) {
      *ub = min;
      cycle_delete(&C_tmp);
      return SUCCESS;
    }
    else {
      cycle_delete(&C_tmp);
      return FAILURE;
    }
  }
  
  else if (algo == DUMB) { 

    dumb_upper_bound(G, ub);

  }

}

int dumb_upper_bound(graph *G, double* ub) {
  // In this case computational complexity is cubic! Try to do better,
  // you can spend n^2 * logn time (where n is the number of vertices of the graph).
  // If you want to use some recursive algorithm such as merge-sort, then pay attention
  // to memory consumption; it may be better to use some in-place algorithm.

  double cost, max;
  int i, j, i_max, j_max, k, flag;
  int n = G->n;
  int* selected = (int*)calloc(n * (n + 1) / 2, sizeof(int));

  *ub = 0;
  for (k = 0; k < n; k++) {

    flag = 0;
    for (i = 1; i <= n; i++) {
      for (j = i+1; j <= n; j++) {
	if (i != j && selected[ j*(j-1)/2 + i-1] == 0) {

	  cost = graph_get_edge_cost(G, i, j);

	  if (flag == 0) {
	    max = cost;
	    i_max = i;
	    j_max = j;
	    flag = 1;
	  }
	  else if (flag == 1 && cost > max) {
	    max = cost;
	    i_max = i;
	    j_max = j;
	  }

	}
      }
    }

    selected[ j_max*(j_max-1)/2 + i_max-1] = 1;
    *ub += max;
  }

  free(selected);
  return SUCCESS;
}

int heur_nearest_neighbour(graph *G, cycle* C, int start_node, double* ub) {
  double min, tour_cost, cost;
  int k, i, v_min, curr, next, some_forced_edges, flag;

  int n = G->n;


  cycle_delete(C);
  cycle_init(C, n);


  int* visited = (int*)calloc(n, sizeof(int)); // visited[v] is equal to 1 if the vertex v has been visited, otherwise visited[v] is equal to 0

  curr = start_node;
  visited[start_node-1] = 1;


  // Main loop: at each step a new vertex is visited and a new edge is added to the solution.
  // Forced edges take priority on free edges, independently of their costs.
  // Forbidden edges cannot be chosen.
  // If at some certain point there is no possibility than choose a forbidden edge, then the procedure stops and returns FAILURE.
  for (k = 0; k < n-1; k++) {

    some_forced_edges = 0;
    for (i = 1; i <= n; i++) {
      if (visited[i-1] == 0) {
	if (graph_get_edge_constr(G, curr, i) == FORCED) {
	  some_forced_edges = 1;
	}
      }
    }

    if (some_forced_edges == 0) {

      flag = 0;
      v_min = 0;
      for (i = 1; i <= n; i++) {
	if (visited[i-1] == 0 && graph_get_edge_constr(G, curr, i) != FORBIDDEN) {
	  cost = graph_get_edge_cost(G, curr, i);
	  if (flag == 0) {
	    v_min = i;
	    min = cost;
	    flag = 1;
	  }
	  else if (flag == 1 && cost < min) {
	    v_min = i;
	    min = cost;
	  }
	}
      }

    }
    else {
      flag = 0;
      v_min = 0;
      for (i = 1; i <= n; i++) {
	if (visited[i-1] == 0 && graph_get_edge_constr(G, curr, i) == FORCED) {
	  cost = graph_get_edge_cost(G, curr, i);
	  if (flag == 0) {
	    v_min = i;
	    min = cost;
	    flag = 1;
	  }
	  else if (flag == 1 && cost < min) {
	    v_min = i;
	    min = cost;
	  }
	}
      }

    }


    if (v_min > 0) {
      C->nodes[k] = curr;
      visited[v_min-1] = 1;
      curr = v_min;
    }
    else {
      free(visited);
      return FAILURE;
    }

  }

  if (graph_get_edge_constr(G, curr, start_node) != FORBIDDEN) {
    C->nodes[n-1] = curr;
  }
  else {
    free(visited);
    return FAILURE;
  }

  int v1, v2, index_1;
  for (i = 0; i < n; i++) {
    if (C->nodes[i] == 1) {
      v1 = C->nodes[(i+1)%n];
      v2 = C->nodes[(i-1+n)%n];
      index_1 = i;
      break;
    }
  }

  // fill in cycle with costs
  for (i = 0; i < n; i++) {
    C->costs[i] = graph_get_edge_cost(G, C->nodes[i], C->nodes[(i+1)%n]);
  }

  if (v1 > v2) { // We do this only to take into account some possibile rounding errors /comparison errors with respect to the costs of the 1-trees computed by compute_ot... maybe this is not necessary.
    // compute the cost of the revese cycle
    *ub = 0.0;
    for (i = 0; i < n; i++) {
      *ub += C->costs[(index_1-i+n)%n];
    }
  }
  else {
    *ub = 0.0;
    for (i = 0; i < n; i++) {
      *ub += C->costs[(index_1+i)%n];
    }
  }

  free(visited);
  return SUCCESS;
}

/*
 * heur2opt
 * G : graph
 * C : cycle computed by another heuristic
 *     (will be modified if it's not 2-opt)
 * cost : cost of the solution computed by the heuristic
 *
 * compute a 2-opt admissible solution, in order to swap crossing edges
 *
 * return : cost of the 2-opt solution
 */
int heur_2_opt(graph *G, cycle *C, double ccost, double* ub) {

  int i, j, v, z, h, k, l;
  double delta;
  double cost = ccost;

  int n = C->n;
  int changed = 1;
  int tmp_nodes[n];

  //int MAX_ITER = 100;
  int counter = 0;

  while (changed /*&& counter < MAX_ITER*/) {
    counter++;
    changed = 0;


    for (i = 0; i < n ; i++) {

      for (j = i+2; j < n; j++) { // i+2: We don't want something like i-->i+1=j-->j+1

	if ((j+1) % n != i) {     // We don't want something like j-->j+1=i-->i+1 (this happens only if i=0 and j=n-1 so that n-1-->0-->1).

	  v = C->nodes[i];
	  z = C->nodes[(i+1) % n];
	  h = C->nodes[j];
	  k = C->nodes[(j+1) % n];

	  // We are guardanteed that v, z, h, k are all different vertices.
	  delta = graph_get_edge_cost(G, v, h) +
	          graph_get_edge_cost(G, z, k) -
	          graph_get_edge_cost(G, v, z) -
	          graph_get_edge_cost(G, h, k);


	  if (delta < 0.0) {

	      cost += delta;
	      
	      // Reverse the portion of the cycle which goes from index i+1 to index j;
	      // From ..., node[i]=v,  node[i+1]=z,  node[i+2]=v_1,  ...,  node[j-1]=v_s,  node[j]=h,  node[j+1]=k, ...
	      // To   ..., node[i]=v,  node[i+1]=h,  node[i+2]=v_s,  ...,  node[j-1]=v_1,  node[j]=z,  node[j+1]=k, ...
	      k = 0;
	      for (l = (i+1)%n; l != (j+1)%n; l = (l+1)%n) {
		tmp_nodes[k] = C->nodes[l];
		k++;
	      }

	      k = k-1;
	      for (l = (i+1)%n; l != (j+1)%n; l = (l+1)%n) {
		C->nodes[l] = tmp_nodes[k];
		k--;
	      }

	      changed = 1;
	      i = j = n;

	  }
	}
      }
    } // end for

  } // end while


  int v1, v2, index_1;
  for (i = 0; i < n; i++) {
    if ((*C).nodes[i] == 1) {
      v1 = C->nodes[(i+1) % n];
      v2 = C->nodes[(i-1+n) % n];
      index_1 = i;
      break;
    }
  }

  // fill in cycle with costs
  for (i = 0; i < n; i++) {
    C->costs[i] = graph_get_edge_cost(G, C->nodes[i], C->nodes[(i+1)%n]);
  }

  if (v1 > v2) { // to take into account rounding errors, maybe not necessary
    *ub = 0.0;
    for (i = 0; i < n; i++) {
      *ub += C->costs[(index_1-i+n)%n];
    }
  }
  else {
    *ub = 0.0;
    for (i = 0; i < n; i++) {
      *ub += C->costs[(index_1+i)%n];
    }
  }

  return SUCCESS;
}

/*
 * random_cycle_generation
 * - G : graph
 * - C : the best cycle computed
 * - num_cycles : number of cycles to be tried
 *
 * generate num_cycles random cycles, and keep the best one
 * (many thanks to Matteo Boscariol for the idea)
 *
 * return : cost of the returned cycle
 */
int generate_random_cycle(graph *G, cycle *C, double* ub) {
  double tour_cost;
  int i, remaining, target, tmp;

  int n = G->n;


  cycle_delete(C);
  cycle_init(C, n);

  // generate permutation of the nodes
  for (i = 0; i < n; i++) {
    C->nodes[i] = i+1;
  }

  remaining = n;
  for (i = 0; i < n; i++) {
    target = rand() % remaining;
    tmp = C->nodes[i];
    C->nodes[i] = C->nodes[target + i];
    C->nodes[target + i] = tmp;
    remaining--;
  }


  int v1, v2, index_1;
  for (i = 0; i < n; i++) {
    if (C->nodes[i] == 1) {
      v1 = C->nodes[(i+1) % n];
      v2 = C->nodes[(i-1+n) % n];
      index_1 = i;
      break;
    }
  }

  if (v1 > v2) { // We do this only to take into account some possibile rounding errors / comparison errors with respect to the costs of the 1-trees computed by compute_ot... maybe this is not necessary.
    // revese the two nodes, normally we would reverse the entire cycle but here the cycle is random generated so we only care that the successor of vertex 1 has index less than the index of its predecessor
    C->nodes[(index_1 + 1) % n] = v2;
    C->nodes[(index_1 - 1 + n) % n] = v1;
  }

  // fill in cycle with costs
  for (i = 0; i < n; i++) {
    C->costs[i] = graph_get_edge_cost(G, C->nodes[i], C->nodes[(i+1)%n]);
  }

 
  *ub = 0.0;
  for (i = 0; i < n; i++) {
    *ub += C->costs[(index_1+i)%n];
  }
 
  return SUCCESS;
}

/**
 * Compute an upper bound for the solution using a genetic algorithm
 * 
 * @param  G the graph
 * @param  C the cycle containing the best solution found
 * @return   cost of the returned solution
 */
double genetic(graph *G, cycle *C) {
  double cost = 0.;

  int i, j;
  int n = G->n;
  int iterations,
      iterations_wo_improvement = 0;

  cycle c,
        best_cycle;
  cycle_init(&c, n);
  cycle_init(&best_cycle, n);

  // reset
  cycle_delete(C);
  cycle_init(C, n);

  int population = 30;
  int tour[population][n];

  double fitness[population],
         norm_fitness[population], // normalized f., probability to be chosen
         costs[population];

  double best_fitness,
         best_cost;
  int best_index;

  double crossover_prob,
         mutation_prob;

  int target, remaining, tmp;

  // initialize
  for (i = 0; i < n; ++i) {
    for (j = 0; j < population; ++j) {
      tour[j][i] = i+1;
    }
  }

  for (i = 0; i < population; ++i) {

    // generate permutation of the node
    remaining = n;
    for (j = 0; j < n; ++j) {

      target = rand() % remaining;

      tmp                 = tour[i][j];
      tour[i][j]          = tour[i][target + j];
      tour[i][target + j] = tmp;

      remaining--;

    }

  }

  best_index = population + 1;
  best_fitness = 0.;

  for (i = 0; i < population; ++i) {
    costs[i] = 0.;
    for (j = 0; j < n-1; ++j) {
      costs[i] += graph_get_edge_cost(G, tour[i][j], tour[i][j+1]);
    }
    costs[i]  += graph_get_edge_cost(G, tour[i][n], tour[i][0]);
    fitness[i] = 1. / costs[i];

    if (fitness[i] > best_fitness) {
      best_fitness = fitness[i];
      best_cost    = costs[i];
      best_index   = i;
    }

  }

  // normalize probabilities
  for (i = 0; i < population; ++i) {
    norm_fitness[i] = fitness[i] / best_fitness;
  }

  int cp1, cp2; // crossover points




  return cost;
}
