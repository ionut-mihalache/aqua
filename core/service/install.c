// SPDX-License-Identifier: LGPL-2.1-or-later

#include <string.h>

#include "aqua-types.h"
#include "commons.h"
#include "install.h"
#include "dsp.h"
#include "macros.h"
#include "platform.h"
#include "return.h"
#include "system-values.h"
#include "utils.h"

int32_t initializeServiceConnections(struct InstallInformation *p_InI) {
    int32_t rc = 0;
    uint32_t i;
    struct ConnectionInformation *connInfo;
    uint32_t mutexId = 0;
    uint32_t condId = 0;
    char mutexName[AQUA_MUTEX_MEM_SIZE];
    char condName[AQUA_COND_MEM_SIZE];

    for (i = 0; i < OPENED_CONNECTIONS; ++i) {
        connInfo = &p_InI->m_Connections[i];

        memset(mutexName, 0, AQUA_MUTEX_MEM_SIZE);
        // TODO: This needs to be checked in order to make sure that the NULL
        // terminator is properly set
        sprintf(mutexName, "__aqua_%s_%u_ret_mutex", p_InI->m_StrId, mutexId);
        mutexId++;
        Sync.createMutex(&connInfo->m_ReturnQMutex, mutexName);

        memset(mutexName, 0, AQUA_MUTEX_MEM_SIZE);
        // TODO: This needs to be checked in order to make sure that the NULL
        // terminator is properly set
        sprintf(mutexName, "__aqua_%s_%u_reqr_mutex", p_InI->m_StrId, mutexId);
        mutexId++;
        Sync.createMutex(&connInfo->m_RequestResponseQMutex, mutexName);

        memset(condName, 0, AQUA_MUTEX_MEM_SIZE);
        // TODO: This needs to be checked in order to make sure that the NULL
        // terminator is properly set
        sprintf(condName, "__aqua_%s_%u_ret_fcond", p_InI->m_StrId, condId);
        condId++;
        Sync.createCond(&connInfo->m_ReturnQFullCond, condName);

        memset(condName, 0, AQUA_MUTEX_MEM_SIZE);
        // TODO: This needs to be checked in order to make sure that the NULL
        // terminator is properly set
        sprintf(condName, "__aqua_%s_%u_ret_econd", p_InI->m_StrId, condId);
        condId++;
        Sync.createCond(&connInfo->m_ReturnQEmptyCond, condName);

        memset(condName, 0, AQUA_MUTEX_MEM_SIZE);
        // TODO: This needs to be checked in order to make sure that the NULL
        // terminator is properly set
        sprintf(condName, "__aqua_%s_%u_req_fcond", p_InI->m_StrId, condId);
        condId++;
        Sync.createCond(&connInfo->m_RequestResponseQFullCond, condName);

        memset(condName, 0, AQUA_MUTEX_MEM_SIZE);
        // TODO: This needs to be checked in order to make sure that the NULL
        // terminator is properly set
        sprintf(condName, "__aqua_%s_%u_req_econd", p_InI->m_StrId, condId);
        condId++;
        Sync.createCond(&connInfo->m_RequestResponseQEmptyCond, condName);
    }

    return rc;
}

static int32_t
s_SendConnectResponse(struct ServiceReturnInfo *p_ReturnInfo,
                      struct ConnectResponseInformation *p_ResponseInfo) {
    int32_t rc = 0;

    /**
     * Send the response to the client to announce that the communication is
     * established
     */
    QPUSH(
        &p_ReturnInfo->m_ResponseQueue, p_ReturnInfo->m_ResponseQueue.m_MaxSize,
        do {
            memcpy(
                &(p_ReturnInfo->m_ResponseQueue.m_Data
                      [*p_ReturnInfo->m_ResponseQueue.m_Metadata.m_PushIdxPtr]),
                p_ResponseInfo, sizeof(struct ConnectResponseInformation));

            memcpy(&p_ReturnInfo->m_ConnectResponseInformation, p_ResponseInfo,
                   sizeof(struct ConnectResponseInformation));
        } while (0));

    return rc;
}

