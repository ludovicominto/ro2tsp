#include "cpx_add_kruskal_secs.h"

int cpx_add_kruskal_secs(CPXENVptr   env,
                         CPXLPptr    lp,
                         cpx_table  *hash_table,
                         onetree    *OT,
                         parameters *pars)
{

  int i, k, status;

  int n = (*OT).n;

  double cost, cost_min;
  int flag, mark1, mark2, x, y;

  int vrtx_mrks[n];
  for (i = 0; i < n; i++) vrtx_mrks[i] = i+1;

  int selected_edges[n];
  memset(selected_edges, 0, sizeof(selected_edges));

  for (k = 0; k < n-2; k++) {

    flag = 0;
    for (i = 2; i <= n; i++) {
      if (i != (*OT).v1 && selected_edges[i-1] == 0) {

        cost = onetree_get_edge_cost(OT, onetree_get_pred(OT, i), i);
             
        if (flag == 0 || cost < cost_min) {
          x = onetree_get_pred(OT, i);
          y = i;
          cost_min = cost;
          flag = 1;
        }

      }
    }
    selected_edges[y-1] = 1;

    mark1 = vrtx_mrks[x-1];
    mark2 = vrtx_mrks[y-1];

    for (i = 2; i <= n; i++) {

      if (vrtx_mrks[i-1] == mark2) vrtx_mrks[i-1] = mark1;

    }

    status = cpx_add_my_sec(env, lp, hash_table,
                            vrtx_mrks, n, mark1, pars);
    assert(status == SUCCESS);

  }

  printf("# added Kruskal SECs\n");
  return status;
}
