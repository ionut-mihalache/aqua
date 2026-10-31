// SPDX-License-Identifier: LGPL-2.1-or-later

#include "client-info.h"

#include "aqua-sync.h"
#include "aqua-types.h"
#include "commons.h"
#include "dsp-client.h"
#include "platform.h"

#include <inttypes.h>
#include <stdio.h>

#define CLIENT_INFO_MAX 8 // maybe will be defined in the configuration file

static struct ClientInfo {
    struct {
        struct ClientConnectInfo connectInfo;
        aqua_u8_t used;
    } connectInfos[CLIENT_INFO_MAX];

    struct {
        struct ClientCallInfo callInfo;
        aqua_u8_t used;
    } callInfos[CLIENT_INFO_MAX];

    struct {
        struct ClientReturnInfo returnInfo;
        aqua_u8_t used;
    } returnInfos[OPENED_CONNECTIONS];

    struct {
        struct ClientConnectRequestInformation connReqInfo;
        aqua_u8_t used;
    } connReqInfos[CLIENT_INFO_MAX];

    aqua_mutex_t mutex;
} s_ClientInfo;

void initClientInfo(const char *p_StrId) {
    char mutexName[AQUA_MUTEX_MEM_SIZE];
    uint64_t nameHash = hashString64(p_StrId);

    snprintf(mutexName, sizeof(mutexName), "__aqua_%016" PRIx64 "%u", nameHash,
             AQUA_CLIENT_INFO_MUTEX_TAG);
    Sync.createMutex(&s_ClientInfo.mutex, mutexName, CLIENT_INFO_MUTEX_HANDLE);
}

int32_t aquaCreateClientConnectInfo() {
    int32_t handle = -1;

    Sync.mutexLock(&s_ClientInfo.mutex);
    for (int32_t i = 0; i < CLIENT_INFO_MAX; ++i) {
        if (!s_ClientInfo.connectInfos[i].used) {
            handle = i;
            s_ClientInfo.connectInfos[i].used = 1;
            break;
        }
    }
    Sync.mutexUnlock(&s_ClientInfo.mutex);

    return handle;
}

int32_t aquaCreateClientCallInfo() {
    int32_t handle = -1;

    Sync.mutexLock(&s_ClientInfo.mutex);
    for (int32_t i = 0; i < CLIENT_INFO_MAX; ++i) {
        if (!s_ClientInfo.callInfos[i].used) {
            handle = i;
            s_ClientInfo.callInfos[i].used = 1;
            break;
        }
    }
    Sync.mutexUnlock(&s_ClientInfo.mutex);

    return handle;
}

int32_t aquaCreateClientReturnInfo() {
    int32_t handle = -1;

    Sync.mutexLock(&s_ClientInfo.mutex);
    for (int32_t i = 0; i < CLIENT_INFO_MAX; ++i) {
        if (!s_ClientInfo.returnInfos[i].used) {
            handle = i;
            s_ClientInfo.returnInfos[i].used = 1;
            break;
        }
    }
    Sync.mutexUnlock(&s_ClientInfo.mutex);

    return handle;
}

int32_t aquaCreateClientConnRequestInfo() {
    int32_t handle = -1;

    Sync.mutexLock(&s_ClientInfo.mutex);
    for (int32_t i = 0; i < CLIENT_INFO_MAX; ++i) {
        if (!s_ClientInfo.connReqInfos[i].used) {
            handle = i;
            s_ClientInfo.connReqInfos[i].used = 1;
            break;
        }
    }
    Sync.mutexUnlock(&s_ClientInfo.mutex);

    return handle;
}

void aquaDestroyClientConnectInfo(int32_t p_ConnectHandle) {
    Sync.mutexLock(&s_ClientInfo.mutex);
    s_ClientInfo.connectInfos[p_ConnectHandle].used = 0;
    Sync.mutexUnlock(&s_ClientInfo.mutex);
}

void aquaDestroyClientCallInfo(int32_t p_CallHandle) {
    Sync.mutexLock(&s_ClientInfo.mutex);
    s_ClientInfo.callInfos[p_CallHandle].used = 0;
    Sync.mutexUnlock(&s_ClientInfo.mutex);
}

void aquaDestroyClientReturnInfo(int32_t p_ReturnHandle) {
    Sync.mutexLock(&s_ClientInfo.mutex);
    s_ClientInfo.returnInfos[p_ReturnHandle].used = 0;
    Sync.mutexUnlock(&s_ClientInfo.mutex);
}

void aquaDestroyClientConnRequestInfo(int32_t p_ConnRequestHandle) {
    Sync.mutexLock(&s_ClientInfo.mutex);
    s_ClientInfo.connReqInfos[p_ConnRequestHandle].used = 0;
    Sync.mutexUnlock(&s_ClientInfo.mutex);
}

struct ClientConnectInfo *getClientConnectInfo(int32_t p_ConnectHandle) {
    return &s_ClientInfo.connectInfos[p_ConnectHandle].connectInfo;
}

struct ClientCallInfo *getClientCallInfo(int32_t p_CallHandle) {
    return &s_ClientInfo.callInfos[p_CallHandle].callInfo;
}

struct ClientReturnInfo *getClientReturnInfo(int32_t p_ReturnHandle) {
    return &s_ClientInfo.returnInfos[p_ReturnHandle].returnInfo;
}

struct ClientConnectRequestInformation *
getClientConnRequestInfo(int32_t p_ConnRequestHandle) {
    return &s_ClientInfo.connReqInfos[p_ConnRequestHandle].connReqInfo;
}
