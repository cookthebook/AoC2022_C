#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "days.h"
#include "log.h"
#include "util.h"

typedef struct list_struct {
    struct list_item {
        uint8_t num;
        struct list_struct *lst;
    } *items;
    int *lens;
    int cnt;
} list_t;

static app_error_e list_from_str(list_t *lst, char *str, char **end_ptr);
static void list_newitem(list_t *lst);
static void list_print(const list_t *lst);
static int list_cmp(const list_t *rhs, const list_t *lhs);
static int list_cmp_qsort(const void *rhs, const void *lhs);

app_error_e day13(app_t *app)
{
    char line[1024];
    list_t *packets = NULL;
    int packets_n = 0;
    app_error_e ret;
    int result = 0;

    log_debug("List item size: %ld\n", sizeof(*packets->items));

    while (fgets(line, sizeof(line), app->input)) {
        size_t line_len = util_strip(line);

        /* ignore */
        if (line_len == 0)
            continue;

        packets_n++;
        packets = realloc(packets, sizeof(*packets) * packets_n);
        memset(&packets[packets_n-1], 0, sizeof(list_t));
        ret = list_from_str(&packets[packets_n-1], line, NULL);
        if (ret) {
            return ret;
        }

        log_debug("Parsed list:\n");
        list_print(&packets[packets_n-1]);
    }

    if (packets_n % 2 != 0) {
        log_error("Odd number of packets\n");
        return APP_ERROR_INVALID;
    }

    if (app->star2) {
        int d1_idx = 0;
        int d2_idx = 0;

        /* insert extra distress packets and sort */
        packets_n += 2;
        packets = realloc(packets, sizeof(*packets)*packets_n);
        packets[packets_n-2].cnt = 1;
        packets[packets_n-2].lens = (int []){ 1 };
        packets[packets_n-2].items = (struct list_item []) {
            {
                .num = 0,
                .lst = (struct list_struct []) {{
                    .items = (struct list_item []) {{ .num = 2, .lst = NULL }},
                    .lens = NULL,
                    .cnt = 1
                }}
            }
        };
        packets[packets_n-1].cnt = 1;
        packets[packets_n-1].lens = (int []){ 1 };
        packets[packets_n-1].items = (struct list_item []) {
            {
                .num = 0,
                .lst = (struct list_struct []) {{
                    .items = (struct list_item []) {{ .num = 6, .lst = NULL }},
                    .lens = NULL,
                    .cnt = 1
                }}
            }
        };

        qsort(packets, packets_n, sizeof(*packets), list_cmp_qsort);
        log_debug("Sorted packets:\r\n");
        for (int i = 0; i < packets_n; i++) {
            list_print(&packets[i]);
            if (
                d1_idx == 0 &&
                packets[i].cnt == 1 &&
                packets[i].items[0].lst &&
                packets[i].items[0].lst->cnt == 1 &&
                packets[i].items[0].lst->items[0].num == 2
            ) {
                d1_idx = i+1;
                log_debug("Found distress signal 1 at %d\n", i);
            }
            else if (
                d2_idx == 0 &&
                packets[i].cnt == 1 &&
                packets[i].items[0].lst &&
                packets[i].items[0].lst->cnt == 1 &&
                packets[i].items[0].lst->items[0].num == 6
            ) {
                d2_idx = i+1;
                log_debug("Found distress signal 2 at %d\n", i);
            }
        }

        log_print("Result (%d*%d) = %d\n", d1_idx, d2_idx, d1_idx*d2_idx);
    } else {
        for (int i = 0; i < packets_n; i += 2) {
            if (list_cmp(&packets[i], &packets[i+1]) < 0) {
                result += (i/2) + 1;
                log_debug("Pair in order:\n");
                list_print(&packets[i]);
                list_print(&packets[i+1]);
            } else {
                list_t tmp;
                memcpy(&tmp, &packets[i], sizeof(list_t));
                memcpy(&packets[i], &packets[i+1], sizeof(tmp));
                memcpy(&packets[i+1], &tmp, sizeof(list_t));
            }
        }
        log_print("Result = %d\n", result);
    }

    return APP_ERROR_OK;
}

