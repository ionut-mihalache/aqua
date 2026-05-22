// SPDX-License-Identifier: LGPL-2.1-or-later

#include <Windows.h>

#include "aqua-sync.h"
#include "aqua-types.h"
#include "platform.h"

_Static_assert(sizeof(HANDLE) <= AQUA_MUTEX_MEM_SIZE,
               "AQUA_MUTEX_MEM_SIZE too small");

_Static_assert(sizeof(HANDLE) <= AQUA_SPINLOCK_MEM_SIZE,
               "AQUA_SPINLOCK_MEM_SIZE too small");

_Static_assert(sizeof(HANDLE) <= AQUA_COND_MEM_SIZE,
               "AQUA_COND_MEM_SIZE too small");

_Static_assert(sizeof(HANDLE) <= AQUA_SEM_MEM_SIZE,
               "AQUA_SEM_MEM_SIZE too small");

static aqua_void_t createMutex(aqua_mutex_t *p_Mutex, const char *p_Name) {
    (void)p_Mutex;
    (void)p_Name;
}

static aqua_void_t createSpinLock(aqua_spinlock_t *p_SpinLock,
                                  const char *p_Name) {
    (void)p_SpinLock;
    (void)p_Name;
}

static aqua_void_t createCond(aqua_cond_t *p_Cond, const char *p_Name) {
    (void)p_Cond;
    (void)p_Name;
}

static aqua_void_t createSemaphore(aqua_sem_t *p_Sem, const char *p_Name) {
    (void)p_Sem;
    (void)p_Name;
}

static aqua_void_t destroyMutex(aqua_mutex_t *p_Mutex) {
    (void)p_Mutex;
}

static aqua_void_t destroyCond(aqua_cond_t *p_Cond) {
    (void)p_Cond;
}

static aqua_void_t destroySemaphore(aqua_sem_t *p_Sem) {
    (void)p_Sem;
}

static aqua_void_t mutexLock(aqua_mutex_t *p_Mutex) {
    (void)p_Mutex;
}

static aqua_void_t spinLock(aqua_spinlock_t *p_SpinLock) {
    (void)p_SpinLock;
}

static aqua_void_t mutexUnlock(aqua_mutex_t *p_Mutex) {
    (void)p_Mutex;
}

static aqua_void_t spinUnlock(aqua_spinlock_t *p_SpinLock) {
    (void)p_SpinLock;
}

static aqua_void_t condWait(aqua_cond_t *p_Cond, aqua_mutex_t *p_Mutex) {
    (void)p_Cond;
    (void)p_Mutex;
}

static aqua_void_t condBroadcast(aqua_cond_t *p_Cond) {
    (void)p_Cond;
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
