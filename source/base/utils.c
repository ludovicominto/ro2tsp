#include "utils.h"


/*
 * parHash
 * parName : the string
 *
 * 'manual' hashing for the parameter name, to fill in the parameter list
 * the values are hard-coded here
 * Maybe ugly, but easy.
 *
 * return : an integer associated to the parameter
 */

short parHash(char *parName) {
  if (strcmp(parName, "NUMBER_OF_NODES") == 0)
    return 0;
  if (strcmp(parName, "RANDOM_INSTANCE") == 0)
    return 1;
  if (strcmp(parName, "RANDOM_SEED") == 0)
    return 2;
  if (strcmp(parName, "SEED") == 0)
    return 3;
  if (strcmp(parName, "TSP_FILE_INSTANCE") == 0)
    return 4;
  if (strcmp(parName, "TSP_FILE") == 0)
    return 5;
  if (strcmp(parName, "HEURISTIC_TRIALS") == 0)
    return 6;
  if (strcmp(parName, "CHOSEN_SOLVER") == 0)
    return 7;
  if (strcmp(parName, "HEURISTIC_ALGO") == 0)
    return 8;

  return -1;
}


// read parameters from config file
// default values for parameters are set here


parameters *getParameters() {
  parameters *pars = (parameters *)calloc(1, sizeof(parameters));

  //printf("@getParameters : calloc done\n");

  pars->solver                 = DEFAULT_SOLVER;
  pars->cplex_callbacks        = N;
  pars->number_of_nodes        = DEFAULT_NUMBER_OF_NODES;
  pars->random_instance_option = DEFAULT_RANDOM_INSTANCE_OPTION;
  pars->random_seed_option     = DEFAULT_RANDOM_SEED_OPTION;
  pars->seed                   = DEFAULT_SEED;
  pars->tsp_file_option        = DEFAULT_TSP_FILE_OPTION;
  pars->tsp_file               = DEFAULT_TSP_FILE;
  pars->tsp_file_format        = DEFAULT_TSP_FILE_FORMAT;
  pars->heuristic_trials       = DEFAULT_HEURISTIC_TRIALS;
  pars->heuristic_algo         = DEFAULT_HEURISTIC_ALGO;
  pars->threads                = NUM_OF_THREADS;
  pars->verbosity              = USEFUL;


  //pars->plot = 1;
  //pars->plotOnlyTree = 0;
  //pars->heuristic_trials = 1;

  FILE *parFile = fopen(FILE_CONFIG, "r");
  char  line[128];
  char *p1,
       *p2;
  int   lineLen;

  if (parFile != NULL) {

    //printf("@getParameters : beginning file scan\n");

    while( fgets(line, sizeof line, parFile) != NULL ){
      lineLen = strlen(line)-1;

      // skip empty lines
      if(lineLen == 0) continue;
      if(line[lineLen] == '\n') line[lineLen] = 0;

      // skip comments
      if(line[0] == '#') continue;

      // split and strip away the spaces
      // removing the spaces is necessary in order to use
      // the parHash() method above
      p1 = strtok(line, "= ");
      p2 = strtok(NULL, "= ");

      //printf("%s %s\n", p1, p2);

      if(p2 != NULL) {
        // manage correctly each parameter
        switch(parHash(p1)) {

          // number of nodes
          case 0 :
            //printf("@getParameters : case 0 : number_of_nodes\n");
            pars->number_of_nodes = atoi(p2);
            //printf("@getParameters : case 0 done\n");
            break;

          // random instance?
          case 1 :
            //printf("@getParameters : case 1 : random_instance_option\n");
            if (strcmp(p2, "TRUE") == 0)
              pars->random_instance_option = 1;
            else if (strcmp(p2, "FALSE") == 0)
              pars->random_instance_option = 0;
            //printf("@getParameters : case 1 done\n");
            break;

          // random seed or a fixed one?
          case 2 :
            //printf("@getParameters : case 2 : random_seed_option\n");
            if (strcmp(p2, "TRUE") == 0)
              pars->random_seed_option = 1;
            else if (strcmp(p2, "FALSE") == 0)
              pars->random_seed_option = 0;
            //printf("@getParameters : case 2 done\n");
            break;

          // seed value
          case 3 :
            //printf("@getParameters : case 3 : seed\n");
            if (pars->random_instance_option == 1 && pars->random_seed_option == 0)
              pars->seed = atoi(p2);
            //printf("@getParameters : case 3 done\n");
            break;

          // use a TSPLIB instance?
          case 4 :
            //printf("@getParameters : case 4 : tsp_file_option\n");
            if (strcmp(p2, "TRUE") == 0)
              pars->tsp_file_option = 1;
            else if (strcmp(p2, "FALSE") == 0)
              pars->tsp_file_option = 0;
            //printf("@getParameters : case 4 done\n");
            break;

          // TSPLIB instance file
          case 5 :
            //printf("@getParameters :: case 5 : tsp_file\n");
            if (pars->tsp_file_option == 1) {
              pars->tsp_file = malloc(sizeof(char) * strlen(p2));
              strcpy(pars->tsp_file, p2);
              //printf("@getParameters :: case 5 :: string copied\n");
            }
            //printf("@getParameters :: case 5 done\n");
            break;

          // number of iterations for the heuristic
          case 6 :
            //printf("@getParameters :: case 6 : heuristic_trials\n");
            pars->heuristic_trials = atoi(p2);
            //printf("@getParameters :: case 6 done\n");
            break;

          case 7 :
            if (strcmp(p2, "BRANCH_AND_BOUND") == 0) {
              pars->solver = BRANCH_AND_BOUND;
            } else if (strcmp(p2, "CPLEX") == 0) {
              pars->solver = CPLEX;
            }
            break;

          case 8 :
            if (strcmp(p2, "NEAREST_NEIGHBOUR") == 0) {
              pars->heuristic_algo = NEAREST_NEIGHBOUR;
            } else if (strcmp(p2, "NEAREST_NEIGHBOUR_2_OPT") == 0) {
              pars->solver = NEAREST_NEIGHBOUR_2_OPT;
            } else if (strcmp(p2, "RANDOM_CYCLES") == 0) {
              pars->solver = RANDOM_CYCLES;
            }

          default:
            //printf("@getParameters :: default\n");
            break;

        }
      }
    }

    //printf("@getParameters : closing (file) time\n");
    fclose(parFile);
    //printf("@getParameters : closing (file) time passed\n");
  }
  else {
    // errore nell'apertura del file
    perror( FILE_CONFIG );
  }

  //printf("@getParameters : exiting\n");
  return pars;
}



