#include <ctype.h>
#include <string.h>
#include "util.h"

size_t util_strip(char *str)
{
    size_t ret = strlen(str);

    while (ret > 0) {
        if (isspace(str[ret-1])) {
            str[ret-1] = '\0';
            ret--;
        } else {
            break;
        }
    }

    return ret;
}
