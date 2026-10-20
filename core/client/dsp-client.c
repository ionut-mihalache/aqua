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

AQUA_API_EXPORT void
aquaConnect(struct ClientReturnInfo *p_ReturnInfo,
            struct ClientConnectInfo *p_ConnectInfo,
            struct ClientConnectRequestInformation *p_RequestInfo) {
    p_ConnectInfo->m_SendConnectRequest(p_ReturnInfo, p_ConnectInfo,
                                        p_RequestInfo);
}

void aquaConnectV2(int32_t returnInfoHandle, int32_t connectInfoHandle,
                   int32_t connReqInfoHandle) {
    struct ClientConnectInfo *connectInfo =
        getClientConnectInfo(connectInfoHandle);
    struct ClientReturnInfo *returnInfo = getClientReturnInfo(returnInfoHandle);
    struct ClientConnectRequestInformation *requestInfo =
        getClientConnRequestInfo(connReqInfoHandle);

    connectInfo->m_SendConnectRequest(returnInfo, connectInfo, requestInfo);
}

AQUA_API_EXPORT void
aquaDisconnect(struct ClientConnectInfo *p_ConnectInfo,
               struct ConnectResponseInformation *p_requestResponseInfo) {
    p_ConnectInfo->m_SendDisconnectRequest(p_ConnectInfo,
                                           p_requestResponseInfo);
}

void aquaDisconnectV2(int32_t connectInfoHandle, int32_t returnInfoHandle) {
    struct ClientConnectInfo *connectInfo =
        getClientConnectInfo(connectInfoHandle);
    struct ClientReturnInfo *returnInfo = getClientReturnInfo(returnInfoHandle);
    struct ConnectResponseInformation *requestResponseInfo =
        &(returnInfo->m_ConnectResponseInformation);

    connectInfo->m_SendDisconnectRequest(connectInfo, requestResponseInfo);

    aquaDestroyClientConnectInfo(connectInfoHandle);
    aquaDestroyClientReturnInfo(returnInfoHandle);
}

AQUA_API_EXPORT void callFn(struct ClientCallInfo *p_CallInfo,
                            void *p_CallData) {
    struct CommunicationInfo cInfo;

    cInfo.m_Q = &(p_CallInfo->m_Q);
    cInfo.m_Data = p_CallData;

    p_CallInfo->m_CallFn(&cInfo);
};

void send(int32_t callInfoHandle, void *data) {
    struct CommunicationInfo info;
    struct ClientCallInfo *callInfo = getClientCallInfo(callInfoHandle);

    info.m_Q = &(callInfo->m_Q);
    info.m_Data = data;

    callInfo->m_CallFn(&info);
}

void sendV2(int32_t callInfoHandle, int32_t returnInfoHandle, void *data,
            uint32_t size) {
    struct CommunicationInfo info;
    struct ClientCallInfo *callInfo = getClientCallInfo(callInfoHandle);
    struct ClientReturnInfo *returnInfo = getClientReturnInfo(returnInfoHandle);

    LOGF("Send queue type: %d\n", callInfo->m_Q.m_Type);
    info.m_Q = &(callInfo->m_Q);

    switch (callInfo->m_Q.m_Type) {
    case SMBQ: {
        struct SMBCall callData;
        LOG("Copying data.\n");
        memcpy(callData.m_CallInfo, data, size);
        LOG("Copied data.\n");
        callData.m_Metadata.m_Size = size;
        callData.m_Metadata.m_ConnId =
            returnInfo->m_ConnectResponseInformation.m_Id;
        info.m_Data = &callData;

        callInfo->m_CallFn(&info);

        break;
    }
    case EMBQ: {
        struct EMBCall callData;
        memcpy(callData.m_CallInfo, data, size);
        callData.m_Metadata.m_Size = size;
        callData.m_Metadata.m_ConnId =
            returnInfo->m_ConnectResponseInformation.m_Id;
        info.m_Data = &callData;

        callInfo->m_CallFn(&info);

        break;
    }
    case QMBQ: {
        struct QMBCall callData;
        memcpy(callData.m_CallInfo, data, size);
        callData.m_Metadata.m_Size = size;
        callData.m_Metadata.m_ConnId =
            returnInfo->m_ConnectResponseInformation.m_Id;
        info.m_Data = &callData;

        callInfo->m_CallFn(&info);

        break;
    }
    case HMBQ: {
        struct HMBCall callData;
        memcpy(callData.m_CallInfo, data, size);
        callData.m_Metadata.m_Size = size;
        callData.m_Metadata.m_ConnId =
            returnInfo->m_ConnectResponseInformation.m_Id;
        info.m_Data = &callData;

        callInfo->m_CallFn(&info);

        break;
    }
    case MBQ: {
        struct MBCall callData;
        memcpy(callData.m_CallInfo, data, size);
        callData.m_Metadata.m_Size = size;
        callData.m_Metadata.m_ConnId =
            returnInfo->m_ConnectResponseInformation.m_Id;
        info.m_Data = &callData;

        callInfo->m_CallFn(&info);

        break;
    }
    case DMBQ: {
        struct DMBCall callData;
        memcpy(callData.m_CallInfo, data, size);
        callData.m_Metadata.m_Size = size;
        callData.m_Metadata.m_ConnId =
            returnInfo->m_ConnectResponseInformation.m_Id;
        info.m_Data = &callData;

        callInfo->m_CallFn(&info);

        break;
    }
    case HGBQ: {
        struct HGBCall callData;
        memcpy(callData.m_CallInfo, data, size);
        callData.m_Metadata.m_Size = size;
        callData.m_Metadata.m_ConnId =
            returnInfo->m_ConnectResponseInformation.m_Id;
        info.m_Data = &callData;

        callInfo->m_CallFn(&info);

        break;
    }
    case GBQ: {
        struct GBCall callData;
        memcpy(callData.m_CallInfo, data, size);
        callData.m_Metadata.m_Size = size;
        callData.m_Metadata.m_ConnId =
            returnInfo->m_ConnectResponseInformation.m_Id;
        info.m_Data = &callData;

        callInfo->m_CallFn(&info);

        break;
    }
    default:
        DIE(true, "Type not recognized");
    }
}

