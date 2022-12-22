#include <stdlib.h>
#include <stdbool.h>

#include "astar.h"

static int *reconstruct(astar_t *astar, int *came_from);

int *
astar_search(astar_t *astar)
{
    /* set of discovered nodes that may need to be expanded */
    int *open_set = malloc(sizeof(int));
    open_set[0] = astar->start;
    int open_set_n = 1;

    /* came_from[n] = previous node on cheapest path from start */
    int *came_from = malloc(sizeof(int) * astar->n_nodes);
    for (int i = 0; i < astar->n_nodes; i++) {
        came_from[i] = -1;
    }

    /* gsocres[n] = cost of cheapest path from start to n currently known */
    uint64_t *gscores = malloc(sizeof(uint64_t) * astar->n_nodes);
    for (int i = 0; i < astar->n_nodes; i++) {
        gscores[i] = UINT64_MAX;
    }
    gscores[astar->start] = 0;

    /* fscores[n] = gscore[n] + h(n), best guess as to how cheap path from start
     * to finish could be if traveled through n */
    uint64_t *fscores = malloc(sizeof(uint64_t) * astar->n_nodes);
    for (int i = 0; i < astar->n_nodes; i++) {
        fscores[i] = UINT64_MAX;
    }
    fscores[astar->start] = astar->h(astar, astar->start);

    while (open_set_n > 0) {
        /* node in open set with lowest fscore */
        astar_node_t *cur;
        int cur_idx;
        int open_set_idx = 0;
        for (int i = 1; i < open_set_n; i++) {
            if (fscores[open_set[i]] < fscores[open_set[open_set_idx]]) {
                open_set_idx = i;
            }
        }
        cur_idx = open_set[open_set_idx];
        cur = &astar->nodes[cur_idx];

        if (cur_idx == astar->end) {
            return reconstruct(astar, came_from);
        }

        /* remove current node */
        for (int i = open_set_idx; i < (open_set_n-1); i++) {
            open_set[i] = open_set[i+1];
        }
        open_set_n--;
        if (open_set_n > 0) {
            open_set = realloc(open_set, sizeof(int) * open_set_n);
        } else {
            open_set = NULL;
        }

        for (int i = 0; i < cur->n_nbrs; i++) {
            int nbr = cur->nbrs[i];
            uint64_t tmp_gscore = gscores[cur_idx];
            if (tmp_gscore != UINT64_MAX)
                tmp_gscore += cur->weights[i];

            if (tmp_gscore < gscores[nbr]) {
                bool in_openset = false;

                came_from[nbr] = cur_idx;
                gscores[nbr] = tmp_gscore;
                fscores[nbr] = tmp_gscore + astar->h(astar, nbr);

                for (int j = 0; j < open_set_n; j++) {
                    if (open_set[j] == nbr) {
                        in_openset = true;
                        break;
                    }
                }

                if (!in_openset) {
                    open_set_n++;
                    open_set = realloc(open_set, sizeof(int) * open_set_n);
                    open_set[open_set_n-1] = nbr;
                }
            }
        }
    }

    return NULL;
}

static int *
reconstruct(astar_t *astar, int *came_from)
{
    int *ret = NULL;
    int ret_n = 0;
    int cur_idx = astar->end;

    while (came_from[cur_idx] != -1) {
        ret_n++;
        ret = realloc(ret, sizeof(int) * ret_n);
        ret[ret_n-1] = cur_idx;
        cur_idx = came_from[cur_idx];
    }

    /* reverse */
    for (int i = 0; i < (ret_n/2); i++) {
        int tmp = ret[i];
        ret[i] = ret[ret_n - 1 - i];
        ret[ret_n - 1 - i] = tmp;
    }

    ret = realloc(ret, sizeof(int) * (ret_n+1));
    ret[ret_n] = -1;

    return ret;
}
