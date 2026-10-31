// SPDX-License-Identifier: LGPL-2.1-or-later

#include "return.h"

#include "aqua-types.h"
#include "commons.h"
#include "dsp-service.h"
#include "dsp.h"
#include "log.h"
#include "macros.h"
#include "platform.h"
#include "system-values.h"
#include "utils.h"

#include <string.h>

static int32_t s_SendReturnFnSMBFn(struct DSPQueue *p_Q, void *p_RI,
                                   uint32_t p_Size) {
    int32_t rc = 0;
    uint32_t idx;
    struct SMBCall *qData;

    if (!p_Q || !p_Q->m_Data || !p_Q->m_Metadata.m_PushIdxPtr ||
        !p_Q->m_Metadata.m_PopIdxPtr || !p_RI) {
        ELOGF("queue or return data is not pointer is invalid.\n");
        rc = -1;
        goto end;
    }

    idx = *p_Q->m_Metadata.m_PushIdxPtr;

    qData = p_Q->m_Data;

    qData[idx].m_Metadata.m_Size = p_Size;
    memcpy(&qData[idx].m_CallInfo, p_RI, p_Size);

end:
    return rc;
}

static int32_t s_SendReturnFnEMBFn(struct DSPQueue *p_Q, void *p_RI,
                                   uint32_t p_Size) {
    int32_t rc = 0;
    struct EMBCall *qData = p_Q->m_Data;

    qData[*p_Q->m_Metadata.m_PushIdxPtr].m_Metadata.m_Size = p_Size;
    memcpy(&qData[*p_Q->m_Metadata.m_PushIdxPtr].m_CallInfo, p_RI, p_Size);

    return rc;
}

static int32_t s_SendReturnFnQMBFn(struct DSPQueue *p_Q, void *p_RI,
                                   uint32_t p_Size) {
    int32_t rc = 0;
    struct QMBCall *qData = p_Q->m_Data;

    qData[*p_Q->m_Metadata.m_PushIdxPtr].m_Metadata.m_Size = p_Size;
    memcpy(&qData[*p_Q->m_Metadata.m_PushIdxPtr].m_CallInfo, p_RI, p_Size);

    return rc;
}

static int32_t s_SendReturnFnHMBFn(struct DSPQueue *p_Q, void *p_RI,
                                   uint32_t p_Size) {
    int32_t rc = 0;
    struct HMBCall *qData = p_Q->m_Data;

    qData[*p_Q->m_Metadata.m_PushIdxPtr].m_Metadata.m_Size = p_Size;
    memcpy(&qData[*p_Q->m_Metadata.m_PushIdxPtr].m_CallInfo, p_RI, p_Size);

    return rc;
}

static int32_t s_SendReturnFnMBFn(struct DSPQueue *p_Q, void *p_RI,
                                  uint32_t p_Size) {
    int32_t rc = 0;
    struct MBCall *qData = p_Q->m_Data;

    qData[*p_Q->m_Metadata.m_PushIdxPtr].m_Metadata.m_Size = p_Size;
    memcpy(&qData[*p_Q->m_Metadata.m_PushIdxPtr].m_CallInfo, p_RI, p_Size);

    return rc;
}

static int32_t s_SendReturnFnDMBFn(struct DSPQueue *p_Q, void *p_RI,
                                   uint32_t p_Size) {
    int32_t rc = 0;
    struct DMBCall *qData = p_Q->m_Data;

    qData[*p_Q->m_Metadata.m_PushIdxPtr].m_Metadata.m_Size = p_Size;
    memcpy(&qData[*p_Q->m_Metadata.m_PushIdxPtr].m_CallInfo, p_RI, p_Size);

    return rc;
}

static int32_t s_SendReturnFnHGBFn(struct DSPQueue *p_Q, void *p_RI,
                                   uint32_t p_Size) {
    int32_t rc = 0;
    struct HGBCall *qData = p_Q->m_Data;

    qData[*p_Q->m_Metadata.m_PushIdxPtr].m_Metadata.m_Size = p_Size;
    memcpy(&qData[*p_Q->m_Metadata.m_PushIdxPtr].m_CallInfo, p_RI, p_Size);

    return rc;
}

