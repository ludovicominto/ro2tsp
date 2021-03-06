#ifndef __CPX_CALLBACKS_H
#define __CPX_CALLBACKS_H


#include <ilcplex/cplex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../base/utils.h"

//#include "cpx_solver.h"
#include "cpx_add_secs.h"
#include "cpx_table.h"
#include "cpx_components.h"


/**
 * parameters passed to the callback
 */
typedef struct _cutinfo {
  CPXLPptr    lp;
  int         numcols;
  int         num;
  double     *x;
  int        *beg;
  int        *ind;
  double     *val;
  double     *rhs;
  cpx_table   hash_table;
  int         number_of_nodes;
  parameters *pars;
} cutinfo;

typedef struct _icbinfo {

  CPXLPptr    lp;
  cpx_table  *hash_table;
  parameters *pars;

} icbinfo;


/**
 * [cpx_subtour_callback description]
 * @param  env          CPLEX environment
 * @param  lp           problem
 * @param  cbdata       pointer to callback parameters
 * @param  wherefrom    integer reporting where the callback was called from
 * @param  cbhandle     poiter to cutinfo struct, containing parameters
 *                      to the user-written callback
 * @param  useraction_p action to be taken on callback completion
 * @return              status of the operation performed
 */
int CPXPUBLIC cpx_subtour_callback(CPXCENVptr  env,
                                   void       *cbdata,
                                   int         wherefrom,
                                   void       *cbhandle,
                                   int        *useraction_p);


/**
 * [cpx_flow_callback description]
 * @param  env          [description]
 * @param  cbdata       [description]
 * @param  wherefrom    [description]
 * @param  cbhandle     [description]
 * @param  useraction_p [description]
 * @return              [description]
 */
int CPXPUBLIC cpx_flow_callback(CPXCENVptr  env,
                                void       *cbdata,
                                int         wherefrom,
                                void       *cbhandle,
                                int        *useraction_p);



#endif
