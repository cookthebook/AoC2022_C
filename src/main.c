#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "app.h"
#include "days.h"
#include "log.h"

#define HELP \
"Usage: aoc [OPTIONS] <day #[s]> <input file>\n"                                      \
"\n"                                                                                  \
"  day #[s]: Which day to select, add s to select second star (e.g. 10, 11s, etc.)\n" \
"  input file: Path to file containing the input, - for stdin\n"                      \
"\n"                                                                                  \
"Options:\n"                                                                          \
"  --debug: Add extra debug prints\n"


int main(int argc, char **argv)
{
    bool debug = false;
    char *day;
    int day_num;
    char *input;
    size_t daylen;
    app_t app = { 0 };
    app_error_e ret;

    if (argc < 3) {
        printf(HELP);
        return APP_ERROR_INVALID;
    }

    for (int i = 1; i < (argc - 2); i++) {
        if (!strcmp(argv[i], "--debug")) {
            debug = true;
        } else {
            log_error("Unrecognized option \"%s\"\n", argv[i]);
            return APP_ERROR_INVALID;
        }
    }

    log_set_debug(debug);

    day = argv[argc-2];
    daylen = strlen(day);
    if (daylen == 0) {
        log_error("Proved a day selection\n");
        return APP_ERROR_INVALID;
    }

    app.star2 = (day[daylen-1] == 's');

    input = argv[argc-1];
    if (!strcmp(input, "-")) {
        app.input = stdin;
    } else {
        app.input = fopen(input, "r");
    }

    if (app.input == NULL) {
        log_error("Unable to open input file \"%s\"\n", input);
        return APP_ERROR_INVALID;
    }

    day_num = atoi(day);
    log_print(
        "~~ Day %d (star %c) ~~\n\n",
        day_num,
        app.star2 ? '2' : '1'
    );

    switch (day_num) {
    case 10:
        ret = day10(&app);
        break;
    case 11:
        ret = day11(&app);
        break;
    case 12:
        ret = day12(&app);
        break;
    case 13:
        ret = day13(&app);
        break;
    default:
        log_error("Invalid day selection \"%s\"\n", day);
        ret = APP_ERROR_INVALID;
    }

    return ret;
}

const char *
app_error_str(app_error_e err)
{
    switch (err) {
    case APP_ERROR_OK:
        return "OK";
    case APP_ERROR_INVALID:
        return "INVALID";
    case APP_ERROR_CRITICAL:
        return "CRITICAL";
    case APP_ERROR_EOF:
        return "EOF";
    default:
        return "UNKNOWN";
    }
}
