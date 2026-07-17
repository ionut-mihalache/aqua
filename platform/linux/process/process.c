// SPDX-License-Identifier: LGPL-2.1-or-later

#include <unistd.h>

#include "platform-types.h"
#include "platform.h"

static aqua_pid_t sf_GetPid() {
    return getpid();
}

struct AQUA_Process Process = {
    .getPid = sf_GetPid,
};
