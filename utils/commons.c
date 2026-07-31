// SPDX-License-Identifier: LGPL-2.1-or-later

#include "commons.h"

#include "aqua-types.h"
#include "platform-types.h"
#include "platform.h"
#include "system-values.h"
#include "utils.h"

aqua_size_t alignUp(aqua_size_t p_Base, aqua_size_t p_Alignment) {
    return (p_Base + p_Alignment - 1) & ~(p_Alignment - 1);
}

void createQ(void **p_QPtrRes, aqua_size_t p_Size, aqua_mem_prot_t p_Prot,
             aqua_file_handle_t p_Fd) {
    aqua_err_t err;
    err = Allocator.memmap(p_QPtrRes, NULL, p_Size, p_Prot, AQUA_MEM_SHARED,
                           p_Fd, 0);
    DIE(err == AQUA_MEM_MAP_FAILED, "Could not map return queue memory");
}
