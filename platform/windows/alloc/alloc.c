// SPDX-License-Identifier: LGPL-2.1-or-later

#include <Windows.h>

#include "aqua-types.h"
#include "platform-types.h"
#include "platform.h"

#include "system-values.h"

static DWORD mapProt(aqua_mem_prot_t p_Prot) {
    DWORD prot = 0;

    if (p_Prot & AQUA_MEM_PROT_READ) {
        prot |= FILE_MAP_READ;
    }

    if (p_Prot & AQUA_MEM_PROT_WRITE) {
        prot |= FILE_MAP_WRITE;
    }

    if (p_Prot & AQUA_MEM_PROT_EXEC) {
        prot |= FILE_MAP_EXECUTE;
    }

    return prot;
}

static aqua_err_t memmap(aqua_void_ptr_t *p_ResultAddr,
                         aqua_void_ptr_t p_StartAddr, aqua_size_t p_Len,
                         aqua_mem_prot_t p_Prot, aqua_mem_flags_t p_Flags,
                         aqua_file_handle_t p_Fd, aqua_off_t p_Off) {
    (void)p_StartAddr;
    (void)p_Flags;

    DWORD prot = mapProt(p_Prot);

    void *ptr =
        MapViewOfFile(p_Fd, prot, p_Off >> 32, p_Off & 0xFFFFFFFF, p_Len);

    *p_ResultAddr = ptr;

    return AQUA_NO_ERROR;
}

static aqua_err_t memunmap(aqua_void_ptr_t p_Addr, aqua_size_t p_Len) {
    (void)p_Len;

    UnmapViewOfFile(p_Addr);

    return AQUA_NO_ERROR;
}

struct AQUA_Allocator Allocator = {
    .memmap = memmap,
    .memunmap = memunmap,
};