static int32_t
s_ReceiveConnectRequest(struct ServiceReturnInfo *p_ReturnInfo,
                        struct ServiceConnectInfo *p_ConnectInfo) {
    int32_t rc = 0;
    struct ConnectQueue *queue = &p_ConnectInfo->m_ConnectQ;
    struct ConnectResponseInformation responseInfo;

    QPOP(
        queue, CONNECTQ_MAX_SIZE, do {
            configureServiceReturnInformation(
                p_ReturnInfo, p_ConnectInfo,
                &queue->m_Data[*queue->m_Metadata.m_PopIdxPtr]);

            memcpy(responseInfo.m_ReturnQName,
                   queue->m_Data[*queue->m_Metadata.m_PopIdxPtr].m_ReturnQName,
                   RETURNQ_NAME_MAX_SIZE);
            memcpy(responseInfo.m_ReturnRequestQName,
                   queue->m_Data[*queue->m_Metadata.m_PopIdxPtr]
                       .m_RequestResponseQName,
                   RETURNQ_NAME_MAX_SIZE);
            responseInfo.m_Id =
                queue->m_Data[*queue->m_Metadata.m_PopIdxPtr].m_ConnectionIdx;
        } while (0));

    s_SendConnectResponse(p_ReturnInfo, &responseInfo);

    return rc;
}

static int32_t
s_ReceiveDisconnectRequest(struct ServiceConnectInfo *p_ConnectInfo) {
    int32_t rc = 0;
    uint32_t idx;
    uint32_t connId;
    struct DisconnectQueue *queue = &p_ConnectInfo->m_DisconnectQ;

    QPOP(
        queue, CONNECTQ_MAX_SIZE, do {
            idx = *queue->m_Metadata.m_PopIdxPtr;
            connId = queue->m_Data[idx].m_ConnectionIdx;
        } while (0));

    rc = Allocator.memunmap(
        p_ConnectInfo->m_Connections[connId].m_RequestResponseQ,
        p_ConnectInfo->m_Connections[connId].m_RequestResponseQMapSize);
    DIE(rc < 0, "Could not unmap request response queue");
    p_ConnectInfo->m_Connections[connId].m_RequestResponseQ = NULL;

    rc = Allocator.memunmap(
        p_ConnectInfo->m_Connections[connId].m_ReturnQ,
        p_ConnectInfo->m_Connections[connId].m_ReturnQMapSize);
    DIE(rc < 0, "Could not unmap return queue");
    p_ConnectInfo->m_Connections[connId].m_ReturnQ = NULL;

    SharedMemoryObject.destroy(
        p_ConnectInfo->m_Connections[connId].m_RequestResponseQName);

    SharedMemoryObject.destroy(
        p_ConnectInfo->m_Connections[connId].m_ReturnQName);

    Sync.mutexLock(p_ConnectInfo->m_ConnectLock);

    p_ConnectInfo->m_Connections[connId].m_Connected = false;

    Sync.mutexUnlock(p_ConnectInfo->m_ConnectLock);

    return rc;
}