short tspHash(char *parName, char *parValue) {
    if (strcmp(parName, "NAME") == 0)
        return 0;

    if (strcmp(parName, "TYPE") == 0) {
        // only TYPE: TSP is allowed here
        if (strcmp(parValue, "TSP") == 0) {
            return 1;
        } else {
            return -1;
        }
    }

    if (strcmp(parName, "COMMENT") == 0)
        return 2;

    if (strcmp(parName, "DIMENSION") == 0)
        return 3;

    if (strcmp(parName, "EDGE_WEIGHT_TYPE") == 0) {
        // only these 4 kinds of 2D distances are considered
        if (strcmp(parValue, "EUC_2D") == 0) {
            return 41;
        } else if (strcmp(parValue, "MAN_2D") == 0) {
            return 42;
        } else if (strcmp(parValue, "CEIL_2D") == 0) {
            return 43;
        } else if (strcmp(parValue, "EXPLICIT") == 0) {
            return 44;
        } else if (strcmp(parValue, "GEO") == 0) {
            return 45;
        } else if (strcmp(parValue, "EUC_3D") == 0) {
            return 423;
        } else if (strcmp(parValue, "MAN_3D") == 0) {
            return 433;
        } else if (strcmp(parValue, "ATT") == 0) {
            return 48;
        } else {
            return -1;
        }
    }

    if (strcmp(parName, "EDGE_WEIGHT_FORMAT") == 0) {
      if (strcmp(parValue, "FULL_MATRIX") == 0) {
        return 46;
      } else if (strcmp(parValue, "LOWER_DIAG_ROW") == 0) {
        return 47;
      } else {
        return -1;
      }
    }

    if (strcmp(parName, "DISPLAY_DATA_TYPE") == 0) {
        if (strcmp(parValue, "COORD_DISPLAY") == 0) {
            return 51;
        } else {
            return -1;
        }
    }

    if (strcmp(parName, "NODE_COORD_SECTION") == 0)
        return 61;

    if (strcmp(parName, "DISPLAY_DATA_SECTION") == 0)
        return 62;

    if (strcmp(parName, "EDGE_WEIGHT_SECTION") == 0) {
      return 63;
    }

    return -1;
}



