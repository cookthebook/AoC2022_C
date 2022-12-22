#include <stdio.h>
#include "log.h"

static bool debug = false;

void log_set_debug(bool _debug)
{
    debug = _debug;
}

bool log_get_debug()
{
    return debug;
}

int log_print(const char *fmt, ...)
{
    va_list ap;
    int ret;

    va_start(ap, fmt);
    ret = vprintf(fmt, ap);
    va_end(ap);

    return ret;
}

int log_debug(const char *fmt, ...)
{
    va_list ap;
    int ret;
    int tmp;

    if (!debug) {
        return 0;
    }

    ret = printf("[" COLOR_GRE "DEBUG" COLOR_DONE "] ");
    if (ret < 0) {
        return ret;
    }

    va_start(ap, fmt);
    tmp = vprintf(fmt, ap);
    va_end(ap);

    if (tmp < 0) {
        return tmp;
    }

    return ret + tmp;
}

int log_error(const char *fmt, ...)
{
    va_list ap;
    int ret;
    int tmp;

    ret = printf("[" COLOR_RED "ERROR" COLOR_DONE "] ");
    if (ret < 0) {
        return ret;
    }

    va_start(ap, fmt);
    tmp = vprintf(fmt, ap);
    va_end(ap);

    if (tmp < 0) {
        return tmp;
    }

    return ret + tmp;
}
