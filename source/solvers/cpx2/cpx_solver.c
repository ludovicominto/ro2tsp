#include "cpx_solver.h"

void cpx_solver(cpx_env *ce, cpx_stats *cs, parameters *pars) {

  int status = 0;

  CPXENVptr env;
  CPXLPptr lp;
  int i, j, idx;

  cpx_table_init(&ce->T, &ce->G_INPUT);

  status = cpx_create_problem(&env, &lp, "problem");
  if (status) {
    fprintf(stderr, "Fatal error in solvers/cpx/cpx_solver.c:\n"
		    "function: cpx_solver:\n"
                    "cpx_create_problem : %d\n", status);
    exit(1);
  }

  status = cpx_setup_problem(env, lp, ce);
  if (status) {
    fprintf(stderr, "Fatal error in solvers/cpx/cpx_solver.c:\n"
		    "function: cpx_solver:\n"
                    "cpx_setup_problem : %d\n", status);
    exit(1);
  }

  status = cpx_set_problem_parameters(env, lp, ce);
  if (status) {
    fprintf(stderr, "Fatal error in solvers/cpx/cpx_solver.c:\n"
		    "function: cpx_solver:\n"
                    "cpx_set_problem_parameters : %d\n", status);
    exit(1);
  }

  status = cpx_preprocessing(env, lp, ce, cs, pars);
  if (status) {
    fprintf(stderr, "Fatal error in solvers/cpx/cpx_solver.c:\n"
		    "function: cpx_solver:\n"
                    "cpx_preprocessing : %d\n", status);
    exit(1);
  }

  status = CPXwriteprob(env, lp, "myprob.lp", "LP");
  if (status) {
    fprintf(stderr, "Fatal error in solvers/cpx/cpx_solver.c:\n"
		    "function: cpx_solver:\n"
                    "CPXwriteprob : %d\n", status);
    exit(1);
  }


  int n = ce->G.n;
  double x[n * (n - 1) / 2];
  int solstat;

  ce->mylp = lp;
  
  //status = cpx_solve_iterative(env, lp, ce, cs, x, n * (n - 1) / 2, &solstat);
  //status = cpx_solve_miliotis(env, lp, ce, cs, x, n * (n - 1) / 2, &solstat);
  status = cpx_solve_proximity(env, lp, ce, cs, x, n * (n - 1) / 2, &solstat);


  graph_init(&ce->G_OUTPUT, n);
  for (idx = 0; idx < n * (n - 1) / 2; idx++) {

	if (x[idx] > 0.9) {

		vrtx_from_idx(&ce->T, &i, &j, idx);
		graph_insert_edge(&ce->G_OUTPUT, i, j, 0.0, FREE);
		cs->z_opt += graph_get_edge_cost(&ce->G_INPUT, i, j);

	}
  }

}
