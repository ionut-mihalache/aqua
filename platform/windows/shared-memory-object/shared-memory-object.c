// SPDX-License-Identifier: LGPL-2.1-or-later

#include <Windows.h>
#include <AclAPI.h>

#include "aqua-types.h"
#include "platform-types.h"
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
    (void)p_Flags;
    (void)p_Mode;
    (void)p_Unlink;

    aqua_file_handle_t handle;
    SECURITY_DESCRIPTOR sd;
    SECURITY_ATTRIBUTES sa;
    PACL pAcl = NULL;
    EXPLICIT_ACCESS ea;

    ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS));
    BuildExplicitAccessWithNameA(&ea, "EVERYONE", FILE_MAP_ALL_ACCESS,
                                 GRANT_ACCESS, NO_INHERITANCE);

    SetEntriesInAcl(1, &ea, NULL, &pAcl);

    InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
    SetSecurityDescriptorDacl(&sd, TRUE, pAcl, FALSE);

    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = &sd;
    sa.bInheritHandle = FALSE;

    handle = CreateFileMapping(
        INVALID_HANDLE_VALUE,  // use paging file
        &sa,                   // default security
        PAGE_READWRITE,        // read/write access
        (DWORD)(p_Size >> 32), // maximum object size (high-order DWORD)
        (DWORD)p_Size,         // maximum object size (low-order DWORD)
        p_Name);               // name of mapping object
    // DIE(handle == NULL, "Could not create shared memory object");

    return handle;
}

static aqua_void_t sf_Destroy(const char *p_Name) {
    (void)p_Name;
}

struct AQUA_SharedMemoryObject SharedMemoryObject = {
    .create = sf_Create,
    .close = sf_Close,
    .destroy = sf_Destroy,
};
