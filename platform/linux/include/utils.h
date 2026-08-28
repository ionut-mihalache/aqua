#ifndef __AQUA_LINUX_UTILS_H_
#define __AQUA_LINUX_UTILS_H_

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DIE(assertion, call_description)                                       \
    do {                                                                       \
        if (assertion) {                                                       \
            fprintf(stderr, "%s (%d): %s - %s\n", __FILE__, __LINE__,          \
                    call_description, strerror(errno));                        \
            exit(EXIT_FAILURE);                                                \
        }                                                                      \
    } while (0)

#endif // __AQUA_LINUX_UTILS_H_
