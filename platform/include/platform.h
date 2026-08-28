// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef __AQUA_PLATFORM_H_
#define __AQUA_PLATFORM_H_

#include "aqua-sync.h"
#include "aqua-types.h"
#include "platform-types.h"

enum {
    INSTALL_DATA_HANDLE,
    CONNECT_LIST_HANDLE,
    CONNECT_HANDLE,
    CONNECT_FULL_HANDLE,
    CONNECT_EMPTY_HANDLE,
    SEND_HANDLE,
    SEND_FULL_HANDLE,
    SEND_EMPTY_HANDLE,
    RECEIVE_HANDLE,
    RECEIVE_FULL_HANDLE,
    RECEIVE_EMPTY_HANDLE,
    CONNECT_RESPONSE_HANDLE,
    CONNECT_RESPONSE_FULL_HANDLE,
    CONNECT_RESPONSE_EMPTY_HANDLE,
    DISCONNECT_HANDLE,
    DISCONNECT_FULL_HANDLE,
    DISCONNECT_EMPTY_HANDLE,
    HANDLES_TOTAL // This needs to be always the last value
};

struct AQUA_Memory {
    aqua_size_t (*getPageSize)();
    aqua_size_t (*getMapGranularity)();
    aqua_void_t (*triggerPageFaults)(aqua_void_t *addr, aqua_size_t size,
                                     aqua_mem_perm_t perm);
};

struct AQUA_Allocator {
    aqua_err_t (*memmap)(aqua_void_ptr_t *result_addr,
                         aqua_void_ptr_t start_addr, aqua_size_t len,
                         aqua_mem_prot_t prot, aqua_mem_flags_t flags,
                         aqua_file_handle_t handle, aqua_off_t off);
    aqua_err_t (*memunmap)(aqua_void_ptr_t addr, aqua_size_t len);
};

struct AQUA_Sync {
    aqua_void_t (*createMutex)(aqua_mutex_t *mutex, const char *name,
                               aqua_u16_t type);
    aqua_void_t (*createSpinLock)(aqua_spinlock_t *spinLock, const char *name,
                                  aqua_u16_t type);
    aqua_void_t (*createCond)(aqua_cond_t *cond, const char *name,
                              aqua_u16_t type);
    aqua_void_t (*createSemaphore)(aqua_sem_t *sem, const char *name,
                                   aqua_sem_cnt_t maxValue, aqua_u16_t type);
    aqua_void_t (*destroyMutex)(aqua_mutex_t *mutex);
    aqua_void_t (*destroySpinLock)(aqua_spinlock_t *spinLock);
    aqua_void_t (*destroyCond)(aqua_cond_t *cond);
    aqua_void_t (*destroySemaphore)(aqua_sem_t *sem);

    aqua_void_t (*mutexLock)(aqua_mutex_t *mutex);
    aqua_void_t (*spinLock)(aqua_spinlock_t *spinLock);
    aqua_void_t (*mutexUnlock)(aqua_mutex_t *mutex);
    aqua_void_t (*spinUnlock)(aqua_spinlock_t *spinUnlock);
    aqua_void_t (*condWait)(aqua_cond_t *cond, aqua_mutex_t *mutex);
    aqua_void_t (*condBroadcast)(aqua_cond_t *cond);
};

struct AQUA_SharedMemoryObject {
    aqua_file_handle_t (*create)(const char *name, aqua_file_flags_t flags,
                                 aqua_file_mode_t mode, aqua_off_t size,
                                 aqua_bool_t unlink);
    aqua_err_t (*close)(aqua_file_handle_t handle);
    aqua_void_t (*destroy)(const char *name);
};

struct AQUA_Process {
    aqua_pid_t (*getPid)();
};

extern struct AQUA_Memory Memory;
extern struct AQUA_Allocator Allocator;
extern struct AQUA_Sync Sync;
extern struct AQUA_SharedMemoryObject SharedMemoryObject;
extern struct AQUA_Process Process;

#endif // __AQUA_PLATFORM_H_