int32_t configureServiceConnectInformation(struct ServiceConnectInfo *p_ConnI,
                                           struct InstallInformation *p_InI) {
    aqua_err_t err;
    int32_t rc = 0;
    aqua_file_handle_t connectQFd, disconnectQFd;
    char condName[AQUA_COND_MEM_SIZE];

    p_InI->m_ConnectQPushIdx = 0;
    p_InI->m_ConnectQPopIdx = 0;
    p_InI->m_ConnectQSize = 0;

    p_InI->m_DisconnectQPushIdx = 0;
    p_InI->m_DisconnectQPopIdx = 0;
    p_InI->m_DisconnectQSize = 0;

    connectQFd = SharedMemoryObject.create(
        p_InI->m_ConnectQName, AQUA_FILE_PERM_RDWR,
        AQUA_FILE_MODE_USER_READ | AQUA_FILE_MODE_USER_WRITE |
            AQUA_FILE_MODE_GROUP_READ | AQUA_FILE_MODE_GROUP_WRITE |
            AQUA_FILE_MODE_OTHER_READ | AQUA_FILE_MODE_OTHER_WRITE,
        CONNECTQ_MAX_SIZE * sizeof(struct ConnectRequest), true);

    struct ConnectRequest *connectQ;
    createQ((void **)&connectQ,
            CONNECTQ_MAX_SIZE * sizeof(struct ConnectRequest),
            AQUA_MEM_PROT_READ | AQUA_MEM_PROT_WRITE, connectQFd);

    Memory.triggerPageFaults(connectQ,
                             CONNECTQ_MAX_SIZE * sizeof(struct ConnectRequest),
                             AQUA_MEM_PROT_READ | AQUA_MEM_PROT_WRITE);

    err = SharedMemoryObject.close(connectQFd);
    DIE(err == AQUA_SHM_OBJ_CLOSE_FAILED, "Could not close connectQFd");

    disconnectQFd = SharedMemoryObject.create(
        p_InI->m_DisconnectQName, AQUA_FILE_PERM_RDWR,
        AQUA_FILE_MODE_USER_READ | AQUA_FILE_MODE_USER_WRITE |
            AQUA_FILE_MODE_GROUP_READ | AQUA_FILE_MODE_GROUP_WRITE |
            AQUA_FILE_MODE_OTHER_READ | AQUA_FILE_MODE_OTHER_WRITE,
        CONNECTQ_MAX_SIZE * sizeof(struct ConnectRequest), true);

    struct ConnectRequest *disconnectQ;
    createQ((void **)&disconnectQ,
            CONNECTQ_MAX_SIZE * sizeof(struct ConnectRequest),
            AQUA_MEM_PROT_READ | AQUA_MEM_PROT_WRITE, connectQFd);

    Memory.triggerPageFaults(disconnectQ,
                             CONNECTQ_MAX_SIZE * sizeof(struct ConnectRequest),
                             AQUA_MEM_PROT_READ | AQUA_MEM_PROT_WRITE);

    err = SharedMemoryObject.close(disconnectQFd);
    DIE(err == AQUA_SHM_OBJ_CLOSE_FAILED, "Could not close disconnectQFd");

    p_ConnI->m_ReceiveConnectRequest = s_ReceiveConnectRequest;
    p_ConnI->m_ConnectQ.m_Data = connectQ;
    p_ConnI->m_ConnectQ.m_Metadata.m_PushIdxPtr = &p_InI->m_ConnectQPushIdx;
    p_ConnI->m_ConnectQ.m_Metadata.m_PopIdxPtr = &p_InI->m_ConnectQPopIdx;
    p_ConnI->m_ConnectQ.m_Metadata.m_Size = &p_InI->m_ConnectQSize;
    p_ConnI->m_Connections = p_InI->m_Connections;

    p_ConnI->m_ReceiveDisconnectRequest = s_ReceiveDisconnectRequest;
    p_ConnI->m_DisconnectQ.m_Data = disconnectQ;
    p_ConnI->m_DisconnectQ.m_Metadata.m_PushIdxPtr =
        &p_InI->m_DisconnectQPushIdx;
    p_ConnI->m_DisconnectQ.m_Metadata.m_PopIdxPtr = &p_InI->m_DisconnectQPopIdx;
    p_ConnI->m_DisconnectQ.m_Metadata.m_Size = &p_InI->m_DisconnectQSize;

    Sync.createMutex(&p_InI->m_ConnectQMutex, "");
    Sync.createMutex(&p_InI->m_DisconnectQMutex, "");
    Sync.createMutex(&p_InI->m_ConnectListLock, "");

    p_ConnI->m_ConnectLock = &p_InI->m_ConnectListLock;

    memset(condName, 0, AQUA_COND_MEM_SIZE);
    // TODO: This needs to be checked in order to make sure that the NULL
    // terminator is properly set
    sprintf(condName, "__aqua_%s_conn_fcond", p_InI->m_StrId);
    Sync.createCond(&p_InI->m_ConnectQFullCond, condName);

    memset(condName, 0, AQUA_COND_MEM_SIZE);
    // TODO: This needs to be checked in order to make sure that the NULL
    // terminator is properly set
    sprintf(condName, "__aqua_%s_conn_econd", p_InI->m_StrId);
    Sync.createCond(&p_InI->m_ConnectQEmptyCond, condName);

    memset(condName, 0, AQUA_COND_MEM_SIZE);
    // TODO: This needs to be checked in order to make sure that the NULL
    // terminator is properly set
    sprintf(condName, "__aqua_%s_disconn_fcond", p_InI->m_StrId);
    Sync.createCond(&p_InI->m_DisconnectQFullCond, condName);

    memset(condName, 0, AQUA_COND_MEM_SIZE);
    // TODO: This needs to be checked in order to make sure that the NULL
    // terminator is properly set
    sprintf(condName, "__aqua_%s_disconn_econd", p_InI->m_StrId);
    Sync.createCond(&p_InI->m_DisconnectQEmptyCond, condName);

    p_ConnI->m_ConnectQ.m_Metadata.m_Lock = &p_InI->m_ConnectQMutex;
    p_ConnI->m_ConnectQ.m_Metadata.m_FullCond = &p_InI->m_ConnectQFullCond;
    p_ConnI->m_ConnectQ.m_Metadata.m_EmptyCond = &p_InI->m_ConnectQEmptyCond;

    p_ConnI->m_DisconnectQ.m_Metadata.m_Lock = &p_InI->m_DisconnectQMutex;
    p_ConnI->m_DisconnectQ.m_Metadata.m_FullCond =
        &p_InI->m_DisconnectQFullCond;
    p_ConnI->m_DisconnectQ.m_Metadata.m_EmptyCond =
        &p_InI->m_DisconnectQEmptyCond;

    return rc;
}