AQUA_API_EXPORT void returnFn(void *p_ReturnData,
                              struct ClientReturnInfo *p_ReturnInfo) {
    struct CommunicationInfo cInfo;

    cInfo.m_Q = &(p_ReturnInfo->m_Q);
    cInfo.m_Data = p_ReturnData;

    p_ReturnInfo->m_ReturnFn(&cInfo);
}

void recv(void *data, int32_t returnInfoHandle) {
    struct CommunicationInfo info;
    struct ClientReturnInfo *returnInfo = getClientReturnInfo(returnInfoHandle);

    info.m_Q = &(returnInfo->m_Q);
    info.m_Data = data;

    returnInfo->m_ReturnFn(&info);
}

AQUA_API_EXPORT int32_t setCallData(int p_Type, void *p_CallInfo,
                                    uint8_t *p_Data, uint32_t p_Size) {
    int32_t rc = 0;

    switch (p_Type) {
    case SMBQ: {
        struct SMBCall *callInfo = p_CallInfo;
        memcpy(callInfo->m_CallInfo, p_Data, p_Size);
        callInfo->m_Metadata.m_Size = p_Size;

        break;
    }
    case EMBQ: {
        struct EMBCall *callInfo = p_CallInfo;
        memcpy(callInfo->m_CallInfo, p_Data, p_Size);
        callInfo->m_Metadata.m_Size = p_Size;

        break;
    }
    case QMBQ: {
        struct QMBCall *callInfo = p_CallInfo;
        memcpy(callInfo->m_CallInfo, p_Data, p_Size);
        callInfo->m_Metadata.m_Size = p_Size;

        break;
    }
    case HMBQ: {
        struct HMBCall *callInfo = p_CallInfo;
        memcpy(callInfo->m_CallInfo, p_Data, p_Size);
        callInfo->m_Metadata.m_Size = p_Size;

        break;
    }
    case MBQ: {
        struct MBCall *callInfo = p_CallInfo;
        memcpy(callInfo->m_CallInfo, p_Data, p_Size);
        callInfo->m_Metadata.m_Size = p_Size;

        break;
    }
    case DMBQ: {
        struct DMBCall *callInfo = p_CallInfo;
        memcpy(callInfo->m_CallInfo, p_Data, p_Size);
        callInfo->m_Metadata.m_Size = p_Size;

        break;
    }
    case HGBQ: {
        struct HGBCall *callInfo = p_CallInfo;
        memcpy(callInfo->m_CallInfo, p_Data, p_Size);
        callInfo->m_Metadata.m_Size = p_Size;

        break;
    }
    case GBQ: {
        struct GBCall *callInfo = p_CallInfo;
        memcpy(callInfo->m_CallInfo, p_Data, p_Size);
        callInfo->m_Metadata.m_Size = p_Size;

        break;
    }
    default:
        DIE(true, "Type not recognized");
    }

    return rc;
}

