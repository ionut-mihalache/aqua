// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef __AQUA_SYNC_H_
#define __AQUA_SYNC_H_

#include <stdalign.h>

#include "aqua-types.h"

#define AQUA_MUTEX_MEM_SIZE 65
#define AQUA_SPINLOCK_MEM_SIZE 65
#define AQUA_COND_MEM_SIZE 65
#define AQUA_SEM_MEM_SIZE 65

#define UUID_LEN 36

typedef struct aqua_mutex {
    union {
        alignas(16) aqua_u8_t
            memory[AQUA_MUTEX_MEM_SIZE]; // used for storing generic information
        aqua_u8_t id[UUID_LEN + 1];
        aqua_u16_t type;
        aqua_u8_t
            extra[AQUA_MUTEX_MEM_SIZE - (UUID_LEN + 1) - sizeof(aqua_u16_t)];
    };
} aqua_mutex_t;

typedef struct aqua_spinlock {
    union {
        alignas(
            16) aqua_u8_t memory[AQUA_SPINLOCK_MEM_SIZE]; // used for storing
                                                          // generic information
        aqua_u8_t id[UUID_LEN + 1];
        aqua_u16_t type;
        aqua_u8_t
            extra[AQUA_SPINLOCK_MEM_SIZE - (UUID_LEN + 1) - sizeof(aqua_u16_t)];
    };
} aqua_spinlock_t;

typedef struct aqua_cond {
    union {
        alignas(16) aqua_u8_t
            memory[AQUA_COND_MEM_SIZE]; // used for storing generic information
        aqua_u8_t id[UUID_LEN + 1];
        aqua_u16_t type;
        aqua_u8_t
            extra[AQUA_COND_MEM_SIZE - (UUID_LEN + 1) - sizeof(aqua_u16_t)];
    };
} aqua_cond_t;

typedef struct aqua_sem {
    union {
        alignas(16) aqua_u8_t
            memory[AQUA_SEM_MEM_SIZE]; // used for storing generic information
        aqua_u8_t id[UUID_LEN + 1];
        aqua_u16_t type;
        aqua_u8_t
            extra[AQUA_SEM_MEM_SIZE - (UUID_LEN + 1) - sizeof(aqua_u16_t)];
    };
} aqua_sem_t;

#endif // __AQUA_SYNC_H_
