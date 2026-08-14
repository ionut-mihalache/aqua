// SPDX-License-Identifier: LGPL-2.1-or-later

#include <string.h>

#include "client-connect.h"
#include "aqua-types.h"
#include "commons.h"
#include "dsp.h"
#include "log.h"
#include "macros.h"
#include "platform-types.h"
#include "platform.h"
#include "system-values.h"

static int32_t s_ReturnFnSMBFn(void *p_RI, struct DSPQueue *p_Q) {
    int32_t rc = 0;
    struct SMBCall *qData = p_Q->m_Data;

    memcpy(p_RI, &qData[*p_Q->m_Metadata.m_PopIdxPtr], sizeof(struct SMBCall));

    return rc;
}

static int32_t s_ReturnFnEMBFn(void *p_RI, struct DSPQueue *p_Q) {
    int32_t rc = 0;
    struct EMBCall *qData = p_Q->m_Data;

    memcpy(p_RI, &qData[*p_Q->m_Metadata.m_PopIdxPtr], sizeof(struct EMBCall));

    return rc;
}

static int32_t s_ReturnFnQMBFn(void *p_RI, struct DSPQueue *p_Q) {
    int32_t rc = 0;
    struct QMBCall *qData = p_Q->m_Data;

    memcpy(p_RI, &qData[*p_Q->m_Metadata.m_PopIdxPtr], sizeof(struct QMBCall));

    return rc;
}

static int32_t s_ReturnFnHMBFn(void *p_RI, struct DSPQueue *p_Q) {
    int32_t rc = 0;
    struct HMBCall *qData = p_Q->m_Data;

    memcpy(p_RI, &qData[*p_Q->m_Metadata.m_PopIdxPtr], sizeof(struct HMBCall));

    return rc;
}

static int32_t s_ReturnFnMBFn(void *p_RI, struct DSPQueue *p_Q) {
    int32_t rc = 0;
    struct MBCall *qData = p_Q->m_Data;

    memcpy(p_RI, &qData[*p_Q->m_Metadata.m_PopIdxPtr], sizeof(struct MBCall));

    return rc;
}

static int32_t s_ReturnFnDMBFn(void *p_RI, struct DSPQueue *p_Q) {
    int32_t rc = 0;
    struct DMBCall *qData = p_Q->m_Data;

    memcpy(p_RI, &qData[*p_Q->m_Metadata.m_PopIdxPtr], sizeof(struct DMBCall));

    return rc;
}

static int32_t s_ReturnFnHGBFn(void *p_RI, struct DSPQueue *p_Q) {
    int32_t rc = 0;
    struct HGBCall *qData = p_Q->m_Data;

    memcpy(p_RI, &qData[*p_Q->m_Metadata.m_PopIdxPtr], sizeof(struct HGBCall));

    return rc;
}

static int32_t s_ReturnFnGBFn(void *p_RI, struct DSPQueue *p_Q) {
    int32_t rc = 0;
    struct GBCall *qData = p_Q->m_Data;

    memcpy(p_RI, &qData[*p_Q->m_Metadata.m_PopIdxPtr], sizeof(struct GBCall));

    return rc;
}

static int32_t s_ReturnFnA(struct DSPQueue *p_Q, void *p_RI,
                           uint32_t p_QMaxSize,
                           int32_t (*p_Fn)(void *, struct DSPQueue *)) {
    int32_t rc = 0;

    QPOP(p_Q, p_QMaxSize, do { rc = p_Fn(p_RI, p_Q); } while (0));

    return rc;
}

static int32_t s_ReturnFnSMB(struct SMBCall *p_RI, struct DSPQueue *p_Q) {
    return s_ReturnFnA(p_Q, p_RI, RETURNQ_MAX_SIZE, s_ReturnFnSMBFn);
}

static int32_t s_ReturnFnEMB(struct EMBCall *p_RI, struct DSPQueue *p_Q) {
    return s_ReturnFnA(p_Q, p_RI, RETURNQ_MAX_SIZE, s_ReturnFnEMBFn);
}

static int32_t s_ReturnFnQMB(struct QMBCall *p_RI, struct DSPQueue *p_Q) {
    return s_ReturnFnA(p_Q, p_RI, RETURNQ_MAX_SIZE, s_ReturnFnQMBFn);
}

