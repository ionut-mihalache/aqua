// SPDX-License-Identifier: LGPL-2.1-or-later

#include "aqua-types.h"
#include "dsp-service.h"
#include "dsp.h"
#include <pthread.h>

#define SERVICE_INFO_MAX 8 // maybe will be defined in the configuration file

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

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
} s_ServiceInfo;

int32_t aquaCreateConnectInfo() {
    int32_t handle = -1;
    pthread_mutex_lock(&mutex);
    for (int32_t i = 0; i < SERVICE_INFO_MAX; ++i) {
        if (!s_ServiceInfo.connectInfos[i].used) {
            handle = i;
            s_ServiceInfo.connectInfos[i].used = 1;
            break;
        }
    }
    pthread_mutex_unlock(&mutex);

    return handle;
}

int32_t aquaCreateCallInfo() {
    int32_t handle = -1;
    pthread_mutex_lock(&mutex);
    for (int32_t i = 0; i < SERVICE_INFO_MAX; ++i) {
        if (!s_ServiceInfo.callInfos[i].used) {
            handle = i;
            s_ServiceInfo.callInfos[i].used = 1;
            break;
        }
    }
    pthread_mutex_unlock(&mutex);

    return handle;
}

int32_t createReturnInfo() {
    int32_t handle = -1;
    pthread_mutex_lock(&mutex);

    // for (int32_t i = 0; i < (int32_t)OPENED_CONNECTIONS; ++i) {
    //     LOGF("[%d] used=%u\n", i, (unsigned)s_ServiceInfo.returnInfos[i].used);
    // }

    for (int32_t i = 0; i < (int32_t)OPENED_CONNECTIONS; ++i) {
        if (!s_ServiceInfo.returnInfos[i].used) {
            handle = i;
            s_ServiceInfo.returnInfos[i].used = 1;
            break;
        }
    }
    pthread_mutex_unlock(&mutex);

    return handle;
}

void freeConnectInfo(int32_t p_ConnectInfoHandle) {
    pthread_mutex_lock(&mutex);
    s_ServiceInfo.connectInfos[p_ConnectInfoHandle].used = 0;
    pthread_mutex_unlock(&mutex);
}

void freeCallInfo(int32_t p_CallInfoHandle) {
    pthread_mutex_lock(&mutex);
    s_ServiceInfo.callInfos[p_CallInfoHandle].used = 0;
    pthread_mutex_unlock(&mutex);
}

void freeReturnInfo(int32_t p_ReturnInfoHandle) {
    pthread_mutex_lock(&mutex);
    s_ServiceInfo.returnInfos[p_ReturnInfoHandle].used = 0;
    pthread_mutex_unlock(&mutex);
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
