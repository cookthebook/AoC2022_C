#ifndef _LOG_H_
#define _LOG_H_

#include <stdbool.h>
#include <stdarg.h>

#define COLOR_GRE "\033[38;5;10m"
#define COLOR_RED "\033[38;5;9m"
#define COLOR_DONE "\033[0m"

/* turn on or off debug logging */
void log_set_debug(bool _debug);

bool log_get_debug();

/* basic printf style log out */
__attribute__((format(printf, 1, 2)))
int log_print(const char *fmt, ...);

/* printf style log that only prints if --debug is enabled */
__attribute__((format(printf, 1, 2)))
int log_debug(const char *fmt, ...);

/* printf style log with extra error prepend */
__attribute__((format(printf, 1, 2)))
int log_error(const char *fmt, ...);

#endif /* _LOGGER_H_ */
