#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "days.h"
#include "app.h"
#include "astar.h"
#include "util.h"
#include "log.h"

static uint64_t astar_h(astar_t *astar, int idx);

static int row_len = -1;

app_error_e day12(app_t *app)
{
    uint8_t *heights = NULL;
    int n_heights = 0;
    char line[256];
    astar_t astar = { 0 };
    int *path;
    int path_len = 0;

    while (fgets(line, sizeof(line), app->input)) {
        size_t line_len = util_strip(line);

        if (row_len < 0) {
            row_len = line_len;
        } else if (row_len != line_len) {
            log_error("Invalid row \"%s\"\n", line);
            return APP_ERROR_INVALID;
        }

        heights = realloc(heights, sizeof(uint8_t) * (n_heights + row_len));
        for (int i = n_heights; i < (n_heights + row_len); i++) {
            uint8_t height;
            int p = i - n_heights;

            if (line[p] == 'S') {
                height = 0;
                astar.start = i;
            }
            else if (line[p] == 'E') {
                height = 25;
                astar.end = i;
            }
            else if (line[p] >= 'a' && line[p] <= 'z') {
                height = line[p] - 'a';
            }
            else {
                log_error("Invalid height \"%c\"\n", line[p]);
                return APP_ERROR_INVALID;
            }

            heights[i] = height;
        }
        n_heights += row_len;
    }

    if (log_get_debug()) {
        log_debug("Grid size: %d\n", n_heights);
        log_debug("Row length: %d\n", row_len);
        log_debug("Col length: %d\n", n_heights/row_len);
        log_debug("Map:\n");

        for (int r = 0; r < (n_heights/row_len); r++) {
            for (int c = 0; c < row_len; c++) {
                int idx = r*row_len + c;

                if (idx == astar.start) {
                    log_print("S");
                } else if (idx == astar.end) {
                    log_print("E");
                } else {
                    log_print("%c", 'a' + heights[idx]);
                }
            }
            log_print("\n");
        }
    }

    /* construct our astar nodes */
    astar.h = astar_h;
    astar.n_nodes = n_heights;
    astar.nodes = calloc(astar.n_nodes, sizeof(*astar.nodes));

    int r_max = n_heights/row_len;
    int c_max = row_len;
    for (int r = 0; r < r_max; r++) {
    for (int c = 0; c < c_max; c++) {
        int idx = r*row_len + c;
        astar_node_t *node = &astar.nodes[idx];
        int h = heights[idx];

        node->nbrs = malloc(sizeof(int)*4);
        node->weights = malloc(sizeof(uint64_t)*4);

        /* top neighbor */
        if (r > 0 && heights[idx - row_len] <= (h + 1)) {
            node->nbrs[node->n_nbrs] = idx - row_len;
            node->weights[node->n_nbrs] = 1;
            node->n_nbrs++;
        }

        /* bottom neighbor */
        if (r < (r_max - 1) && heights[idx + row_len] <= (h + 1)) {
            node->nbrs[node->n_nbrs] = idx + row_len;
            node->weights[node->n_nbrs] = 1;
            node->n_nbrs++;
        }

        /* left neighbor */
        if (c > 0 && heights[idx-1] <= (h + 1)) {
            node->nbrs[node->n_nbrs] = idx - 1;
            node->weights[node->n_nbrs] = 1;
            node->n_nbrs++;
        }

        /* right neighbor */
        if (c < (c_max-1) && heights[idx + 1] <= (h + 1)) {
            node->nbrs[node->n_nbrs] = idx + 1;
            node->weights[node->n_nbrs] = 1;
            node->n_nbrs++;
        }
    }
    }

    if (app->star2) {
        path_len = INT32_MAX;

        for (int i = 0; i < n_heights; i++) {
            int cur_len = 0;
            if (heights[i] == 0) {
                astar.start = i;
            } else {
                continue;
            }

            path = astar_search(&astar);
            if (!path)
                continue;

            for (cur_len = 0; path[cur_len] != -1; cur_len++);

            if (cur_len < path_len) {
                path_len = cur_len;
            }
        }

        log_print("Shortest path length from height 0: %d\n", path_len);
    } else {
        path = astar_search(&astar);
        if (!path) {
            log_error("Failed to find a path\n");
            return APP_ERROR_CRITICAL;
        }

        log_debug("Shortest path: ");
        for (path_len = 0; path[path_len] != -1; path_len++) {
            if (log_get_debug()) {
                printf("%d, ", path[path_len]);
            }
        }
        if (log_get_debug()) {
            printf("\n");
        }

        log_print("Best path length: %d\n", path_len);
    }
    return APP_ERROR_OK;
}

static uint64_t astar_h(astar_t *astar, int idx)
{
    int row = idx / row_len;
    int col = idx % row_len;
    int erow = astar->end / row_len;
    int ecol = astar->end % row_len;

    int drow = erow - row;
    int dcol = ecol - col;

    drow = drow < 0 ? -drow : drow;
    dcol = dcol < 0 ? -dcol : dcol;

    return dcol + drow;
}