static int32_t s_ReturnFnHMB(struct HMBCall *p_RI, struct DSPQueue *p_Q) {
    return s_ReturnFnA(p_Q, p_RI, RETURNQ_MAX_SIZE, s_ReturnFnHMBFn);
}

static int32_t s_ReturnFnMB(struct MBCall *p_RI, struct DSPQueue *p_Q) {
    return s_ReturnFnA(p_Q, p_RI, RETURNQ_MAX_SIZE, s_ReturnFnMBFn);
}

static int32_t s_ReturnFnDMB(struct DMBCall *p_RI, struct DSPQueue *p_Q) {
    return s_ReturnFnA(p_Q, p_RI, RETURNQ_MAX_SIZE, s_ReturnFnDMBFn);
}

static int32_t s_ReturnFnHGB(struct HGBCall *p_RI, struct DSPQueue *p_Q) {
    return s_ReturnFnA(p_Q, p_RI, RETURNQ_MAX_SIZE, s_ReturnFnHGBFn);
}

static int32_t s_ReturnFnGB(struct GBCall *p_RI, struct DSPQueue *p_Q) {
    return s_ReturnFnA(p_Q, p_RI, RETURNQ_MAX_SIZE, s_ReturnFnGBFn);
}

static int32_t s_QPop(struct CommunicationInfo *p_CmI) {
    switch (p_CmI->m_Q->m_Type) {
    case SMBQ:
        return s_ReturnFnSMB((struct SMBCall *)p_CmI->m_Data, p_CmI->m_Q);
    case EMBQ:
        return s_ReturnFnEMB((struct EMBCall *)p_CmI->m_Data, p_CmI->m_Q);
    case QMBQ:
        return s_ReturnFnQMB((struct QMBCall *)p_CmI->m_Data, p_CmI->m_Q);
    case HMBQ:
        return s_ReturnFnHMB((struct HMBCall *)p_CmI->m_Data, p_CmI->m_Q);
    case MBQ:
        return s_ReturnFnMB((struct MBCall *)p_CmI->m_Data, p_CmI->m_Q);
    case DMBQ:
        return s_ReturnFnDMB((struct DMBCall *)p_CmI->m_Data, p_CmI->m_Q);
    case HGBQ:
        return s_ReturnFnHGB((struct HGBCall *)p_CmI->m_Data, p_CmI->m_Q);
    case GBQ:
        return s_ReturnFnGB((struct GBCall *)p_CmI->m_Data, p_CmI->m_Q);
    default:
        /**
         * TODO
         */
        return (-1);
    }
}

