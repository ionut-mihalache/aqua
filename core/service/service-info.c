// SPDX-License-Identifier: LGPL-2.1-or-later
#include "aqua-sync.h"
#include "aqua-types.h"
#include "commons.h"
#include "dsp-service.h"
#include "dsp.h"
#include "platform.h"

#include <inttypes.h>
#include <stdio.h>

#define SERVICE_INFO_MAX 8 // maybe will be defined in the configuration file

static struct ServiceInfo {
    struct {
        struct ServiceConnectInfo connectInfo;
        aqua_u8_t used;
    } connectInfos[SERVICE_INFO_MAX];

    struct {
        struct ServiceCallInfo callInfo;
        aqua_u8_t used;
    } callInfos[SERVICE_INFO_MAX];

    struct {
        struct ServiceReturnInfo returnInfo;
        aqua_u8_t used;
    } returnInfos[OPENED_CONNECTIONS];

    aqua_mutex_t mutex;
} s_ServiceInfo;

void initServiceInfo(const char *p_StrId) {
    char mutexName[AQUA_MUTEX_MEM_SIZE];
    uint64_t nameHash = hashString64(p_StrId);

    snprintf(mutexName, sizeof(mutexName), "__aqua_%016" PRIx64 "%u", nameHash,
             AQUA_SERVICE_INFO_MUTEX_TAG);
    Sync.createMutex(&s_ServiceInfo.mutex, mutexName,
                     SERVICE_INFO_MUTEX_HANDLE);
}

int32_t aquaCreateConnectInfo() {
    int32_t handle = -1;
    Sync.mutexLock(&s_ServiceInfo.mutex);
    for (int32_t i = 0; i < SERVICE_INFO_MAX; ++i) {
        if (!s_ServiceInfo.connectInfos[i].used) {
            handle = i;
            s_ServiceInfo.connectInfos[i].used = 1;
            break;
        }
    }
    Sync.mutexUnlock(&s_ServiceInfo.mutex);

    return handle;
}

int32_t aquaCreateCallInfo() {
    int32_t handle = -1;
    Sync.mutexLock(&s_ServiceInfo.mutex);
    for (int32_t i = 0; i < SERVICE_INFO_MAX; ++i) {
        if (!s_ServiceInfo.callInfos[i].used) {
            handle = i;
            s_ServiceInfo.callInfos[i].used = 1;
            break;
        }
    }
    Sync.mutexUnlock(&s_ServiceInfo.mutex);

    return handle;
}

int32_t createReturnInfo() {
    int32_t handle = -1;
    Sync.mutexLock(&s_ServiceInfo.mutex);

    for (int32_t i = 0; i < (int32_t)OPENED_CONNECTIONS; ++i) {
        if (!s_ServiceInfo.returnInfos[i].used) {
            handle = i;
            s_ServiceInfo.returnInfos[i].used = 1;
            break;
        }
    }
    Sync.mutexUnlock(&s_ServiceInfo.mutex);

    return handle;
}

void freeConnectInfo(int32_t p_ConnectInfoHandle) {
    Sync.mutexLock(&s_ServiceInfo.mutex);
    s_ServiceInfo.connectInfos[p_ConnectInfoHandle].used = 0;
    Sync.mutexUnlock(&s_ServiceInfo.mutex);
}

void freeCallInfo(int32_t p_CallInfoHandle) {
    Sync.mutexLock(&s_ServiceInfo.mutex);
    s_ServiceInfo.callInfos[p_CallInfoHandle].used = 0;
    Sync.mutexUnlock(&s_ServiceInfo.mutex);
}

void freeReturnInfo(int32_t p_ReturnInfoHandle) {
    Sync.mutexLock(&s_ServiceInfo.mutex);
    s_ServiceInfo.returnInfos[p_ReturnInfoHandle].used = 0;
    Sync.mutexUnlock(&s_ServiceInfo.mutex);
}

struct ServiceConnectInfo *getConnectInfo(int32_t p_ConnectInfoHandle) {
    return &s_ServiceInfo.connectInfos[p_ConnectInfoHandle].connectInfo;
}

struct ServiceCallInfo *getCallInfo(int32_t p_CallInfoHandle) {
    return &s_ServiceInfo.callInfos[p_CallInfoHandle].callInfo;
}

struct ServiceReturnInfo *getReturnInfo(int32_t p_ReturnInfoHandle) {
    return &s_ServiceInfo.returnInfos[p_ReturnInfoHandle].returnInfo;
}