static int32_t s_SendReturnFnGBFn(struct DSPQueue *p_Q, void *p_RI,
                                  uint32_t p_Size) {
    int32_t rc = 0;
    struct GBCall *qData = p_Q->m_Data;

    qData[*p_Q->m_Metadata.m_PushIdxPtr].m_Metadata.m_Size = p_Size;
    memcpy(&qData[*p_Q->m_Metadata.m_PushIdxPtr].m_CallInfo, p_RI, p_Size);

    return rc;
}

static int32_t s_SendReturnFnA(struct DSPQueue *p_Q, void *p_RI,
                               uint32_t p_Size, uint32_t p_QMaxSize,
                               int32_t (*p_Fn)(struct DSPQueue *, void *,
                                               uint32_t)) {
    int32_t rc = 0;

    QPUSH(p_Q, p_QMaxSize, do { rc = p_Fn(p_Q, p_RI, p_Size); } while (0));

    return rc;
}

static int32_t s_SendReturnFnSMB(struct DSPQueue *p_Q, struct SMBCall *p_RI,
                                 uint32_t p_Size) {
    return s_SendReturnFnA(p_Q, p_RI, p_Size, RETURNQ_MAX_SIZE,
                           s_SendReturnFnSMBFn);
}

static int32_t s_SendReturnFnEMB(struct DSPQueue *p_Q, struct EMBCall *p_RI,
                                 uint32_t p_Size) {
    return s_SendReturnFnA(p_Q, p_RI, p_Size, RETURNQ_MAX_SIZE,
                           s_SendReturnFnEMBFn);
}

static int32_t s_SendReturnFnQMB(struct DSPQueue *p_Q, struct QMBCall *p_RI,
                                 uint32_t p_Size) {
    return s_SendReturnFnA(p_Q, p_RI, p_Size, RETURNQ_MAX_SIZE,
                           s_SendReturnFnQMBFn);
}

static int32_t s_SendReturnFnHMB(struct DSPQueue *p_Q, struct HMBCall *p_RI,
                                 uint32_t p_Size) {
    return s_SendReturnFnA(p_Q, p_RI, p_Size, RETURNQ_MAX_SIZE,
                           s_SendReturnFnHMBFn);
}

static int32_t s_SendReturnFnMB(struct DSPQueue *p_Q, struct MBCall *p_RI,
                                uint32_t p_Size) {
    return s_SendReturnFnA(p_Q, p_RI, p_Size, RETURNQ_MAX_SIZE,
                           s_SendReturnFnMBFn);
}

static int32_t s_SendReturnFnDMB(struct DSPQueue *p_Q, struct DMBCall *p_RI,
                                 uint32_t p_Size) {
    return s_SendReturnFnA(p_Q, p_RI, p_Size, RETURNQ_MAX_SIZE,
                           s_SendReturnFnDMBFn);
}

static int32_t s_SendReturnFnHGB(struct DSPQueue *p_Q, struct HGBCall *p_RI,
                                 uint32_t p_Size) {
    return s_SendReturnFnA(p_Q, p_RI, p_Size, RETURNQ_MAX_SIZE,
                           s_SendReturnFnHGBFn);
}

static int32_t s_SendReturnFnGB(struct DSPQueue *p_Q, struct GBCall *p_RI,
                                uint32_t p_Size) {
    return s_SendReturnFnA(p_Q, p_RI, p_Size, RETURNQ_MAX_SIZE,
                           s_SendReturnFnGBFn);
}