static int32_t
s_ProcessConnectionRequest(uint32_t p_ConnId, struct ClientReturnInfo *p_RI,
                           struct ConnectRequest *p_ConnRq,
                           struct ClientConnectInfo *p_ConnI,
                           struct ClientConnectRequestInformation *p_ConnInf) {
    aqua_file_handle_t requestResponseQFd;
    aqua_err_t err;
    aqua_file_handle_t returnQFd;
    int32_t rc = 0;
    aqua_file_flags_t qFlag;
    aqua_mem_prot_t qProt;
    aqua_file_mode_t qMode;
    size_t qSize;
    void *returnQ;

    /**
     * With the connection index found we need to construct the request for the
     * service
     */
    p_ConnRq->m_ConnectionIdx = p_ConnId;

    memcpy(p_ConnI->m_Connections[p_ConnId].m_RequestResponseQName,
           p_ConnInf->m_ReturnQName, strlen(p_ConnInf->m_ReturnQName));
    memcpy(p_ConnI->m_Connections[p_ConnId].m_ReturnQName,
           p_ConnInf->m_RequestResponseQName,
           strlen(p_ConnInf->m_RequestResponseQName));

    memset(p_ConnRq->m_ReturnQName, 0, RETURNQ_NAME_MAX_SIZE);
    memcpy(p_ConnRq->m_ReturnQName, p_ConnInf->m_ReturnQName,
           strlen(p_ConnInf->m_ReturnQName));

    memset(p_ConnRq->m_RequestResponseQName, 0, RETURNQ_NAME_MAX_SIZE);
    memcpy(p_ConnRq->m_RequestResponseQName, p_ConnInf->m_RequestResponseQName,
           strlen(p_ConnInf->m_RequestResponseQName));

    p_ConnRq->m_ReturnQType = p_ConnInf->m_QType;

    p_RI->m_Q.m_MaxSize = RETURNQ_MAX_SIZE;
    p_ConnRq->m_ReturnQSize =
        RETURNQ_MAX_SIZE; // CHECK: possibly user specified

    p_RI->m_ResponseQueue.m_MaxSize = RETURN_RESPONSEQ_MAX_SIZE;
    p_ConnRq->m_ResponseQSize =
        RETURN_RESPONSEQ_MAX_SIZE; // CHECK: possibly user specified

    requestResponseQFd = SharedMemoryObject.create(
        p_ConnInf->m_RequestResponseQName, AQUA_FILE_PERM_RDWR,
        AQUA_FILE_MODE_USER_READ | AQUA_FILE_MODE_USER_WRITE |
            AQUA_FILE_MODE_GROUP_READ | AQUA_FILE_MODE_GROUP_WRITE |
            AQUA_FILE_MODE_OTHER_READ | AQUA_FILE_MODE_OTHER_WRITE,
        p_ConnInf->m_ResponseQSize * sizeof(struct ConnectResponseInformation),
        true);
    DIE(requestResponseQFd < 0, "Could not create shared memory object");

    struct ConnectResponseInformation *requestResponseQ;
    err = Allocator.memmap(
        (aqua_void_ptr_t *)&requestResponseQ, NULL,
        p_ConnInf->m_ResponseQSize * sizeof(struct ConnectResponseInformation),
        AQUA_MEM_PROT_READ, AQUA_MEM_SHARED, requestResponseQFd, 0);
    DIE(err == AQUA_MEM_MAP_FAILED,
        "Could not map request response queue memory");

    Memory.triggerPageFaults(requestResponseQ,
                             p_ConnInf->m_ResponseQSize *
                                 sizeof(struct ConnectResponseInformation),
                             AQUA_MEM_PROT_READ);

    err = SharedMemoryObject.close(requestResponseQFd);
    DIE(err == AQUA_SHM_OBJ_CLOSE_FAILED, "Could not close requestResponseQFd");

    switch (p_ConnInf->m_QType) {
    case SMBQ:
        qFlag = AQUA_FILE_PERM_RDWR;
        qMode = AQUA_FILE_MODE_USER_READ | AQUA_FILE_MODE_USER_WRITE |
                AQUA_FILE_MODE_GROUP_READ | AQUA_FILE_MODE_GROUP_WRITE |
                AQUA_FILE_MODE_OTHER_READ | AQUA_FILE_MODE_OTHER_WRITE;
        qSize = p_ConnInf->m_ReturnQSize * sizeof(struct SMBCall);
        qProt = AQUA_MEM_PROT_READ;

        break;
    case EMBQ:
        qFlag = AQUA_FILE_PERM_RDWR;
        qMode = AQUA_FILE_MODE_USER_READ | AQUA_FILE_MODE_USER_WRITE |
                AQUA_FILE_MODE_GROUP_READ | AQUA_FILE_MODE_GROUP_WRITE |
                AQUA_FILE_MODE_OTHER_READ | AQUA_FILE_MODE_OTHER_WRITE;
        qSize = p_ConnInf->m_ReturnQSize * sizeof(struct EMBCall);
        qProt = AQUA_MEM_PROT_READ;

        break;
    case QMBQ:
        qFlag = AQUA_FILE_PERM_RDWR;
        qMode = AQUA_FILE_MODE_USER_READ | AQUA_FILE_MODE_USER_WRITE |
                AQUA_FILE_MODE_GROUP_READ | AQUA_FILE_MODE_GROUP_WRITE |
                AQUA_FILE_MODE_OTHER_READ | AQUA_FILE_MODE_OTHER_WRITE;
        qSize = p_ConnInf->m_ReturnQSize * sizeof(struct QMBCall);
        qProt = AQUA_MEM_PROT_READ;

        break;
    case HMBQ:
        qFlag = AQUA_FILE_PERM_RDWR;
        qMode = AQUA_FILE_MODE_USER_READ | AQUA_FILE_MODE_USER_WRITE |
                AQUA_FILE_MODE_GROUP_READ | AQUA_FILE_MODE_GROUP_WRITE |
                AQUA_FILE_MODE_OTHER_READ | AQUA_FILE_MODE_OTHER_WRITE;
        qSize = p_ConnInf->m_ReturnQSize * sizeof(struct HMBCall);
        qProt = AQUA_MEM_PROT_READ;

        break;
    case MBQ:
        qFlag = AQUA_FILE_PERM_RDWR;
        qMode = AQUA_FILE_MODE_USER_READ | AQUA_FILE_MODE_USER_WRITE |
                AQUA_FILE_MODE_GROUP_READ | AQUA_FILE_MODE_GROUP_WRITE |
                AQUA_FILE_MODE_OTHER_READ | AQUA_FILE_MODE_OTHER_WRITE;
        qSize = p_ConnInf->m_ReturnQSize * sizeof(struct MBCall);
        qProt = AQUA_MEM_PROT_READ;

        break;
    case DMBQ:
        qFlag = AQUA_FILE_PERM_RDWR;
        qMode = AQUA_FILE_MODE_USER_READ | AQUA_FILE_MODE_USER_WRITE |
                AQUA_FILE_MODE_GROUP_READ | AQUA_FILE_MODE_GROUP_WRITE |
                AQUA_FILE_MODE_OTHER_READ | AQUA_FILE_MODE_OTHER_WRITE;
        qSize = p_ConnInf->m_ReturnQSize * sizeof(struct DMBCall);
        qProt = AQUA_MEM_PROT_READ;

        break;
    case HGBQ:
        qFlag = AQUA_FILE_PERM_RDWR;
        qMode = AQUA_FILE_MODE_USER_READ | AQUA_FILE_MODE_USER_WRITE |
                AQUA_FILE_MODE_GROUP_READ | AQUA_FILE_MODE_GROUP_WRITE |
                AQUA_FILE_MODE_OTHER_READ | AQUA_FILE_MODE_OTHER_WRITE;
        qSize = p_ConnInf->m_ReturnQSize * sizeof(struct HGBCall);
        qProt = AQUA_MEM_PROT_READ;

        break;
    case GBQ:
        qFlag = AQUA_FILE_PERM_RDWR;
        qMode = AQUA_FILE_MODE_USER_READ | AQUA_FILE_MODE_USER_WRITE |
                AQUA_FILE_MODE_GROUP_READ | AQUA_FILE_MODE_GROUP_WRITE |
                AQUA_FILE_MODE_OTHER_READ | AQUA_FILE_MODE_OTHER_WRITE;
        qSize = p_ConnInf->m_ReturnQSize * sizeof(struct GBCall);
        qProt = AQUA_MEM_PROT_READ;

        break;
    default:
        /**
         * TODO
         */
        DIE(true, "QType is not recognized");
    }

    returnQFd = SharedMemoryObject.create(p_ConnInf->m_ReturnQName, qFlag,
                                          qMode, qSize, true);

    createQ(&returnQ, qSize, qProt, returnQFd);

    Memory.triggerPageFaults(returnQ, qSize, qProt);

    err = SharedMemoryObject.close(requestResponseQFd);
    DIE(err == AQUA_SHM_OBJ_CLOSE_FAILED, "Could not close returnQFd");

    p_ConnI->m_Connections[p_ConnId].m_RequestResponseQPushIdx = 0;
    p_ConnI->m_Connections[p_ConnId].m_RequestResponseQPopIdx = 0;
    p_ConnI->m_Connections[p_ConnId].m_RequestResponseQSize = 0;

    p_RI->m_ResponseQueue.m_Data = requestResponseQ;
    p_RI->m_ResponseQueue.m_Metadata.m_FullCond =
        &p_ConnI->m_Connections[p_ConnId].m_RequestResponseQFullCond;
    p_RI->m_ResponseQueue.m_Metadata.m_EmptyCond =
        &p_ConnI->m_Connections[p_ConnId].m_RequestResponseQEmptyCond;
    p_RI->m_ResponseQueue.m_Metadata.m_Lock =
        &p_ConnI->m_Connections[p_ConnId].m_RequestResponseQMutex;
    p_RI->m_ResponseQueue.m_Metadata.m_PushIdxPtr =
        &p_ConnI->m_Connections[p_ConnId].m_RequestResponseQPushIdx;
    p_RI->m_ResponseQueue.m_Metadata.m_PopIdxPtr =
        &p_ConnI->m_Connections[p_ConnId].m_RequestResponseQPopIdx;
    p_RI->m_ResponseQueue.m_Metadata.m_Size =
        &p_ConnI->m_Connections[p_ConnId].m_RequestResponseQSize;

    p_RI->m_Q.m_Data = returnQ;
    p_RI->m_Q.m_Metadata.m_FullCond =
        &p_ConnI->m_Connections[p_ConnId].m_ReturnQFullCond;
    p_RI->m_Q.m_Metadata.m_EmptyCond =
        &p_ConnI->m_Connections[p_ConnId].m_ReturnQEmptyCond;
    p_RI->m_Q.m_Metadata.m_Lock =
        &p_ConnI->m_Connections[p_ConnId].m_ReturnQMutex;
    p_RI->m_Q.m_Metadata.m_PushIdxPtr =
        &p_ConnI->m_Connections[p_ConnId].m_ReturnQPushIdx;
    p_RI->m_Q.m_Metadata.m_PopIdxPtr =
        &p_ConnI->m_Connections[p_ConnId].m_ReturnQPopIdx;
    p_RI->m_Q.m_Metadata.m_Size =
        &p_ConnI->m_Connections[p_ConnId].m_ReturnQSize;
    p_RI->m_Q.m_Type = p_ConnInf->m_QType;

    p_RI->m_ReturnFn = s_QPop;

    return rc;
}

