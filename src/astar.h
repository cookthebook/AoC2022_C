#include <stdint.h>

typedef struct astar_node_struct {
    /* indeces of the neighbor nodes of this node */
    int *nbrs;
    /* weights of the edges between this node and the neighbors */
    uint64_t *weights;
    int n_nbrs;
    /* user data */
    void *data;
} astar_node_t;

typedef struct astar_struct {
    /* nodes in the graph */
    astar_node_t *nodes;
    int n_nodes;
    /* index of the start node */
    int start;
    /* index of the end node */
    int end;
    /* heuristic for estimate cost of cheapest path of target node index to end */
    uint64_t (*h)(struct astar_struct *, int);
} astar_t;

/* Returns a -1 terminated list of the shortest node index path found
 * reference: https://en.wikipedia.org/wiki/A*_search_algorithm#Pseudocode */
int *astar_search(astar_t *astar);
