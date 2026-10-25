// SPDX-License-Identifier: LGPL-2.1-or-later

#include "dsp-client.h"

#include "aqua-types.h"
#include "client-call.h"
#include "client-connect.h"
#include "client-info.h"
#include "commons.h"
#include "dsp.h"
#include "log.h"
#include "platform-types.h"
#include "platform.h"
#include "system-values.h"
#include "utils.h"

#include <string.h>

AQUA_API_EXPORT void aquaConnect(int32_t returnInfoHandle,
                                 int32_t connectInfoHandle,
                                 int32_t connReqInfoHandle) {
    struct ClientConnectInfo *connectInfo =
        getClientConnectInfo(connectInfoHandle);
    struct ClientReturnInfo *returnInfo = getClientReturnInfo(returnInfoHandle);
    struct ClientConnectRequestInformation *requestInfo =
        getClientConnRequestInfo(connReqInfoHandle);

    connectInfo->m_SendConnectRequest(returnInfo, connectInfo, requestInfo);
}

AQUA_API_EXPORT void aquaDisconnect(int32_t connectInfoHandle,
                                    int32_t returnInfoHandle) {
    struct ClientConnectInfo *connectInfo =
        getClientConnectInfo(connectInfoHandle);
    struct ClientReturnInfo *returnInfo = getClientReturnInfo(returnInfoHandle);
    struct ConnectResponseInformation *requestResponseInfo =
        &(returnInfo->m_ConnectResponseInformation);

    connectInfo->m_SendDisconnectRequest(connectInfo, requestResponseInfo);

    aquaDestroyClientConnectInfo(connectInfoHandle);
    aquaDestroyClientReturnInfo(returnInfoHandle);
}

AQUA_API_EXPORT void aquaClientSend(int32_t callInfoHandle, void *data,
                                    uint32_t size) {
    struct ClientCommunicationInfo info;
    struct ClientCallInfo *callInfo = getClientCallInfo(callInfoHandle);
    // struct ClientReturnInfo *returnInfo =
    // getClientReturnInfo(returnInfoHandle);

    info.cInfo.m_Q = &(callInfo->m_Q);
    info.cInfo.m_Data = data;
    info.dataSize = size;
    // info.info = returnInfo;
    callInfo->m_CallFn(&info);
}

AQUA_API_EXPORT void aquaClientRecv(void *data, int32_t returnInfoHandle) {
    struct CommunicationInfo info;
    struct ClientReturnInfo *returnInfo = getClientReturnInfo(returnInfoHandle);

    info.m_Q = &(returnInfo->m_Q);
    info.m_Data = data;

    returnInfo->m_ReturnFn(&info);
}

static aqua_size_t sf_GetInstallArenaSize() {
    aqua_size_t mapGranularity = Memory.getMapGranularity();

    aqua_size_t alignedHeaderSize =
        alignUp(sizeof(struct InstallInfo), mapGranularity);

    aqua_size_t alignedServiceSize =
        alignUp(sizeof(struct InstallInformation), mapGranularity);

    return alignedHeaderSize + alignedServiceSize * SERVICES_NUMBER;
}

static aqua_size_t sf_GetServiceOff(uint16_t i) {
    aqua_size_t mapGranularity = Memory.getMapGranularity();

    size_t alignedHeaderSize =
        alignUp(sizeof(struct InstallInfo), mapGranularity);

    aqua_size_t alignedServiceSize =
        alignUp(sizeof(struct InstallInformation), mapGranularity);

    return alignedHeaderSize + i * alignedServiceSize;
}

static struct InstallInformation *sf_GetService(struct InstallInfo *info,
                                                uint16_t i) {
    return (struct InstallInformation *)((aqua_u8_t *)info +
                                         sf_GetServiceOff(i));
}

AQUA_API_EXPORT void aquaInitClientConnection(int32_t connectInfoHandle,
                                              int32_t callInfoHandle,
                                              const char *serviceStrId) {
    int rc;
    aqua_file_handle_t installShmFd;
    aqua_err_t err;
    struct InstallInformation *installInfo;
    uint8_t connected = false;
    uint16_t i;
    struct ClientConnectInfo *connectInfo =
        getClientConnectInfo(connectInfoHandle);
    struct ClientCallInfo *callInfo = getClientCallInfo(callInfoHandle);

    aqua_size_t installArenaSize = sf_GetInstallArenaSize();

    installShmFd = SharedMemoryObject.create(
        INSTALL_MZONE, AQUA_FILE_PERM_RDWR,
        AQUA_FILE_MODE_USER_READ | AQUA_FILE_MODE_USER_WRITE |
            AQUA_FILE_MODE_GROUP_READ | AQUA_FILE_MODE_GROUP_WRITE |
            AQUA_FILE_MODE_OTHER_READ | AQUA_FILE_MODE_OTHER_WRITE,
        installArenaSize, false);
    DIE(installShmFd < 0,
        "Could not open install memory zone shared memory object");

    struct InstallInfo *installMemZone = NULL;
    err = Allocator.memmap((aqua_void_ptr_t *)&installMemZone, NULL,
                           installArenaSize,
                           AQUA_MEM_PROT_READ | AQUA_MEM_PROT_WRITE,
                           AQUA_MEM_SHARED, installShmFd, 0);

    DIE(err == AQUA_MEM_MAP_FAILED, "Could not mmap install memory zone");

    for (i = 0; i < SERVICES_NUMBER; ++i) {
        installInfo = sf_GetService(installMemZone, i);

        if (!installInfo->m_Available) {
            continue;
        }

        if (!strcmp(installInfo->m_StrId, serviceStrId)) {
            if (installInfo->m_Available) {
                connected = true;
            }
            break;
        }
    }

    if (!connected) {
        ELOGF("Could not connect. Service is not installed or unavailable.\n");
        return;
    }

    rc = Allocator.memunmap(installMemZone, sizeof(struct InstallInfo));
    DIE(rc != 0, "Could not unmap install memory zone");

    /**
     * Map only the information of the service
     */
    err = Allocator.memmap(
        (aqua_void_ptr_t *)&installInfo, NULL,
        alignUp(sizeof(struct InstallInformation), Memory.getMapGranularity()),
        AQUA_MEM_PROT_READ | AQUA_MEM_PROT_WRITE, AQUA_MEM_SHARED, installShmFd,
        sf_GetServiceOff(i));
    DIE(err == AQUA_MEM_MAP_FAILED, "Could not map service information");

    configureClientConnectInformation(connectInfo, installInfo);
    configureClientCallInformation(callInfo, installInfo);
}

AQUA_API_EXPORT void
aquaSetClientConnRequestInfo(int32_t connRequestHandle, const char *returnQName,
                             uint32_t returnQSize, const char *reqResponseQName,
                             uint32_t responseQSize, enum QType qType) {
    struct ClientConnectRequestInformation *requestInfo =
        getClientConnRequestInfo(connRequestHandle);

    memset(requestInfo->m_ReturnQName, 0, RETURNQ_NAME_MAX_SIZE);
    memcpy(requestInfo->m_ReturnQName, returnQName, strlen(returnQName));
    requestInfo->m_ReturnQSize = returnQSize;

    memset(requestInfo->m_RequestResponseQName, 0, RETURNQ_NAME_MAX_SIZE);
    memcpy(requestInfo->m_RequestResponseQName, reqResponseQName,
           strlen(reqResponseQName));
    requestInfo->m_ResponseQSize = responseQSize;

    requestInfo->m_QType = qType;
}
