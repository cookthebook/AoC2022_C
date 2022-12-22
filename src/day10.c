#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "days.h"
#include "log.h"
#include "util.h"

typedef struct cpu_struct {
    int32_t x;
    uint32_t cycles;
} cpu_t;

static app_error_e star1(app_t *app);
static app_error_e star2(app_t *app);
static app_error_e proc_ln(char *line);

static cpu_t cpu = { .x = 1, .cycles = 1 };

app_error_e day10(app_t *app) {
    if (app->star2) {
        return star2(app);
    } else {
        return star1(app);
    }
}

static app_error_e
star1(app_t *app)
{
    int32_t *strengths = NULL;
    int n_strengths = 0;
    char line[16];
    int64_t result = 0;

    while (fgets(line, sizeof(line), app->input)) {
        cpu_t cpu_tmp;
        int32_t *str = NULL;
        app_error_e ret;

        memcpy(&cpu_tmp, &cpu, sizeof(cpu));
        ret = proc_ln(line);
        if (ret) {
            return ret;
        }

        /* record strength on cycle 20, 60, 100, etc. */
        if (cpu.cycles % 40 == 20) {
            str = &cpu.x;
        }
        else if ((cpu_tmp.cycles % 40 < 20) && (cpu.cycles % 40 > 20)) {
            str = &cpu_tmp.x;
        }

        if (str) {
            n_strengths++;
            strengths = realloc(strengths, sizeof(*strengths)*n_strengths);
            strengths[n_strengths-1] = (*str);
        }
    }

    for (int i = 0; i < n_strengths; i++) {
        uint32_t cycle = 20 + (i*40);
        result += strengths[i] * cycle;
        log_debug(
            "Add strength: %d*%d, new total: %ld\n",
            strengths[i], cycle, result
        );
    }

    log_print("Sum of %d strengths: %ld\n", n_strengths, result);
    return APP_ERROR_OK;
}

static app_error_e star2(app_t *app)
{
    char line[16];
    int cycle = 1;
    int pos;

    for (int v = 0; v < 6; v++) {
        for (int h = 0; h < 40; h++) {
            if (cycle > cpu.cycles) {
                fgets(line, sizeof(line), app->input);
                int ret = proc_ln(line);
                if (ret) {
                    return ret;
                }
            }

            if (cycle == cpu.cycles) {
                pos = cpu.x;
            }

            if (pos >= (h-1) && pos <= (h+1)) {
                log_print("\u2588"); /* solid box */
            } else {
                log_print(" ");
            }

            cycle++;
        }

        log_print("\n");
    }

    return APP_ERROR_OK;
}

static app_error_e proc_ln(char *line)
{
    size_t len = util_strip(line);

    if (len < 4) {
        goto line_err;
    }

    if (!strcmp(line, "noop")) {
        cpu.cycles++;
        log_debug("noop\n");
    }
    else if (len > 5 && !memcmp(line, "addx ", 5)) {
        int val = atoi(&line[5]);
        cpu.x += val;
        cpu.cycles += 2;
        log_debug("addx %d\n", val);
    } else {
        goto line_err;
    }

    return APP_ERROR_OK;

line_err:
    log_error("Invalid line \"%s\"\n", line);
    return APP_ERROR_INVALID;
}
