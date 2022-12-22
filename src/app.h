#ifndef _APP_H_
#define _APP_H_

#include <stdbool.h>
#include <stdio.h>

typedef enum app_error_enum {
    APP_ERROR_OK = 0,
    APP_ERROR_INVALID,
    APP_ERROR_CRITICAL,
    APP_ERROR_EOF,
} app_error_e;

typedef struct app_struct {
    /* input file for the challenge */
    FILE *input;
    /* is this star 2 */
    bool star2;
} app_t;

/* return a static string for an error */
const char *app_error_str(app_error_e err);

#endif /* _APP_H_ */
