#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "days.h"
#include "log.h"
#include "util.h"

typedef struct monkey_struct {
    /* what items monkey has */
    uint64_t *items;
    /* current number of items */
    int item_cnt;
    /* max number of items list can store */
    int item_max;
    /* what kind of operation (+, *) */
    char oper;
#define OPER_AMNT_OLD (INT32_MAX)
    /* RHS of the operation */
    int oper_amnt;
    /* divisibility test */
    int div_test;
    /* index of monkey to throw to on true */
    int true_throw;
    /* index of monkey to throw to on false */
    int false_throw;
    /* number of times monkey inspected an item */
    uint64_t inspect_count;
} monkey_t;

monkey_t *monkeys = NULL;
int n_monkeys = 0;
uint64_t monkey_mod = 1;
static bool star2 = false;

static app_error_e rd_monkey(app_t *app);
static app_error_e do_round();
static void print_monkey(int idx);

app_error_e day11(app_t *app)
{
    app_error_e ret;
    uint64_t n1 = 0;
    uint64_t n2 = 0;
    int rounds = app->star2 ? 10000 : 20;

    star2 = app->star2;

    log_debug("Init Monkeys\n");
    do {
        ret = rd_monkey(app);
    } while (ret == APP_ERROR_OK);

    if (ret != APP_ERROR_EOF) {
        return ret;
    }

    for (int i = 0; i < rounds; i++) {
        ret = do_round();
        if (ret) {
            return ret;
        }
    }

    for (int i = 0; i < n_monkeys; i++) {
        uint32_t cnt = monkeys[i].inspect_count;
        log_print("Monkey %d inspected items %d times\n", i, cnt);
        
        if (cnt > n1) {
            n2 = n1;
            n1 = cnt;
        } else if (cnt > n2) {
            n2 = cnt;
        }
    }

    log_print("Result (%lu*%lu): %lu\n", n1, n2, n1*n2);

    return APP_ERROR_OK;
}

static app_error_e rd_monkey(app_t *app)
{
    char line[64];
    char *linep;
    size_t line_len;
    monkey_t *monkey;

    if (fgets(line, sizeof(line), app->input) == NULL)
        return APP_ERROR_EOF;

    n_monkeys++;
    monkeys = realloc(monkeys, sizeof(*monkeys) * n_monkeys);
    monkey = &monkeys[n_monkeys-1];
    memset(monkey, 0, sizeof(*monkey));



    /* "  Starting items: <num>, <num>, ..." */
    if (fgets(line, sizeof(line), app->input) == NULL)
        return APP_ERROR_INVALID;
    line_len = util_strip(line);
    if (line_len < 19) {
        log_error("Invalid starting items: \"%s\"", line);
        return APP_ERROR_INVALID;
    }
    linep = &line[18];
    do {
        char *str;

        if ((*linep) == ',') {
            str = linep + 1;
        } else {
            str = linep;
        }

        monkey->item_max++;
        monkey->item_cnt++;
        monkey->items = realloc(
            monkey->items,
            sizeof(*monkey->items) * monkey->item_max
        );
        monkey->items[monkey->item_max-1] = strtoll(str, &linep, 10);
    } while ((*linep) == ',');

    /* "  Operation: new = old <op> <num or old>" */
    if (fgets(line, sizeof(line), app->input) == NULL)
        return APP_ERROR_INVALID;
    line_len = util_strip(line);
    if (line_len < 26) {
        log_error("Invalid operation: \"%s\"", line);
        return APP_ERROR_INVALID;
    }
    monkey->oper = line[23];

    if (!strcmp(&line[25], "old")) {
        monkey->oper_amnt = OPER_AMNT_OLD;
    } else {
        monkey->oper_amnt = atoi(&line[25]);
    }

    /* "  Test: divisible by <num>" */
    if (fgets(line, sizeof(line), app->input) == NULL)
        return APP_ERROR_INVALID;
    line_len = util_strip(line);
    if (line_len < 22) {
        log_error("Invalid div test: \"%s\"", line);
        return APP_ERROR_INVALID;
    }
    monkey->div_test = atoi(&line[21]);

    /* "    If true: throw to monkey <num>"*/
    if (fgets(line, sizeof(line), app->input) == NULL)
        return APP_ERROR_INVALID;
    line_len = util_strip(line);
    if (line_len < 30) {
        log_error("Invalid true throw: \"%s\"", line);
        return APP_ERROR_INVALID;
    }
    monkey->true_throw = atoi(&line[29]);

    /* "    If false: throw to monkey <num>"*/
    if (fgets(line, sizeof(line), app->input) == NULL)
        return APP_ERROR_INVALID;
    line_len = util_strip(line);
    if (line_len < 31) {
        log_error("Invalid false throw: \"%s\"", line);
        return APP_ERROR_INVALID;
    }
    monkey->false_throw = atoi(&line[30]);

    print_monkey(n_monkeys-1);

    monkey_mod *= monkey->div_test;

    /* consume empty line */
    if (fgets(line, sizeof(line), app->input) == NULL)
        return APP_ERROR_EOF;

    return APP_ERROR_OK;
}



