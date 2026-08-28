// SPDX-License-Identifier: LGPL-2.1-or-later

#include <Windows.h>
#include <assert.h>

#include "aqua-sync.h"
#include "aqua-types.h"
#include "platform-types.h"
#include "platform.h"

_Thread_local static HANDLE tl_Handles[HANDLES_TOTAL];

static aqua_void_t createMutex(aqua_mutex_t *p_Mutex, const char *p_Name,
                               aqua_u16_t p_Type) {
    assert(strlen(p_Name) <= UUID_LEN);
    assert(p_Type = HANDLES_TOTAL);

    memset(p_Mutex->id, 0, UUID_LEN + 1);
    memcpy(p_Mutex->id, p_Name, strlen(p_Name));
    p_Mutex->type = p_Type;

    HANDLE handle = CreateMutex(NULL, FALSE, p_Name);

    // TODO: Check if mutex handle is valid

    tl_Handles[p_Mutex->type] = handle;
}

static aqua_void_t createSpinLock(aqua_spinlock_t *p_SpinLock,
                                  const char *p_Name, aqua_u16_t p_Type) {
    assert(strlen(p_Name) <= UUID_LEN);
    assert(p_Type < HANDLES_TOTAL);

    memset(p_SpinLock->id, 0, UUID_LEN + 1);
    memcpy(p_SpinLock->memory, p_Name, strlen(p_Name));
    p_SpinLock->type = p_Type;

    HANDLE handle = CreateMutex(NULL, FALSE, p_Name);

    // TODO: Check if spinlock handle is valid

    tl_Handles[p_SpinLock->type] = handle;
}

static aqua_void_t createCond(aqua_cond_t *p_Cond, const char *p_Name,
                              aqua_u16_t p_Type) {
    assert(strlen(p_Name) <= UUID_LEN);
    assert(p_Type < HANDLES_TOTAL);

    memset(p_Cond->id, 0, UUID_LEN + 1);
    memcpy(p_Cond->memory, p_Name, strlen(p_Name));
    p_Cond->type = p_Type;

    HANDLE handle = CreateEvent(NULL, FALSE, FALSE, p_Name);

    // TODO: Check if condition handle is valid

    tl_Handles[p_Cond->type] = handle;
}

static aqua_void_t createSemaphore(aqua_sem_t *p_Sem, const char *p_Name,
                                   aqua_sem_cnt_t p_MaxVal, aqua_u16_t p_Type) {
    assert(strlen(p_Name) <= UUID_LEN);

    memset(p_Sem->id, 0, UUID_LEN + 1);
    memcpy(p_Sem->memory, p_Name, strlen(p_Name));
    p_Sem->type = p_Type;

    HANDLE handle = CreateSemaphore(NULL, 0, p_MaxVal, p_Name);

    // TODO: Check if mutex handle is valid

    tl_Handles[p_Sem->type] = handle;
}

static aqua_void_t destroyMutex(aqua_mutex_t *p_Mutex) {
    // TODO: Check if mutex handle is valid

    CloseHandle(tl_Handles[p_Mutex->type]);
}

static aqua_void_t destroySpinLock(aqua_spinlock_t *p_SpinLock) {
    // TODO: Check if spinlock handle is valid

    CloseHandle(tl_Handles[p_SpinLock->type]);
}

static aqua_void_t destroyCond(aqua_cond_t *p_Cond) {
    // TODO: Check if condition handle is valid

    CloseHandle(tl_Handles[p_Cond->type]);
}

static aqua_void_t destroySemaphore(aqua_sem_t *p_Sem) {
    // TODO: Check if semaphore handle is valid

    CloseHandle(tl_Handles[p_Sem->type]);
}

static aqua_void_t mutexLock(aqua_mutex_t *p_Mutex) {
    HANDLE handle = tl_Handles[p_Mutex->type];

    if (handle == NULL) {
        handle = OpenMutex(MUTEX_ALL_ACCESS, FALSE, (char *)p_Mutex->id);
        tl_Handles[p_Mutex->type] = handle;
    }

    // TODO: Check if mutex handle is valid

    WaitForSingleObject(handle, INFINITE);
}

static aqua_void_t spinLock(aqua_spinlock_t *p_SpinLock) {
    HANDLE handle = tl_Handles[p_SpinLock->type];

    if (handle == NULL) {
        handle = OpenMutex(MUTEX_ALL_ACCESS, FALSE, (char *)p_SpinLock->id);
        tl_Handles[p_SpinLock->type] = handle;
    }

    // TODO: Check if spinlock handle is valid

    WaitForSingleObject(handle, INFINITE);
}

static aqua_void_t mutexUnlock(aqua_mutex_t *p_Mutex) {
    HANDLE handle = tl_Handles[p_Mutex->type];

    if (handle == NULL) {
        handle = OpenMutex(MUTEX_ALL_ACCESS, FALSE, (char *)p_Mutex->id);
        tl_Handles[p_Mutex->type] = handle;
    }

    // TODO: Check if mutex handle is valid

    ReleaseMutex(handle);
}

static aqua_void_t spinUnlock(aqua_spinlock_t *p_SpinLock) {
    HANDLE handle = tl_Handles[p_SpinLock->type];

    if (handle == NULL) {
        handle = OpenMutex(MUTEX_ALL_ACCESS, FALSE, (char *)p_SpinLock->id);
        tl_Handles[p_SpinLock->type] = handle;
    }

    // TODO: Check if spinlock handle is valid

    ReleaseMutex(handle);
}

static aqua_void_t condWait(aqua_cond_t *p_Cond, aqua_mutex_t *p_Mutex) {
    HANDLE mutexHandle = tl_Handles[p_Mutex->type];

    if (mutexHandle == NULL) {
        mutexHandle = OpenMutex(MUTEX_ALL_ACCESS, FALSE, (char *)p_Mutex->id);
        tl_Handles[p_Mutex->type] = mutexHandle;
    }

    // TODO: Check if mutex handle is valid

    HANDLE condHandle = tl_Handles[p_Cond->type];

    if (condHandle == NULL) {
        condHandle = OpenEvent(MUTEX_ALL_ACCESS, FALSE, (char *)p_Cond->id);
        tl_Handles[p_Cond->type] = condHandle;
    }

    // TODO: Check if condition handle is valid

    ReleaseMutex(mutexHandle);
    WaitForSingleObject(condHandle, 5000);
    WaitForSingleObject(mutexHandle, INFINITE);
}

static aqua_void_t condBroadcast(aqua_cond_t *p_Cond) {
    HANDLE condHandle = tl_Handles[p_Cond->type];

    if (condHandle == NULL) {
        condHandle = OpenEvent(MUTEX_ALL_ACCESS, FALSE, (char *)p_Cond->id);
        tl_Handles[p_Cond->type] = condHandle;
    }

    // TODO: Check if condition handle is valid

    SetEvent(condHandle);
}

struct AQUA_Sync Sync = {
    .createMutex = createMutex,
    .createSpinLock = createSpinLock,
    .createCond = createCond,
    .createSemaphore = createSemaphore,
    .destroyMutex = destroyMutex,
    .destroySpinLock = destroySpinLock,
    .destroyCond = destroyCond,
    .destroySemaphore = destroySemaphore,

    .mutexLock = mutexLock,
    .spinLock = spinLock,
    .mutexUnlock = mutexUnlock,
    .spinUnlock = spinUnlock,

    .condWait = condWait,
    .condBroadcast = condBroadcast,
};