static int32_t s_SendReturnFn(struct ServerCommunicationInfo *p_CmI) {
    switch (p_CmI->cInfo.m_Q->m_Type) {
    case SMBQ:
        return s_SendReturnFnSMB(p_CmI->cInfo.m_Q,
                                 (struct SMBCall *)p_CmI->cInfo.m_Data,
                                 p_CmI->dataSize);
    case EMBQ:
        return s_SendReturnFnEMB(p_CmI->cInfo.m_Q,
                                 (struct EMBCall *)p_CmI->cInfo.m_Data,
                                 p_CmI->dataSize);
    case QMBQ:
        return s_SendReturnFnQMB(p_CmI->cInfo.m_Q,
                                 (struct QMBCall *)p_CmI->cInfo.m_Data,
                                 p_CmI->dataSize);
    case HMBQ:
        return s_SendReturnFnHMB(p_CmI->cInfo.m_Q,
                                 (struct HMBCall *)p_CmI->cInfo.m_Data,
                                 p_CmI->dataSize);
    case MBQ:
        return s_SendReturnFnMB(p_CmI->cInfo.m_Q,
                                (struct MBCall *)p_CmI->cInfo.m_Data,
                                p_CmI->dataSize);
    case DMBQ:
        return s_SendReturnFnDMB(p_CmI->cInfo.m_Q,
                                 (struct DMBCall *)p_CmI->cInfo.m_Data,
                                 p_CmI->dataSize);
    case HGBQ:
        return s_SendReturnFnHGB(p_CmI->cInfo.m_Q,
                                 (struct HGBCall *)p_CmI->cInfo.m_Data,
                                 p_CmI->dataSize);
    case GBQ:
        return s_SendReturnFnGB(p_CmI->cInfo.m_Q,
                                (struct GBCall *)p_CmI->cInfo.m_Data,
                                p_CmI->dataSize);
    default:
        /**
         * TODO
         */
        return (-1);
    }
}

