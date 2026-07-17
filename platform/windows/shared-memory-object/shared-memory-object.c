// SPDX-License-Identifier: LGPL-2.1-or-later

#include <Windows.h>

#include "aqua-types.h"
#include "platform.h"
#include "system-values.h"

static aqua_err_t sf_Close(aqua_file_handle_t p_Handle) {
    (void)p_Handle;

    aqua_err_t err = AQUA_NO_ERROR;

    return err;
}

static aqua_file_handle_t sf_Create(const char *p_Name,
                                    aqua_file_flags_t p_Flags,
                                    aqua_file_mode_t p_Mode, aqua_off_t p_Size,
                                    aqua_bool_t p_Unlink) {
    (void)p_Name;
    (void)p_Flags;
    (void)p_Mode;
    (void)p_Size;
    (void)p_Unlink;

    return NULL;
}

static aqua_void_t sf_Destroy(const char *p_Name) {
    (void)p_Name;
}

struct AQUA_SharedMemoryObject SharedMemoryObject = {
    .create = sf_Create,
    .close = sf_Close,
    .destroy = sf_Destroy,
};
