// SPDX-License-Identifier: LGPL-2.1-or-later

#include <Windows.h>
#include <assert.h>

#include "aqua-sync.h"
#include "aqua-types.h"
#include "platform.h"

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

static aqua_void_t createSemaphore(aqua_sem_t *p_Sem, const char *p_Name) {
    (void)p_Sem;
    (void)p_Name;
}

static aqua_void_t destroyMutex(aqua_mutex_t *p_Mutex) {
    char *mutexName = (char *)p_Mutex->memory;

    HANDLE mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, mutexName);

    CloseHandle(mutex);
}

static aqua_void_t destroyCond(aqua_cond_t *p_Cond) {
    char *condName = (char *)p_Cond->memory;

    HANDLE cond = OpenEvent(EVENT_ALL_ACCESS, FALSE, condName);

    CloseHandle(cond);
}

static aqua_void_t destroySemaphore(aqua_sem_t *p_Sem) {
    (void)p_Sem;
}

static aqua_void_t mutexLock(aqua_mutex_t *p_Mutex) {
    char *mutexName = (char *)p_Mutex->memory;

    HANDLE mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, mutexName);

    WaitForSingleObject(mutex, INFINITE);
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
    char *mutexName = (char *)p_Mutex->memory;
    char *condName = (char *)p_Cond->memory;

    HANDLE mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, mutexName);
    HANDLE cond = OpenEvent(EVENT_ALL_ACCESS, FALSE, condName);

    ReleaseMutex(mutex);
    WaitForSingleObject(cond, INFINITE);
    WaitForSingleObject(mutex, INFINITE);
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