static app_error_e do_round()
{
    static int round_cnt = 0;

    round_cnt++;
    log_debug("\nRound %d\n", round_cnt);

    for (int i = 0; i < n_monkeys; i++) {
        monkey_t *monkey = &monkeys[i];
        print_monkey(i);

        for (int j = 0; j < monkey->item_cnt; j++) {
            int amnt = monkey->oper_amnt;
            monkey_t *target;
            int target_idx;

            if (monkey->oper_amnt == OPER_AMNT_OLD)
                amnt = monkey->items[j];

            log_debug("Inspect item %ld\n", monkey->items[j]);
            /* inspect */
            switch (monkey->oper) {
            case '+':
                monkey->items[j] += amnt;
                break;
            case '*':
                monkey->items[j] *= amnt;
                break;
            default:
                log_error("Invalid operation \"%c\"\n", monkey->oper);
                return APP_ERROR_INVALID;
            }

            if (!star2)
                monkey->items[j] /= 3;

            /* all math can be done modulo the division test of LCM all monkeys */
            monkey->items[j] = monkey->items[j] % monkey_mod;

            monkey->inspect_count++;
            log_debug("New level %ld\n", monkey->items[j]);

            /* test n throw */
            if (monkey->items[j] % monkey->div_test == 0) {
                target_idx = monkey->true_throw;
            } else {
                target_idx = monkey->false_throw;
            }

            if (target_idx >= n_monkeys || target_idx == i) {
                log_error("Invalid monkey index %d\n", target_idx);
                return APP_ERROR_INVALID;
            }

            target = &monkeys[target_idx];
            if (target->item_max == target->item_cnt) {
                target->item_max++;
                target->items = realloc(
                    target->items,
                    sizeof(*target->items) * target->item_max
                );
            }

            log_debug(
                "Throw from monkey %d to %d\n",
                i, target_idx
            );
            target->items[target->item_cnt] = monkey->items[j];
            target->item_cnt++;
        }

        monkey->item_cnt = 0;
    }

    return APP_ERROR_OK;
}



static void
print_monkey(int idx)
{
    monkey_t *monkey = &monkeys[idx];
    log_debug("Monkey %d\n", idx);
    log_debug("  Items: ");
    if (log_get_debug()) {
        for (int i = 0; i < monkey->item_cnt; i++) {
                printf("%ld, ", monkey->items[i]);
        }
        printf("\n");
    }
    if (monkey->oper_amnt == OPER_AMNT_OLD) {
        log_debug(
            "  Operation: new = old %c old\n",
            monkey->oper
        );
    } else {
        log_debug(
            "  Operation: new = old %c %d\n",
            monkey->oper, monkey->oper_amnt
        );
    }
    log_debug("  Test: divisible by %d\n", monkey->div_test);
    log_debug("    If true: throw to monkey %d\n", monkey->true_throw);
    log_debug("    If false: throw to monkey %d\n", monkey->false_throw);
    log_debug("  Inspect count: %lu\n", monkey->inspect_count);
}