int32_t configureServiceReturnInformation(struct ServiceReturnInfo *p_RI,
                                          struct ServiceConnectInfo *p_ConnI,
                                          struct ConnectRequest *p_Rq) {
    int32_t rc = 0;
    aqua_file_handle_t returnQFd;
    aqua_file_handle_t requestResponseQFd;
    uint32_t connectionIdx;
    aqua_file_flags_t qFlag;
    aqua_mem_prot_t qProt;
    aqua_file_mode_t qMode;
    size_t qSize;
    void *returnQ;
    aqua_err_t err;

    connectionIdx = p_Rq->m_ConnectionIdx;

    requestResponseQFd = SharedMemoryObject.create(
        p_Rq->m_RequestResponseQName, AQUA_FILE_PERM_RDWR,
        AQUA_FILE_MODE_USER_READ | AQUA_FILE_MODE_USER_WRITE |
            AQUA_FILE_MODE_GROUP_READ | AQUA_FILE_MODE_GROUP_WRITE |
            AQUA_FILE_MODE_OTHER_READ | AQUA_FILE_MODE_OTHER_WRITE,
        p_Rq->m_ResponseQSize * sizeof(struct ConnectResponseInformation),
        false);

    switch (p_Rq->m_ReturnQType) {
    case SMBQ:
        qFlag = AQUA_FILE_PERM_RDWR;
        qMode = AQUA_FILE_MODE_USER_READ | AQUA_FILE_MODE_USER_WRITE |
                AQUA_FILE_MODE_GROUP_READ | AQUA_FILE_MODE_GROUP_WRITE |
                AQUA_FILE_MODE_OTHER_READ | AQUA_FILE_MODE_OTHER_WRITE;
        qSize = p_Rq->m_ReturnQSize * sizeof(struct SMBCall);
        qProt = AQUA_MEM_PROT_READ | AQUA_MEM_PROT_WRITE;

        break;
    case EMBQ:
        qFlag = AQUA_FILE_PERM_RDWR;
        qMode = AQUA_FILE_MODE_USER_READ | AQUA_FILE_MODE_USER_WRITE |
                AQUA_FILE_MODE_GROUP_READ | AQUA_FILE_MODE_GROUP_WRITE |
                AQUA_FILE_MODE_OTHER_READ | AQUA_FILE_MODE_OTHER_WRITE;
        qSize = p_Rq->m_ReturnQSize * sizeof(struct EMBCall);
        qProt = AQUA_MEM_PROT_READ | AQUA_MEM_PROT_WRITE;

        break;
    case QMBQ:
        qFlag = AQUA_FILE_PERM_RDWR;
        qMode = AQUA_FILE_MODE_USER_READ | AQUA_FILE_MODE_USER_WRITE |
                AQUA_FILE_MODE_GROUP_READ | AQUA_FILE_MODE_GROUP_WRITE |
                AQUA_FILE_MODE_OTHER_READ | AQUA_FILE_MODE_OTHER_WRITE;
        qSize = p_Rq->m_ReturnQSize * sizeof(struct QMBCall);
        qProt = AQUA_MEM_PROT_READ | AQUA_MEM_PROT_WRITE;

        break;
    case HMBQ:
        qFlag = AQUA_FILE_PERM_RDWR;
        qMode = AQUA_FILE_MODE_USER_READ | AQUA_FILE_MODE_USER_WRITE |
                AQUA_FILE_MODE_GROUP_READ | AQUA_FILE_MODE_GROUP_WRITE |
                AQUA_FILE_MODE_OTHER_READ | AQUA_FILE_MODE_OTHER_WRITE;
        qSize = p_Rq->m_ReturnQSize * sizeof(struct HMBCall);
        qProt = AQUA_MEM_PROT_READ | AQUA_MEM_PROT_WRITE;

        break;
    case MBQ:
        qFlag = AQUA_FILE_PERM_RDWR;
        qMode = AQUA_FILE_MODE_USER_READ | AQUA_FILE_MODE_USER_WRITE |
                AQUA_FILE_MODE_GROUP_READ | AQUA_FILE_MODE_GROUP_WRITE |
                AQUA_FILE_MODE_OTHER_READ | AQUA_FILE_MODE_OTHER_WRITE;
        qSize = p_Rq->m_ReturnQSize * sizeof(struct MBCall);
        qProt = AQUA_MEM_PROT_READ | AQUA_MEM_PROT_WRITE;

        break;
    case DMBQ:
        qFlag = AQUA_FILE_PERM_RDWR;
        qMode = AQUA_FILE_MODE_USER_READ | AQUA_FILE_MODE_USER_WRITE |
                AQUA_FILE_MODE_GROUP_READ | AQUA_FILE_MODE_GROUP_WRITE |
                AQUA_FILE_MODE_OTHER_READ | AQUA_FILE_MODE_OTHER_WRITE;
        qSize = p_Rq->m_ReturnQSize * sizeof(struct DMBCall);
        qProt = AQUA_MEM_PROT_READ | AQUA_MEM_PROT_WRITE;

        break;
    case HGBQ:
        qFlag = AQUA_FILE_PERM_RDWR;
        qMode = AQUA_FILE_MODE_USER_READ | AQUA_FILE_MODE_USER_WRITE |
                AQUA_FILE_MODE_GROUP_READ | AQUA_FILE_MODE_GROUP_WRITE |
                AQUA_FILE_MODE_OTHER_READ | AQUA_FILE_MODE_OTHER_WRITE;
        qSize = p_Rq->m_ReturnQSize * sizeof(struct HGBCall);
        qProt = AQUA_MEM_PROT_READ | AQUA_MEM_PROT_WRITE;

        break;
    case GBQ:
        qFlag = AQUA_FILE_PERM_RDWR;
        qMode = AQUA_FILE_MODE_USER_READ | AQUA_FILE_MODE_USER_WRITE |
                AQUA_FILE_MODE_GROUP_READ | AQUA_FILE_MODE_GROUP_WRITE |
                AQUA_FILE_MODE_OTHER_READ | AQUA_FILE_MODE_OTHER_WRITE;
        qSize = p_Rq->m_ReturnQSize * sizeof(struct GBCall);
        qProt = AQUA_MEM_PROT_READ | AQUA_MEM_PROT_WRITE;

        break;
    default:
        /**
         * TODO
         */
        DIE(true, "QType is not recognized");
    }

    returnQFd = SharedMemoryObject.create(p_Rq->m_ReturnQName, qFlag, qMode,
                                          qSize, false);

    createQ(&returnQ, qSize, qProt, returnQFd);

    p_ConnI->m_Connections[connectionIdx].m_ReturnQMapSize = qSize;

    err = SharedMemoryObject.close(returnQFd);
    DIE(err == AQUA_SHM_OBJ_CLOSE_FAILED, "Could not close returnQFd");

    struct ConnectResponseInformation *requestResponseQ = NULL;
    err = Allocator.memmap((aqua_void_ptr_t *)&requestResponseQ, NULL,
                           p_Rq->m_ResponseQSize *
                               sizeof(struct ConnectResponseInformation),
                           AQUA_MEM_PROT_WRITE | AQUA_MEM_PROT_READ,
                           AQUA_MEM_SHARED, requestResponseQFd, 0);
    DIE(err == AQUA_MEM_MAP_FAILED,
        "Could not map request response queue memory");

    err = SharedMemoryObject.close(requestResponseQFd);
    DIE(err == AQUA_SHM_OBJ_CLOSE_FAILED,
        "Could not close request response queue shared object file descriptor");

    p_ConnI->m_Connections[connectionIdx].m_ReturnQPushIdx = 0;
    p_ConnI->m_Connections[connectionIdx].m_ReturnQPopIdx = 0;
    p_ConnI->m_Connections[connectionIdx].m_ReturnQSize = 0;

    p_ConnI->m_Connections[connectionIdx].m_ReturnQ = returnQ;

    p_RI->m_Q.m_Data = returnQ;

    p_RI->m_Q.m_Metadata.m_FullCond =
        &p_ConnI->m_Connections[connectionIdx].m_ReturnQFullCond;
    p_RI->m_Q.m_Metadata.m_EmptyCond =
        &p_ConnI->m_Connections[connectionIdx].m_ReturnQEmptyCond;
    p_RI->m_Q.m_Metadata.m_Lock =
        &p_ConnI->m_Connections[connectionIdx].m_ReturnQMutex;

    p_RI->m_Q.m_Metadata.m_PushIdxPtr =
        &p_ConnI->m_Connections[connectionIdx].m_ReturnQPushIdx;
    p_RI->m_Q.m_Metadata.m_PopIdxPtr =
        &p_ConnI->m_Connections[connectionIdx].m_ReturnQPopIdx;
    p_RI->m_Q.m_Metadata.m_Size =
        &p_ConnI->m_Connections[connectionIdx].m_ReturnQSize;

    p_RI->m_Q.m_Type = p_Rq->m_ReturnQType;

    p_RI->m_ResponseQueue.m_MaxSize = p_Rq->m_ReturnQSize;

    p_RI->m_SendReturnFn = s_SendReturnFn;

    p_ConnI->m_Connections[connectionIdx].m_RequestResponseQMapSize =
        p_Rq->m_ResponseQSize * sizeof(struct ConnectResponseInformation);
    p_ConnI->m_Connections[connectionIdx].m_RequestResponseQ = requestResponseQ;

    p_RI->m_ResponseQueue.m_Data = requestResponseQ;

    p_RI->m_ResponseQueue.m_Metadata.m_FullCond =
        &p_ConnI->m_Connections[connectionIdx].m_RequestResponseQFullCond;
    p_RI->m_ResponseQueue.m_Metadata.m_EmptyCond =
        &p_ConnI->m_Connections[connectionIdx].m_RequestResponseQEmptyCond;
    p_RI->m_ResponseQueue.m_Metadata.m_Lock =
        &p_ConnI->m_Connections[connectionIdx].m_RequestResponseQMutex;

    p_RI->m_ResponseQueue.m_Metadata.m_PushIdxPtr =
        &p_ConnI->m_Connections[connectionIdx].m_RequestResponseQPushIdx;
    p_RI->m_ResponseQueue.m_Metadata.m_PopIdxPtr =
        &p_ConnI->m_Connections[connectionIdx].m_RequestResponseQPopIdx;
    p_RI->m_ResponseQueue.m_Metadata.m_Size =
        &p_ConnI->m_Connections[connectionIdx].m_RequestResponseQSize;
    p_RI->m_ResponseQueue.m_MaxSize = p_Rq->m_ResponseQSize;

    return rc;
}
