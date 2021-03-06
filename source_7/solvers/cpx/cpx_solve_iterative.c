#include "../../solvers/cpx/cpx_solve_iterative.h"

int cpx_solve_iterative(CPXENVptr    cplexenv,
                        CPXLPptr     lp,
                        environment	*env,
                        parameters  *pars,
                        statistics  *stats,
                        double      *x,
                        int          x_size,
                        int         *solstat) {

	int status = 0;

	int n       = env->main_graph.vrtx_num;
	int numcols = n * (n - 1) / 2;
	int vrtx_comp[n];

	clock_t t1, t2;
	double maxtime = 3600;

	status = CPXsetintparam (cplexenv, CPX_PARAM_MIPSEARCH, CPX_MIPSEARCH_DYNAMIC);
	//status = CPXsetintparam (env, CPX_PARAM_MIPSEARCH, CPX_MIPSEARCH_TRADITIONAL);
	if (status) {
		fprintf(stderr, "Fatal error in solvers/cpx/cpx_solve_iterative.c:\n"
	                    "function: cpx_solve_iterative:\n"
	                    "CPXsetinparam (CPX_PARAM_MIPSEARCH) : %d\n", status);
	    return status;
	}

	/* each variable in the original model can be expressed as
	 * a linear form of variables in the presolved model
	 */
	//status = CPXsetintparam (env, CPX_PARAM_PRELINEAR, CPX_ON);
	status = CPXsetintparam (cplexenv, CPX_PARAM_PRELINEAR, CPX_OFF);
	if (status) {
		fprintf(stderr, "Fatal error in solvers/cpx/cpx_solve_iterative.c:\n"
						"function: cpx_solve_iterative:\n"
	                  	"CPXsetinparam (CPX_PARAM_PRELINEAR) : %d\n", status);
		return status;
	}

	//status = CPXsetintparam (env, CPX_PARAM_REDUCE, CPX_PREREDUCE_NOPRIMALORDUAL);
	status = CPXsetintparam (cplexenv, CPX_PARAM_REDUCE, CPX_PREREDUCE_PRIMALANDDUAL);
	//status = CPXsetintparam (env, CPX_PARAM_REDUCE, CPX_PREREDUCE_PRIMALONLY);
	//status = CPXsetintparam (env, CPX_PARAM_REDUCE, CPX_PREREDUCE_DUALONLY);
	if (status) {
		fprintf(stderr, "Fatal error in solvers/cpx/cpx_solve_iterative.c:\n"
						"function: cpx_solve_iterative:\n"
						"CPXsetinparam (CPX_PARAM_REDUCE) : %d\n", status);
		return status;
	}

	// use reduced (on) or original (off)?
	status = CPXsetintparam (cplexenv, CPX_PARAM_MIPCBREDLP, CPX_ON);
	//status = CPXsetintparam (env, CPX_PARAM_MIPCBREDLP, CPX_OFF);
	if (status) {
		fprintf(stderr, "Fatal error in solvers/cpx/cpx_solve_iterative.c:\n"
						"function: cpx_solve_iterative:\n"
						"CPXsetinparam (CPX_PARAM_MIPCBREDLP) : %d\n", status);
		return status;
	}

	  /*
	  status = CPXsetintparam (env, CPX_PARAM_PREDUAL, -1);
	  //status = CPXsetintparam (env, CPX_PARAM_PREDUAL, 0);
	  if (status) {
	    fprintf(stderr, "Fatal error in solvers/cpx/cpx_solve_iterative.c:\n"
	                    "function: cpx_solve_iterative:\n"
	                    "CPXsetinparam (CPX_PARAM_PREDUAL) : %d\n", status);
	    return status;
	  }
	  */

	  /*
	  status = CPXsetdblparam(env, CPX_PARAM_CUTUP, ce->init_ub);
	  if (status) {
	    fprintf(stderr, "Fatal error in solvers/cpx/cpx_solve_iterative.c:\n"
	                    "function: cpx_solve_iterative:\n"
	                    "CPXsetdblparam (CPX_PARAM_CUTUP) : %d\n", status);
	    return status;
	  }
	  */

	  /*
	  status = CPXsetdblparam(env, CPX_PARAM_CUTLO, ce->init_lb);
	  if (status) {
	    fprintf(stderr, "Fatal error in solvers/cpx/cpx_solve_iterative.c:\n"
	                    "function: cpx_solve_iterative:\n"
	                    "CPXsetdblparam (CPX_PARAM_CUTLO) : %d\n", status);
	    return status;
	  }
	  */

	// WHY THIS???? don't remember
	status = CPXpresolve(cplexenv, lp, CPX_ALG_NONE);
	if (status) {
		fprintf(stderr, "Fatal error in solvers/cpx/cpx_solve_iterative.c:\n"
						"function: cpx_solve_iterative:\n"
						"CPXpresolve : %d\n", status);
		return status;
	}

	/* Actually solve the problem, iterating while there are more than one
	 * connected components (aka subtours)
	 */
	set vertex_set;
	set_init(&vertex_set);
	set_setup(&vertex_set, n);

	cpx_cstr sec;
	cpx_cstr_init(&sec);
	cpx_cstr_setup(&sec, numcols);

	int i, k, my_edge, my_comp, s1, s2, iters = 0;

	int  numcomp		= 0;
	int *components 	= (int*)malloc(n * sizeof(int));

	while (numcomp != 1) {

		// set time limit
		// printf("maxtime %f\n", maxtime);
		status = CPXsetdblparam(cplexenv, CPX_PARAM_TILIM, maxtime);
		if (status) {
			fprintf(stderr, "Fatal error in solvers/cpx/cpx_solve_iterative.c:\n"
							"function: cpx_solve_iterative:\n"
							"CPXsetdblparam : %d\n", status);
			return 1;
		}

		/* optimize */
		t1 = clock();
		status = CPXmipopt(cplexenv, lp);
		if (status) {
			fprintf(stderr, "Fatal error in solvers/cpx/cpx_solve_iterative.c:\n"
							"function: cpx_solve_iterative:\n"
							"CPXmipopt : %d\n", status);
			return 1;
		}
		t2 = clock();

		/* retrieve solution status
		 * exit if CPXMIP_INFEASIBLE */
		*solstat = CPXgetstat(cplexenv, lp);
		if (*solstat == 103) break;

		if (CPXgetnumcols(cplexenv, lp) != numcols) {
			fprintf(stderr, "Fatal error in solvers/cpx/cpx_solve_iterative.c:\n"
					 	 	"function: cpx_solve_iterative:\n"
					 	 	"unexpected number of columns in the current solution\n");
			return status;
		}


		double zobjval;
		status = CPXgetobjval(cplexenv, lp, &zobjval);
		printf("objval: %f, iteration : %d\n", zobjval, iters++);

		/* retrieve solution coefficients */
		status = CPXgetx(cplexenv, lp, x, 0, numcols-1);
		if (status) {
			fprintf(stderr, "Fatal error in solvers/cpx/cpx_solve_iterative.c:\n"
							"function: cpx_solve_iterative:\n"
							"CPXgetx : %d\n", status);
			return 1;
		}

		// don't remember what this does
		status = CPXfreepresolve(cplexenv, lp);
		if (status) {
			fprintf(stderr, "Fatal error in solvers/cpx/cpx_solve_iterative.c:\n"
							"function: cpx_solve_iterative:\n"
							"CPXfreepresolve : %d\n", status);
			return 1;
		}

		/* mark the connected components */
		set_empty(&vertex_set);
		for (my_edge = 0; my_edge < numcols; my_edge++) {
			if (x[my_edge] > 0.9)  {
				set_find(&vertex_set, get_v1(my_edge), &s1);
				set_find(&vertex_set, get_v2(my_edge), &s2);
				if (s1 != s2) {
					set_union(&vertex_set, s1, s2);
				}
			}
		}

		for (k = 0; k < n; k++) components[k] = 0;
		for (k = 0; k < n; k++) components[vertex_set.list[k]] = 1;
		numcomp = 0;
		for (k = 0; k < n; k++) {
			if (components[k] == 1) numcomp++;
		}

		/* if there are more than one connected components, and therefore subtours,
		 * generate the relative constraints and add them to the model,
		 * before another iteration of CPLEX */
		if (numcomp > 1) {

			for (i = 0; i < n; i++) {
				if (components[i] == 1) {

					my_comp = i;

					/* ad the sec (subtour elimination constraint)
					 * associated to this component
					 */
					cpx_cstr_empty(&sec);
					status = cpx_cstr_sec(env, &vertex_set, my_comp, &sec);
					if (status) {
						fprintf(stderr, "Fatal error in solvers/cpx/cpx_solve_iterative.c:\n"
										"function: cpx_solve_iterative:\n"
										"cpx_cstr_sec : %d\n", status);
				        return 1;
					}

					status = cpx_cstr_add(cplexenv, lp, &sec);
					if (status) {
						fprintf(stderr, "Fatal error in solvers/cpx/cpx_solve_iterative.c:\n"
										"function: cpx_solve_iterative:\n"
										"cpx_cstr_add : %d\n", status);
						return 1;
					}

				} /* end if components[i] == 1 */

			} /* end for i */

			printf("# SECs added: %d\n", numcomp);

		} /* end if numcomp > 1 */

		maxtime = maxtime - (double)(t2 - t1) / CLOCKS_PER_SEC;
		if (maxtime <= 0) {
			numcomp = 1;
			printf("time limit reached\n");
		}

	} /* end while */


	free(components);
	set_delete(&vertex_set);
	cpx_cstr_delete(&sec);


	return 0;

}