/*
 * read_tsp_from_file
 * - G : graph to be filled in
 * - ppars : parameters (some may be overridden)
 *
 * will override parameters, if needed
 *
 * Assumes file is formatted in the right way.
 * Does not guarantee correct working if this is not true.
 * Flexibility = 0!
 */
void read_tsp_from_file(egraph *G, parameters *pars) {

    //parameters *pars = (parameters *)ppars;
    //printf("reading tsp file %s\n", pars->tsp_file);
    FILE *tspFile = fopen(pars->tsp_file, "r");
    int   i = 0, j;
    char  line[1024];
    char *p1, *p2;
    int   lineLen;
    short haveCoords  = 1;
    short matrix_type = 0;

    egraph_delete(G);

    if (tspFile != NULL) {
        while(fgets(line, sizeof line, tspFile) != NULL) {
            lineLen = strlen(line)-1;

            // skip empty lines
            if(lineLen == 0) continue;
            if(line[lineLen] == '\n') line[lineLen] = 0;

            // skip comments
            if(line[0] == '#') continue;

            // split and strip away the spaces
            // removing the spaces is necessary in order to use
            // the parHash() method above
            p1 = strtok(line, ": ");
            p2 = strtok(NULL, ": ");

            // manage correctly each parameter
            switch(tspHash(p1, p2)) {

                // parName = 'NAME'
                case 0 : break;

                // (parName, parValue) = ('TYPE', 'TSP')
                case 1 : break;

                // parName = 'COMMENT'
                case 2 : break;

                // parName = 'DIMENSION'
                case 3 : pars->number_of_nodes = atoi(p2);
                         break;

                // 41 : (parName, parValue) = ('EDGE_WEIGHT_TYPE', 'EUC_2D')
                case 41: pars->tsp_file_format = 41;
                         break;
                // 42 : (parName, parValue) = ('EDGE_WEIGHT_TYPE', 'MAN_2D')
                case 42: pars->tsp_file_format = 42;
                         break;
                // 43 : (parName, parValue) = ('EDGE_WEIGHT_TYPE', 'CEIL_2D')
                case 43: pars->tsp_file_format = 43;
                         break;
                // 44 : (parName, parValue) = ('EDGE_WEIGHT_TYPE', 'EXPLICIT')
                case 44: pars->tsp_file_format = 44;
                         break;
                // 45 : (parName, parValue) = ('EDGE_WEIGHT_TYPE', 'GEO')
                case 45: pars->tsp_file_format = 45;
                         break;
                // 48 : (parName, parValue) = ('EDGE_WEIGHT_TYPE', 'ATT')
                case 48: pars->tsp_file_format = 48;
                         break;

                // 423 : (parName, parValue) = ('EDGE_WEIGHT_TYPE', 'EUC_3D')
                case 423: pars->tsp_file_format = 423;
                          break;
                // 433 : (parName, parValue) = ('EDGE_WEIGHT_TYPE', 'MAN_3D')
                case 433: pars->tsp_file_format = 433;
                          break;

                // (parName, parValue) = ('EDGE_WEIGHT_FORMAT', 'FULL_MATRIX')
                case 46: matrix_type = 46;
                         break;
                // (parName, parValue) = ('EDGE_WEIGHT_FORMAT', 'LOWER_DIAG_ROW')
                case 47: matrix_type = 47;
                         break;

                // (parName, parValue) = ('DISPLAY_DATA_TYPE', 'COORD_DISPLAY')
                case 51: break;

                // parName = 'NODE_COORD_SECTION'
                case 61:
                // parName = 'DISPLAY_DATA_SECTION'
                case 62:
                // parName = 'EDGE_WEIGHT_SECTION'
                case 63: if (matrix_type == 0) {
                             egraph_init(G, pars->number_of_nodes);

                             const char delimiters[] = " ";
                             while( fgets(line, sizeof line, tspFile) != NULL ) {
                                lineLen = strlen(line)-1;

                                // skip empty lines
                                if(lineLen == 0) continue;
                                if(line[lineLen] == '\n') line[lineLen] = 0;

                                // skip comments
                                if(line[0] == '#') continue;

                                if (strcmp(line, "EOF") == 0 ||
                                    strcmp(line, " EOF") == 0) {
                                    break;
                                }

                                char *token1, *token2, *token3;

                                token1 = strtok(line, delimiters);
                                token2 = strtok(NULL, delimiters);
                                token3 = strtok(NULL, delimiters);

                                // printf("%s %s %s | ", token1, token2, token3);
                                // printf("%d %f %f\n", atoi(token1), atof(token2), atof(token3));

                                j = atoi(token1)-1;
                                G->V[j].x = atof(token2);
                                G->V[j].y = atof(token3);

                             }
                             break;

                         } else if (matrix_type == 46) {

                             // full matrix
                             // each row contains pars->number_of_nodes elements
                             // (or, at least, it should...)
                             // since the assumption is NOT verified, we need
                             // to keep track of the count of read elements
                             // to manually split each row
                             egraph_init(G, pars->number_of_nodes);
                             int row = 0, cumulative_counter = 0, stop;
                             const char delimiters[] = " ";
                             while (fgets(line, sizeof(line), tspFile) != NULL) {
                                lineLen = strlen(line)-1;

                                // skip empty lines
                                if(lineLen == 0) continue;
                                if(line[lineLen] == '\n') line[lineLen] = 0;

                                // skip comments
                                if(line[0] == '#') continue;

                                if (strcmp(line, "EOF") == 0 ||
                                    strcmp(line, " EOF") == 0) {
                                    break;
                                }
                                char *tokens[pars->number_of_nodes], *tok;

                                tok = strtok(line, delimiters);

                                // very dirty
                                tokens[cumulative_counter] = tok;
                                cumulative_counter++;
                                stop = 0;
                                while (!stop) {
                                  tok = strtok(NULL, delimiters);
                                  if (tok == NULL) {
                                    stop = 1;
                                  } else {
                                    tokens[cumulative_counter] = tok;
                                    cumulative_counter++;
                                  }
                                }

                                if (cumulative_counter >= pars->number_of_nodes) {
                                  row++;
                                  cumulative_counter %= pars->number_of_nodes;

                                  for (j = 0; j < row; ++j) {
                                    if (row-1 != j && row <= pars->number_of_nodes &&
                                      j < pars->number_of_nodes) {
                                      egraph_insert_edge(G, row-1, j, atof(tokens[j]));
                                    }
                                  }
                                }

                             }

                             haveCoords = 0;
                             break;
                         } else if (matrix_type == 47) {
                             // lower diagonal row matrix
                             // things are (surprisingly) easier here,
                             // since we have to count the read elements,
                             // and manually create the rows.
                             egraph_init(G, pars->number_of_nodes);

                             int row = 0, pos = 0;
                             const char delimiters[] = " ";
                             while( fgets(line, sizeof line, tspFile) != NULL ) {

                                lineLen = strlen(line)-1;

                                // skip empty lines
                                if(lineLen == 0) continue;
                                if(line[lineLen] == '\n') line[lineLen] = 0;

                                // skip comments
                                if(line[0] == '#') continue;

                                if (strcmp(line, "EOF") == 0 ||
                                    strcmp(line, " EOF") == 0) {
                                    break;
                                }

                                char *tok;
                                tok = strtok(line, delimiters);

                                while (tok != NULL) {
                                  if (row != pos && row < pars->number_of_nodes &&
                                      pos < pars->number_of_nodes) {
                                      // <- eliminare i controlli row,pos<number_of_nodes ?
                                    egraph_insert_edge(G, row, pos, atof(tok));
                                  }
                                  pos++;
                                  if (atof(tok) == 0.0) {
                                    row++;
                                    pos = 0;
                                  }
                                  tok = strtok(NULL, delimiters);
                                }
                             }

                             haveCoords = 0;
                             break;
                         }

                default: break;
            }
        }
        fclose(tspFile);
    } else {
        perror(pars->tsp_file);
    }

    //for (i = 0; i < G->n; ++i) {
    //      printf("%f %f", egraph_get_node_x(G, i), egraph_get_node_y(G, i));
    //  }
    //  char cc = getchar();

    //printf("graph filled, exiting\n");

    double x, y, x2, y2;

    double min_x,
           max_x,
           min_y,
           max_y;


    if (haveCoords) {

      double q1, // intermediate values for the 'GEO' distances
             q2,
             q3,
             fract,
             latitude1  = 0.,
             longitude1 = 0.,
             latitude2  = 0.,
             longitude2 = 0.;

      // TSPLIB file contains node coordinates
      // for each edge, compute the distance (= the cost)
      min_x = G->V[0].x;
      max_x = min_x;
      min_y = G->V[0].y;
      max_y = min_y;


      for (i = 0; i < pars->number_of_nodes; i++) {

        x = G->V[i].x;
        y = G->V[i].y;

        if (pars->tsp_file_format == 45) {
          // 'GEO' distances
          fract      = x - (int)x;
          latitude1  = PI * ((int)x + 5.0 * fract / 3.0) / 180.0;
          fract      = y - (int)y;
          longitude1 = PI * ((int)y + 5.0 * fract / 3.0) / 180.0;
        }

        for (j = i+1; j < pars->number_of_nodes; j++) {

          x2 = G->V[j].x;
          y2 = G->V[j].y;

          // choose right function to compute distance
          if (pars->tsp_file_format == 41) {

            // euclidean 2D distance, or boh
            egraph_insert_edge(G, i, j,
             //nearbyint(sqrt(pow(x - x2, 2) + pow(y - y2, 2)))
             round(sqrt(pow(x - x2, 2) + pow(y - y2, 2)))
            );

          } else if (pars->tsp_file_format == 45) {

            // 'GEO' distances
            fract      = x2 -  (int)x2;
            latitude2  = PI * ((int)x2 + 5.0 * fract / 3.0) / 180.0;
            fract      = y2 -  (int)y2;
            longitude2 = PI * ((int)y2 + 5.0 * fract / 3.0) / 180.0;

            q1 = cosf(longitude1 - longitude2);
            q2 = cosf(latitude1  - latitude2);
            q3 = cosf(latitude1  + latitude2);

            egraph_insert_edge(G, i, j,
                (int)(EARTH_RADIUS * acosf(0.5*((1.0+q1)*q2 - (1.0-q1)*q3) ) + 1.0)
            );

          // end else-if 'GEO'
          } else if (pars->tsp_file_format == 42) {

            // manhattan distance 2D
            egraph_insert_edge(G, i, j,
              (int)(fabs(x - x2) + fabs(y - y2))
            );

          } else if (pars->tsp_file_format == 43) {

            // ceil 2D
            egraph_insert_edge(G, i, j,
              ceil(sqrt(pow(x - x2, 2) + pow(y - y2, 2)))
            );

          } else if (pars->tsp_file_format == 48) {

            // att pseudo-Euclidean
            double xd  = x - x2,
                   yd  = y - y2,
                   rij = sqrt((xd*xd + yd*yd) / 10.0),
                   tij = round(rij); //nearbyint

            if (tij < rij) {
              tij = tij + 1;
            }

            egraph_insert_edge(G, i, j, tij);

          } else if (pars->tsp_file_format == 423 ||
                     pars->tsp_file_format == 433   ) {

            fprintf(stderr, "HOUSTON... 3D, we don't cover this...\n");
            exit(1);

          }


        } // end for j

        // look for updated bounds
        if (x < min_x) {
          min_x = x;
        } else if (x > max_x) {
          max_x = x;
        }

        if (y < min_y) {
          min_y = y;
        } else if (y > max_y) {
          max_y = y;
        }
      }
    } else {
      // TSPLIB instance has the cost matrix
      // we need to compute the nodes coords
      // found at http://stackoverflow.com/questions/10963054/finding-the-coordinates-of-points-from-distance-matrix
      // hope it works, since there is probably something I haven't understood yet...

      double Dp1p2 = egraph_get_edge_cost(G, 0, 1),  // 1-2
             Dp1p3,                                  // 1-3
             Dp2p3,                                  // 2-3
             Dp1pn,                                  // 1-n, n >= 4
             Dp2pn,                                  // 2-n, n >= 4
             tentative_cost_1,
             tentative_cost_2,
             cosine,
             angle;

      double x1 = 0.,
             y1 = 0.,
             x2 = Dp1p2,
             y2 = 0.,
             x3,
             y3;

      // set first node at (0,0)
      G->V[0].x = x1;
      G->V[0].y = y1;

      // set node 2 on the same horz. line, at proper distance
      G->V[1].x = x2;
      G->V[1].y = y2;

      // for padding
      min_x = 0.;
      max_x = Dp1p2 + 1;
      min_y = 0.;
      max_y = 2.;

      // set third node coords, in order to build the orthonormal space
      Dp1p3 = egraph_get_edge_cost(G, 0, 2);
      Dp2p3 = egraph_get_edge_cost(G, 1, 2);

      cosine = (Dp1p2*Dp1p2 + Dp1p3*Dp1p3 - Dp2p3*Dp2p3) / (2*Dp1p2*Dp1p3);
      angle  = acosf(cosine);
      x3 = cosine * Dp1p3;
      y3 = sinf(angle) * Dp1p3;

      G->V[2].x = x3;
      G->V[2].y = y3;

      // iterate brute-force step until all nodes have been somewhat correctly given
      // their coords - or some values sufficiently near to them.

      for (i = 3; i < pars->number_of_nodes; ++i) {

        Dp1pn = egraph_get_edge_cost(G, 0, i);
        Dp2pn = egraph_get_edge_cost(G, 1, i);

        cosine = (Dp1p2*Dp1p2 + Dp1pn*Dp1pn - Dp2pn*Dp2pn) / (2*Dp1p2*Dp1pn);
        angle  = acosf(cosine);
        x = cosine * Dp1pn;
        y = sinf(angle) * Dp1pn;

        G->V[i].x = x;

        tentative_cost_1 = sqrt((x-x3)*(x-x3) + (y-y3)*(y-y3));
        tentative_cost_2 = sqrt((x-x3)*(x-x3) + (-y-y3)*(-y-y3));

        if (fabs(tentative_cost_1 - egraph_get_edge_cost(G, 2, i)) > 
            fabs(tentative_cost_2 - egraph_get_edge_cost(G, 2, i))) {
          y = -y;
        }
        G->V[i].y = y;

        if (x < min_x) {
          min_x = x;
        } else if (x > max_x) {
          max_x = x;
        }

        if (y < min_y) {
          min_y = y;
        } else if (y > max_y) {
          max_y = y;
        }

      }

    } // end if-else

    // set bounds and add some padding
    G->min_x = min_x - (max_x - min_x) / 50.;
    G->max_x = max_x + (max_x - min_x) / 50.;
    G->min_y = min_y - (max_y - min_y) / 50.;
    G->max_y = max_y + (max_y - min_y) / 50.;

#ifdef DEBUG
    printf("area bounds\n");
    printf("%f %f %f %f\n", min_x, max_x, min_y, max_y);
    printf("%f %f %f %f\n", G->min_x, G->max_x, G->min_y, G->max_y);

    egraph_print(G);
#endif

}



/*
 * snbdComp
 *const void * a,b : the two elements to be compared
 *
 * comparison function to be passed to qsort()
 *
 * return: -1,0,1 depending on the comparison of the two elements
 */
//int snbdComp(const void *aa, const void *bb) {
/*int snbdComp(node *a, node *b) {
    / *node a = aa,
         b = bb;* /

    if (a->deg < b->deg) { return -1; }
    if (a->deg > b->deg) { return 1; }
    return 0;
}*/

// sort edges by weight
int sebwComp (const void * a, const void * b) {
  if (*(double*)a > *(double*)b) return 1;
  else if (*(double*)a < *(double*)b) return -1;
  else return 0;
}


/*
 * free_and_null
 * - char ** : pointer to the list
 *
 * empty a char** and set the pointer to NULL
 */
void free_and_null(char **ptr) {
  if (*ptr != NULL) {
    free(*ptr);
    *ptr = NULL;
  }
} // END free_and_null

/**
 * compute difference in seconds between two clock_t times
 * @param  start start time
 * @param  end   end time
 * @return       seconds passed between start and end
 */
inline double time_elapsed(clock_t start, clock_t end) {
  return (double) (end - start) / CLOCKS_PER_SEC;
}