static app_error_e list_from_str(list_t *lst, char *str, char **end_ptr)
{
    char *strp = str;

    if (*strp != '[') {
        log_error("Invalid list \"%s\"\n", str);
    }
    strp++;

    while (1) {
        if (*strp == ']') {
            strp++;
            break;
        } else if (*strp == '\0') {
            log_error("Unclosed list\n");
            return APP_ERROR_INVALID;
        }

        if (*strp == '[') {
            int ret;

            list_newitem(lst);
            lst->items[lst->cnt-1].lst = calloc(1, sizeof(list_t));
            ret = list_from_str(lst->items[lst->cnt-1].lst, strp, &strp);
            if (ret) {
                return ret;
            }
            lst->lens[lst->cnt-1] = lst->items[lst->cnt-1].lst->cnt;
        }
        else if (*strp >= '0' && *strp <= '9') {
            char *end_ptr;
            long val = strtol(strp, &end_ptr, 10);
            strp = end_ptr;

            if (val > UINT8_MAX || val < 0) {
                log_error("Invalid list value %ld\n", val);
                return APP_ERROR_INVALID;
            }

            list_newitem(lst);
            lst->items[lst->cnt-1].num = val;
            lst->lens[lst->cnt-1] = 1;
        } else {
            log_error("Invalid list value: \"%s\"\n", strp);
            return APP_ERROR_INVALID;
        }

        if (*strp == ',')
            strp++;
    }

    if (end_ptr) {
        *end_ptr = strp;
    }

    return APP_ERROR_OK;
}

static void list_newitem(list_t *lst)
{
    lst->cnt++;
    lst->items = realloc(lst->items, sizeof(*lst->items) * lst->cnt);
    memset(&lst->items[lst->cnt-1], 0, sizeof(*lst->items));
    lst->lens = realloc(lst->lens, sizeof(*lst->lens) * lst->cnt);
    lst->lens[lst->cnt-1] = 0;
}

static void __list_print_recur(const list_t *lst)
{
    log_print("[");
    for (int i = 0; i < lst->cnt; i++) {
        if (lst->items[i].lst) {
            __list_print_recur(lst->items[i].lst);
        } else {
            log_print("%d,", lst->items[i].num);
        }
    }
    log_print("]");
}
static void list_print(const list_t *lst)
{
    if (!log_get_debug())
        return;

    log_debug("");
    __list_print_recur(lst);
    log_print("\n");
}

static int list_cmp(const list_t *lhs, const list_t *rhs)
{
    int min = rhs->cnt < lhs->cnt ? rhs->cnt : lhs->cnt;

    log_debug("Compare lists:\n");
    list_print(lhs);
    list_print(rhs);

    for (int i = 0; i < min; i++) {
        int val;
        list_t tmp;

        if (lhs->items[i].lst && rhs->items[i].lst) {
            val = list_cmp(lhs->items[i].lst, rhs->items[i].lst);
        }
        else if (rhs->items[i].lst) {
            tmp.cnt = 1;
            tmp.items = calloc(1, sizeof(*tmp.items));
            tmp.items[0].num = lhs->items[i].num;
            val = list_cmp(&tmp, rhs->items[i].lst);
            free(tmp.items);
        }
        else if (lhs->items[i].lst) {
            tmp.cnt = 1;
            tmp.items = calloc(1, sizeof(*tmp.items));
            tmp.items[0].num = rhs->items[i].num;
            val = list_cmp(lhs->items[i].lst, &tmp);
            free(tmp.items);
        }
        else {
            if (lhs->items[i].num < rhs->items[i].num) {
                val = -1;
            } else if (lhs->items[i].num > rhs->items[i].num) {
                val = 1;
            } else {
                val = 0;
            }
        }

        if (val != 0)
            return val;
    }

    if (lhs->cnt == rhs->cnt) {
        return 0;
    } else if (lhs->cnt < rhs->cnt) {
        return -1;
    } else {
        return 1;
    }
}

static int list_cmp_qsort(const void *rhs, const void *lhs)
{
    return list_cmp((list_t *)rhs, (list_t *)lhs);
}