static int32_t
s_SendConnectRequest(struct ClientReturnInfo *p_ReturnInfo,
                     struct ClientConnectInfo *p_ConnectInfo,
                     struct ClientConnectRequestInformation *p_RequestInfo) {
    int32_t rc = 0;
    uint32_t idx;
    uint32_t connId;
    struct ConnectQueue *queue = &p_ConnectInfo->m_ConnectQ;

    /**
     *  search for a free spot in the opened connections for the service
     *  send the request to the service with the connection index
     *  WIP: wait for the service to establish the connection on its side
     */
    Sync.mutexLock(p_ConnectInfo->m_ConnectLock);
    for (connId = 0; connId < OPENED_CONNECTIONS; ++connId) {
        if (!p_ConnectInfo->m_Connections[connId].m_Connected) {
            p_ConnectInfo->m_Connections[connId].m_Connected = true;
            break;
        }
    }
    Sync.mutexUnlock(p_ConnectInfo->m_ConnectLock);

    QPUSH(
        queue, CONNECTQ_MAX_SIZE, do {
            idx = *queue->m_Metadata.m_PushIdxPtr;
            s_ProcessConnectionRequest(connId, p_ReturnInfo,
                                       &queue->m_Data[idx], p_ConnectInfo,
                                       p_RequestInfo);
        } while (0));

    /**
     * Wait for the response from the service to announce that the communication
     * is established
     */
    QPOP(
        &p_ReturnInfo->m_ResponseQueue, p_ReturnInfo->m_ResponseQueue.m_MaxSize,
        do {
            /**
             * WIP: Add the information to the response queue. Now the signal is
             * enough
             */
            idx = *p_ReturnInfo->m_ResponseQueue.m_Metadata.m_PopIdxPtr;

            memcpy(&p_ReturnInfo->m_ConnectResponseInformation,
                   &p_ReturnInfo->m_ResponseQueue.m_Data[idx],
                   sizeof(struct ConnectResponseInformation));
        } while (0));

    return rc;
}