int32_t setCallDataV2(int p_Type, void *p_CallInfo, uint8_t *p_Data,
                      uint32_t p_Size, int32_t returnInfoHandle) {
    int32_t rc = 0;
    struct ClientReturnInfo *returnInfo = getClientReturnInfo(returnInfoHandle);

    switch (p_Type) {
    case SMBQ: {
        struct SMBCall *callInfo = p_CallInfo;
        memcpy(callInfo->m_CallInfo, p_Data, p_Size);
        callInfo->m_Metadata.m_Size = p_Size;
        callInfo->m_Metadata.m_ConnId =
            returnInfo->m_ConnectResponseInformation.m_Id;

        break;
    }
    case EMBQ: {
        struct EMBCall *callInfo = p_CallInfo;
        memcpy(callInfo->m_CallInfo, p_Data, p_Size);
        callInfo->m_Metadata.m_Size = p_Size;
        callInfo->m_Metadata.m_ConnId =
            returnInfo->m_ConnectResponseInformation.m_Id;

        break;
    }
    case QMBQ: {
        struct QMBCall *callInfo = p_CallInfo;
        memcpy(callInfo->m_CallInfo, p_Data, p_Size);
        callInfo->m_Metadata.m_Size = p_Size;
        callInfo->m_Metadata.m_ConnId =
            returnInfo->m_ConnectResponseInformation.m_Id;

        break;
    }
    case HMBQ: {
        struct HMBCall *callInfo = p_CallInfo;
        memcpy(callInfo->m_CallInfo, p_Data, p_Size);
        callInfo->m_Metadata.m_Size = p_Size;
        callInfo->m_Metadata.m_ConnId =
            returnInfo->m_ConnectResponseInformation.m_Id;

        break;
    }
    case MBQ: {
        struct MBCall *callInfo = p_CallInfo;
        memcpy(callInfo->m_CallInfo, p_Data, p_Size);
        callInfo->m_Metadata.m_Size = p_Size;
        callInfo->m_Metadata.m_ConnId =
            returnInfo->m_ConnectResponseInformation.m_Id;

        break;
    }
    case DMBQ: {
        struct DMBCall *callInfo = p_CallInfo;
        memcpy(callInfo->m_CallInfo, p_Data, p_Size);
        callInfo->m_Metadata.m_Size = p_Size;
        callInfo->m_Metadata.m_ConnId =
            returnInfo->m_ConnectResponseInformation.m_Id;

        break;
    }
    case HGBQ: {
        struct HGBCall *callInfo = p_CallInfo;
        memcpy(callInfo->m_CallInfo, p_Data, p_Size);
        callInfo->m_Metadata.m_Size = p_Size;
        callInfo->m_Metadata.m_ConnId =
            returnInfo->m_ConnectResponseInformation.m_Id;

        break;
    }
    case GBQ: {
        struct GBCall *callInfo = p_CallInfo;
        memcpy(callInfo->m_CallInfo, p_Data, p_Size);
        callInfo->m_Metadata.m_Size = p_Size;
        callInfo->m_Metadata.m_ConnId =
            returnInfo->m_ConnectResponseInformation.m_Id;

        break;
    }
    default:
        DIE(true, "Type not recognized");
    }

    return rc;
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

AQUA_API_EXPORT void aquaInitConnection(struct ClientConnectInfo *p_ConnectInfo,
                                        struct ClientCallInfo *p_CallInfo,
                                        const char *p_ServiceStrId) {
    int rc;
    aqua_file_handle_t installShmFd;
    aqua_err_t err;
    struct InstallInformation *installInfo;
    uint8_t connected = false;
    uint16_t i;

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

        if (!strcmp(installInfo->m_StrId, p_ServiceStrId)) {
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

    configureClientConnectInformation(p_ConnectInfo, installInfo);
    configureClientCallInformation(p_CallInfo, installInfo);
}

void retriveInitInformation(struct ClientConnectInfo *p_ConnectInfo,
                            struct ClientCallInfo *p_CallInfo,
                            const char *p_ServiceStrId) {
    aquaInitConnection(p_ConnectInfo, p_CallInfo, p_ServiceStrId);
}

void aquaInitConnectionV2(int32_t connectInfoHandle, int32_t callInfoHandle,
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

struct ConnectResponseInformation *
getConnectResponse(struct ClientReturnInfo *p_ReturnInfo) {
    if (*p_ReturnInfo->m_ResponseQueue.m_Metadata.m_Size == 0) {
        return &p_ReturnInfo->m_ResponseQueue.m_Data[0];
    }

    return NULL;
}

void aquaSetClientConnRequestInfo(int32_t connRequestHandle,
                                  const char *returnQName, uint32_t returnQSize,
                                  const char *reqResponseQName,
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
