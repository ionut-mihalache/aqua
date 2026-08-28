// SPDX-License-Identifier: LGPL-2.1-or-later

#include <errno.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/user.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <unistd.h>

#include "aqua-types.h"
#include "platform.h"
#include "system-values.h"
#include "utils.h"

static int mapFlags(aqua_file_flags_t p_Flags) {
    int flags = 0;

    if (p_Flags & AQUA_FILE_PERM_READ) {
        flags |= O_RDONLY;
    }

    if (p_Flags & AQUA_FILE_PERM_WRITE) {
        flags |= O_WRONLY;
    }

    if (p_Flags & AQUA_FILE_PERM_RDWR) {
        flags |= O_RDWR;
    }

    return flags;
}

static mode_t mapMode(aqua_file_mode_t p_Mode) {
    mode_t flags = 0;

    if (p_Mode & AQUA_FILE_MODE_USER_READ) {
        flags |= S_IRUSR;
    }

    if (p_Mode & AQUA_FILE_MODE_USER_WRITE) {
        flags |= S_IWUSR;
    }

    if (p_Mode & AQUA_FILE_MODE_GROUP_READ) {
        flags |= S_IRGRP;
    }

    if (p_Mode & AQUA_FILE_MODE_GROUP_WRITE) {
        flags |= S_IWGRP;
    }

    if (p_Mode & AQUA_FILE_MODE_OTHER_READ) {
        flags |= S_IROTH;
    }

    if (p_Mode & AQUA_FILE_MODE_OTHER_WRITE) {
        flags |= S_IWOTH;
    }

    return flags;
}

static aqua_err_t sf_Close(aqua_file_handle_t p_Handle) {
    int rc;
    aqua_err_t err = AQUA_NO_ERROR;

    rc = close(p_Handle);
    if (rc < 0) {
        err = AQUA_SHM_OBJ_CLOSE_FAILED;
    }

    return err;
}

static aqua_file_handle_t sf_Create(const char *p_Name,
                                    aqua_file_flags_t p_Flags,
                                    aqua_file_mode_t p_Mode, aqua_off_t p_Size,
                                    aqua_bool_t p_Unlink) {
    int rc;
    int shmFd = -1;
    uint8_t shouldTruncate = true;
    int flags = mapFlags(p_Flags);
    mode_t mode = mapMode(p_Mode);

    int oldMask = umask(0);

    if (p_Unlink) {
        shm_unlink(p_Name); // TODO: This should not happen all the time.
    }

    shmFd = shm_open(p_Name, O_CREAT | O_EXCL | flags, mode);
    if (shmFd < 0) {
        if (errno == EEXIST) {
            shouldTruncate = false;
            shmFd = shm_open(p_Name, flags, mode);
            goto end;
        } else {
            goto end;
        }
    }

    if (!shouldTruncate) {
        goto end;
    }

    // Prepare space to allow installation for SERVICES_NUMBER services at most
    // We need a bit map for fast iteration
    // Get the number of bytes for the bit map
    // The information that we need is an array of pointers to the information
    // that we need
    rc = ftruncate(shmFd, p_Size);
    DIE(rc != 0, "Could not truncate shared memory object");

    umask(oldMask);

end:
    return shmFd;
}

static aqua_void_t sf_Destroy(const char *p_Name) {
    shm_unlink(p_Name);
}

struct AQUA_SharedMemoryObject SharedMemoryObject = {
    .create = sf_Create,
    .close = sf_Close,
    .destroy = sf_Destroy,
};