static int32_t
s_SendDisconnectRequest(struct ClientConnectInfo *p_ConnectInfo,
                        struct ConnectResponseInformation *p_ResponseInfo) {
    int32_t rc = 0;
    uint32_t idx;
    uint32_t connId;

    struct DisconnectQueue *queue = &p_ConnectInfo->m_DisconnectQ;

    QPUSH(
        queue, CONNECTQ_MAX_SIZE, do {
            idx = *queue->m_Metadata.m_PushIdxPtr;

            connId = p_ResponseInfo->m_Id;

            queue->m_Data[idx].m_ConnectionIdx = connId;
        } while (0));

    return rc;
}

int32_t configureClientConnectInformation(struct ClientConnectInfo *p_ConnI,
                                          struct InstallInformation *p_InI) {
    int32_t rc = 0;
    aqua_err_t err;
    aqua_file_handle_t connectQFd, disconnectQFd;

    connectQFd = SharedMemoryObject.create(
        p_InI->m_ConnectQName, AQUA_FILE_PERM_RDWR,
        AQUA_FILE_MODE_USER_READ | AQUA_FILE_MODE_USER_WRITE |
            AQUA_FILE_MODE_GROUP_READ | AQUA_FILE_MODE_GROUP_WRITE |
            AQUA_FILE_MODE_OTHER_READ | AQUA_FILE_MODE_OTHER_WRITE,
        CONNECTQ_MAX_SIZE * sizeof(struct ConnectRequest), false);

    struct ConnectRequest *connectQ = NULL;
    err = Allocator.memmap((aqua_void_ptr_t *)&connectQ, NULL,
                           CONNECTQ_MAX_SIZE * sizeof(struct ConnectRequest),
                           AQUA_MEM_PROT_READ | AQUA_MEM_PROT_WRITE,
                           AQUA_MEM_SHARED, connectQFd, 0);
    DIE(err == AQUA_MEM_MAP_FAILED, "Could not map connectQ");

    err = SharedMemoryObject.close(connectQFd);
    DIE(err == AQUA_SHM_OBJ_CLOSE_FAILED, "Could not close connectQFd");

    disconnectQFd = SharedMemoryObject.create(
        p_InI->m_DisconnectQName, AQUA_FILE_PERM_RDWR,
        AQUA_FILE_MODE_USER_READ | AQUA_FILE_MODE_USER_WRITE |
            AQUA_FILE_MODE_GROUP_READ | AQUA_FILE_MODE_GROUP_WRITE |
            AQUA_FILE_MODE_OTHER_READ | AQUA_FILE_MODE_OTHER_WRITE,
        CONNECTQ_MAX_SIZE * sizeof(struct ConnectRequest), false);

    struct ConnectRequest *disconnectQ = NULL;
    err = Allocator.memmap((aqua_void_ptr_t *)&disconnectQ, NULL,
                           CONNECTQ_MAX_SIZE * sizeof(struct ConnectRequest),
                           AQUA_MEM_PROT_READ | AQUA_MEM_PROT_WRITE,
                           AQUA_MEM_SHARED, disconnectQFd, 0);
    DIE(err == AQUA_MEM_MAP_FAILED, "Could not map disconnect queue memory");

    err = SharedMemoryObject.close(disconnectQFd);
    DIE(err == AQUA_SHM_OBJ_CLOSE_FAILED, "Could not close disconnectQFd");

    p_ConnI->m_SendConnectRequest = s_SendConnectRequest;
    p_ConnI->m_Connections = p_InI->m_Connections;
    p_ConnI->m_ConnectQ.m_Data = connectQ;
    p_ConnI->m_ConnectQ.m_Metadata.m_PushIdxPtr = &p_InI->m_ConnectQPushIdx;
    p_ConnI->m_ConnectQ.m_Metadata.m_PopIdxPtr = &p_InI->m_ConnectQPopIdx;
    p_ConnI->m_ConnectQ.m_Metadata.m_Size = &p_InI->m_ConnectQSize;
    p_ConnI->m_ConnectQ.m_Metadata.m_Lock = &p_InI->m_ConnectQMutex;
    p_ConnI->m_ConnectQ.m_Metadata.m_FullCond = &p_InI->m_ConnectQFullCond;
    p_ConnI->m_ConnectQ.m_Metadata.m_EmptyCond = &p_InI->m_ConnectQEmptyCond;
    p_ConnI->m_ConnectLock = &p_InI->m_ConnectListLock;

    p_ConnI->m_SendDisconnectRequest = s_SendDisconnectRequest;
    p_ConnI->m_DisconnectQ.m_Data = disconnectQ;
    p_ConnI->m_DisconnectQ.m_Metadata.m_PushIdxPtr =
        &p_InI->m_DisconnectQPushIdx;
    p_ConnI->m_DisconnectQ.m_Metadata.m_PopIdxPtr = &p_InI->m_DisconnectQPopIdx;
    p_ConnI->m_DisconnectQ.m_Metadata.m_Size = &p_InI->m_DisconnectQSize;
    p_ConnI->m_DisconnectQ.m_Metadata.m_Lock = &p_InI->m_DisconnectQMutex;
    p_ConnI->m_DisconnectQ.m_Metadata.m_FullCond =
        &p_InI->m_DisconnectQFullCond;
    p_ConnI->m_DisconnectQ.m_Metadata.m_EmptyCond =
        &p_InI->m_DisconnectQEmptyCond;

    return rc;
}
