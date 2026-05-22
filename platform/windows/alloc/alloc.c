// SPDX-License-Identifier: LGPL-2.1-or-later

#include "aqua-types.h"
#include "platform-types.h"
#include "platform.h"

#include "system-values.h"

static aqua_err_t memmap(aqua_void_ptr_t *p_ResultAddr,
                         aqua_void_ptr_t p_StartAddr, aqua_size_t p_Len,
                         aqua_mem_prot_t p_Prot, aqua_mem_flags_t p_Flags,
                         aqua_file_handle_t p_Fd, aqua_off_t p_Off) {
    (void)p_ResultAddr;
    (void)p_StartAddr;
    (void)p_Len;
    (void)p_Prot;
    (void)p_Flags;
    (void)p_Fd;
    (void)p_Off;

    return AQUA_NO_ERROR;
}

static aqua_int_t memunmap(aqua_void_ptr_t p_Addr, aqua_size_t p_Len) {
    (void)p_Addr;
    (void)p_Len;

    return (-1);
}

struct AQUA_Allocator Allocator = {
    .memmap = memmap,
    .memunmap = memunmap,
};
