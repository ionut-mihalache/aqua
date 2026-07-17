// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef __AQUA_LINUX_TYPES_H_
#define __AQUA_LINUX_TYPES_H_

#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include <sys/types.h>

#define AQUA_API_EXPORT __attribute__((visibility("default")))

typedef size_t aqua_size_t;
typedef ssize_t aqua_ssize_t;

typedef off_t aqua_off_t;
typedef off64_t aqua_off64_t;
typedef loff_t aqua_loff_t;

typedef pid_t aqua_pid_t;

typedef int aqua_file_handle_t;

#endif // __AQUA_LINUX_TYPES_H_
