// SPDX-License-Identifier: LGPL-2.1-or-later

#include <Windows.h>
#include <assert.h>

#include "aqua-sync.h"
#include "aqua-types.h"
#include "platform.h"

struct HandleInfo {
    char name[max(AQUA_MUTEX_MEM_SIZE,
                  max(AQUA_SPINLOCK_MEM_SIZE,
                      max(AQUA_COND_MEM_SIZE, AQUA_SEM_MEM_SIZE)))];
    HANDLE handle;
};

_Thread_local struct HandleInfo tl_Handles[128];
_Thread_local uint32_t tl_Size;

static aqua_void_t createMutex(aqua_mutex_t *p_Mutex, const char *p_Name) {
    assert(strlen(p_Name) <= AQUA_MUTEX_MEM_SIZE);

    memcpy(p_Mutex->memory, p_Name, strlen(p_Name));
    CreateMutex(NULL, FALSE, p_Name);
}

static aqua_void_t createSpinLock(aqua_spinlock_t *p_SpinLock,
                                  const char *p_Name) {
    assert(strlen(p_Name) <= AQUA_SPINLOCK_MEM_SIZE);

    memcpy(p_SpinLock->memory, p_Name, strlen(p_Name));

    CreateMutex(NULL, FALSE, p_Name);
}

static aqua_void_t createCond(aqua_cond_t *p_Cond, const char *p_Name) {
    assert(strlen(p_Name) <= AQUA_COND_MEM_SIZE);

    memcpy(p_Cond->memory, p_Name, strlen(p_Name));

    CreateEvent(NULL, FALSE, FALSE, p_Name);
}

static aqua_void_t createSemaphore(aqua_sem_t *p_Sem, const char *p_Name,
                                   aqua_sem_cnt_t p_MaxVal) {
    assert(strlen(p_Name) <= AQUA_SEM_MEM_SIZE);

    memcpy(p_Sem->memory, p_Name, strlen(p_Name));

    CreateSemaphore(NULL, 0, p_MaxVal, p_Name);
}

static aqua_void_t destroyMutex(aqua_mutex_t *p_Mutex) {
    HANDLE mutexHandle = NULL;
    char *mutexName = (char *)p_Mutex->memory;

    // Search for already opened mutex
    for (uint32_t i = 0; i < tl_Size; ++i) {
        if (!strcmp(tl_Handles[i].name, mutexName)) {
            mutexHandle = tl_Handles[i].handle;
            break;
        }
    }

    // TODO: Check if mutex handle is valid

    CloseHandle(mutexHandle);
}

static aqua_void_t destroyCond(aqua_cond_t *p_Cond) {
    HANDLE condHandle = NULL;
    char *condName = (char *)p_Cond->memory;

    // Search for already opened condition
    for (uint32_t i = 0; i < tl_Size; ++i) {
        if (!strcmp(tl_Handles[i].name, condName)) {
            condHandle = tl_Handles[i].handle;
            break;
        }
    }

    // TODO: Check if condition handle is valid

    CloseHandle(condHandle);
}

static aqua_void_t destroySemaphore(aqua_sem_t *p_Sem) {
    HANDLE semHandle = NULL;
    char *semName = (char *)p_Sem->memory;

    // Search for already opened semaphore
    for (uint32_t i = 0; i < tl_Size; ++i) {
        if (!strcmp(tl_Handles[i].name, semName)) {
            semHandle = tl_Handles[i].handle;
            break;
        }
    }

    // TODO: Check if semaphore handle is valid

    CloseHandle(semHandle);
}

static aqua_void_t mutexLock(aqua_mutex_t *p_Mutex) {
    HANDLE mutexHandle = NULL;
    char *mutexName = (char *)p_Mutex->memory;

    // Search for already opened mutex
    for (uint32_t i = 0; i < tl_Size; ++i) {
        if (!strcmp(tl_Handles[i].name, mutexName)) {
            mutexHandle = tl_Handles[i].handle;
            goto handleOpened;
        }
    }

    mutexHandle = OpenMutex(MUTEX_ALL_ACCESS, FALSE, mutexName);
    memcpy(tl_Handles[tl_Size].name, mutexName, strlen(mutexName));
    tl_Handles[tl_Size].handle = mutexHandle;
    tl_Size++;

handleOpened:
    WaitForSingleObject(mutexHandle, INFINITE);
}

static aqua_void_t spinLock(aqua_spinlock_t *p_SpinLock) {
    char *spinlockName = (char *)p_SpinLock->memory;

    HANDLE spinLock = OpenMutex(MUTEX_ALL_ACCESS, FALSE, spinlockName);

    WaitForSingleObject(spinLock, INFINITE);
}

static aqua_void_t mutexUnlock(aqua_mutex_t *p_Mutex) {
    char *mutexName = (char *)p_Mutex->memory;

    HANDLE mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, mutexName);

    ReleaseMutex(mutex);
}

static aqua_void_t spinUnlock(aqua_spinlock_t *p_SpinLock) {
    char *spinlockName = (char *)p_SpinLock->memory;

    HANDLE spinLock = OpenMutex(MUTEX_ALL_ACCESS, FALSE, spinlockName);

    ReleaseMutex(spinLock);
}

static aqua_void_t condWait(aqua_cond_t *p_Cond, aqua_mutex_t *p_Mutex) {
    HANDLE mutexHandle = NULL;
    HANDLE condHandle = NULL;
    char *mutexName = (char *)p_Mutex->memory;
    char *condName = (char *)p_Cond->memory;

    // Search for already opened condition
    for (uint32_t i = 0; i < tl_Size; ++i) {
        if (!strcmp(tl_Handles[i].name, condName)) {
            condHandle = tl_Handles[i].handle;
            if (mutexHandle != NULL) {
                break;
            }
            continue;
        }

        if (!strcmp(tl_Handles[i].name, mutexName)) {
            mutexHandle = tl_Handles[i].handle;
            if (condHandle != NULL) {
                break;
            }
        }
    }

    mutexHandle = OpenMutex(MUTEX_ALL_ACCESS, FALSE, mutexName);
    memcpy(tl_Handles[tl_Size].name, mutexName, strlen(mutexName));
    tl_Handles[tl_Size].handle = mutexHandle;
    tl_Size++;

    condHandle = OpenEvent(EVENT_ALL_ACCESS, FALSE, condName);
    memcpy(tl_Handles[tl_Size].name, condName, strlen(condName));
    tl_Handles[tl_Size].handle = condHandle;
    tl_Size++;

    ReleaseMutex(mutexHandle);
    WaitForSingleObject(condHandle, INFINITE);
    WaitForSingleObject(mutexHandle, INFINITE);
}

static aqua_void_t condBroadcast(aqua_cond_t *p_Cond) {
    char *condName = (char *)p_Cond->memory;

    HANDLE cond = OpenEvent(EVENT_ALL_ACCESS, FALSE, condName);

    SetEvent(cond);
}

struct AQUA_Sync Sync = {
    .createMutex = createMutex,
    .createSpinLock = createSpinLock,
    .createCond = createCond,
    .createSemaphore = createSemaphore,
    .destroyMutex = destroyMutex,
    .destroyCond = destroyCond,
    .destroySemaphore = destroySemaphore,

    .mutexLock = mutexLock,
    .spinLock = spinLock,
    .mutexUnlock = mutexUnlock,
    .spinUnlock = spinUnlock,

    .condWait = condWait,
    .condBroadcast = condBroadcast,
};
