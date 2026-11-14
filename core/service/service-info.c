// SPDX-License-Identifier: LGPL-2.1-or-later
#include "aqua-sync.h"
#include "aqua-types.h"
#include "commons.h"
#include "dsp-service.h"
#include "dsp.h"
#include "platform-types.h"
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

    aqua_mutex_t connectMutex;
    aqua_mutex_t callMutex;
    aqua_mutex_t returnMutex;
} s_ServiceInfo;

void initServiceInfo(const char *p_StrId) {
    char mutexName[AQUA_MUTEX_MEM_SIZE];
    uint64_t nameHash = hashString64(p_StrId);

    snprintf(mutexName, sizeof(mutexName), "__aqua_%016" PRIx64 "%u", nameHash,
             AQUA_SERVICE_INFO_CONNECT_MUTEX_TAG);
    Sync.createMutex(&s_ServiceInfo.connectMutex, mutexName,
                     SERVICE_INFO_CONNECT_MUTEX_HANDLE);

    snprintf(mutexName, sizeof(mutexName), "__aqua_%016" PRIx64 "%u", nameHash,
             AQUA_SERVICE_INFO_CALL_MUTEX_TAG);
    Sync.createMutex(&s_ServiceInfo.callMutex, mutexName,
                     SERVICE_INFO_CALL_MUTEX_HANDLE);

    snprintf(mutexName, sizeof(mutexName), "__aqua_%016" PRIx64 "%u", nameHash,
             AQUA_SERVICE_INFO_CALL_MUTEX_TAG);
    Sync.createMutex(&s_ServiceInfo.callMutex, mutexName,
                     SERVICE_INFO_CALL_MUTEX_HANDLE);
}

AQUA_API_EXPORT int32_t aquaCreateServerConnectInfo() {
    int32_t handle = -1;
    Sync.mutexLock(&s_ServiceInfo.connectMutex);
    for (int32_t i = 0; i < SERVICE_INFO_MAX; ++i) {
        if (!s_ServiceInfo.connectInfos[i].used) {
            handle = i;
            s_ServiceInfo.connectInfos[i].used = 1;
            break;
        }
    }
    Sync.mutexUnlock(&s_ServiceInfo.connectMutex);

    return handle;
}

AQUA_API_EXPORT int32_t aquaCreateServerCallInfo() {
    int32_t handle = -1;
    Sync.mutexLock(&s_ServiceInfo.callMutex);
    for (int32_t i = 0; i < SERVICE_INFO_MAX; ++i) {
        if (!s_ServiceInfo.callInfos[i].used) {
            handle = i;
            s_ServiceInfo.callInfos[i].used = 1;
            break;
        }
    }
    Sync.mutexUnlock(&s_ServiceInfo.callMutex);

    return handle;
}

int32_t createReturnInfo() {
    int32_t handle = -1;
    Sync.mutexLock(&s_ServiceInfo.returnMutex);

    for (int32_t i = 0; i < (int32_t)OPENED_CONNECTIONS; ++i) {
        if (!s_ServiceInfo.returnInfos[i].used) {
            handle = i;
            s_ServiceInfo.returnInfos[i].used = 1;
            break;
        }
    }
    Sync.mutexUnlock(&s_ServiceInfo.returnMutex);

    return handle;
}

void freeConnectInfo(int32_t p_ConnectInfoHandle) {
    Sync.mutexLock(&s_ServiceInfo.connectMutex);
    s_ServiceInfo.connectInfos[p_ConnectInfoHandle].used = 0;
    Sync.mutexUnlock(&s_ServiceInfo.connectMutex);
}

void freeCallInfo(int32_t p_CallInfoHandle) {
    Sync.mutexLock(&s_ServiceInfo.callMutex);
    s_ServiceInfo.callInfos[p_CallInfoHandle].used = 0;
    Sync.mutexUnlock(&s_ServiceInfo.callMutex);
}

void freeReturnInfo(int32_t p_ReturnInfoHandle) {
    Sync.mutexLock(&s_ServiceInfo.callMutex);
    s_ServiceInfo.returnInfos[p_ReturnInfoHandle].used = 0;
    Sync.mutexUnlock(&s_ServiceInfo.callMutex);
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

uint32_t getConnectionId(uint32_t p_ReturnHandle) {
    return s_ServiceInfo.returnInfos[p_ReturnHandle]
        .returnInfo.m_ConnectResponseInformation.m_Id;
}
