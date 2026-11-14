// SPDX-License-Identifier: LGPL-2.1-or-later

#include "client-call.h"

#include "aqua-types.h"
#include "commons.h"
#include "dsp.h"
#include "macros.h"
#include "platform.h"
#include "system-values.h"
#include "utils.h"

#include <string.h>

static int32_t s_SMBPushFn(struct DSPQueue *p_Q, void *p_D, uint32_t p_Size) {
    int32_t rc = 0;
    struct SMBCall *qData = p_Q->m_Data;

    qData[*p_Q->m_Metadata.m_PushIdxPtr].m_Metadata.m_Size = p_Size;
    memcpy(&qData[*p_Q->m_Metadata.m_PushIdxPtr].m_CallInfo, p_D, p_Size);

    return rc;
}

static int32_t s_EMBPushFn(struct DSPQueue *p_Q, void *p_D, uint32_t p_Size) {
    int32_t rc = 0;
    struct EMBCall *qData = p_Q->m_Data;

    qData[*p_Q->m_Metadata.m_PushIdxPtr].m_Metadata.m_Size = p_Size;
    memcpy(&qData[*p_Q->m_Metadata.m_PushIdxPtr].m_CallInfo, p_D, p_Size);

    return rc;
}

static int32_t s_QMBPushFn(struct DSPQueue *p_Q, void *p_D, uint32_t p_Size) {
    int32_t rc = 0;
    struct QMBCall *qData = p_Q->m_Data;

    qData[*p_Q->m_Metadata.m_PushIdxPtr].m_Metadata.m_Size = p_Size;
    memcpy(&qData[*p_Q->m_Metadata.m_PushIdxPtr].m_CallInfo, p_D, p_Size);

    return rc;
}

static int32_t s_HMBPushFn(struct DSPQueue *p_Q, void *p_D, uint32_t p_Size) {
    int32_t rc = 0;
    struct HMBCall *qData = p_Q->m_Data;

    qData[*p_Q->m_Metadata.m_PushIdxPtr].m_Metadata.m_Size = p_Size;
    memcpy(&qData[*p_Q->m_Metadata.m_PushIdxPtr].m_CallInfo, p_D, p_Size);

    return rc;
}

static int32_t s_MBPushFn(struct DSPQueue *p_Q, void *p_D, uint32_t p_Size) {
    int32_t rc = 0;
    struct MBCall *qData = p_Q->m_Data;

    qData[*p_Q->m_Metadata.m_PushIdxPtr].m_Metadata.m_Size = p_Size;
    memcpy(&qData[*p_Q->m_Metadata.m_PushIdxPtr].m_CallInfo, p_D, p_Size);

    return rc;
}

static int32_t s_DMBPushFn(struct DSPQueue *p_Q, void *p_D, uint32_t p_Size) {
    int32_t rc = 0;
    struct DMBCall *qData = p_Q->m_Data;

    qData[*p_Q->m_Metadata.m_PushIdxPtr].m_Metadata.m_Size = p_Size;
    memcpy(&qData[*p_Q->m_Metadata.m_PushIdxPtr].m_CallInfo, p_D, p_Size);

    return rc;
}

static int32_t s_HGBPushFn(struct DSPQueue *p_Q, void *p_D, uint32_t p_Size) {
    int32_t rc = 0;
    struct HGBCall *qData = p_Q->m_Data;

    qData[*p_Q->m_Metadata.m_PushIdxPtr].m_Metadata.m_Size = p_Size;
    memcpy(&qData[*p_Q->m_Metadata.m_PushIdxPtr].m_CallInfo, p_D, p_Size);

    return rc;
}

static int32_t s_GBPushFn(struct DSPQueue *p_Q, void *p_D, uint32_t p_Size) {
    int32_t rc = 0;
    struct GBCall *qData = p_Q->m_Data;

    qData[*p_Q->m_Metadata.m_PushIdxPtr].m_Metadata.m_Size = p_Size;
    memcpy(&qData[*p_Q->m_Metadata.m_PushIdxPtr].m_CallInfo, p_D, p_Size);

    return rc;
}

static int32_t s_QPushA(struct DSPQueue *p_Queue, void *p_D, uint32_t p_Size,
                        uint32_t p_QMaxSize,
                        int32_t (*p_Fn)(struct DSPQueue *, void *, uint32_t)) {
    int32_t rc = 0;

    QPUSH(
        p_Queue, p_QMaxSize, do { rc = p_Fn(p_Queue, p_D, p_Size); } while (0));

    return rc;
}

static int32_t s_QPushSMB(struct DSPQueue *p_Queue, struct SMBCall *p_D,
                          uint32_t p_Size) {
    return s_QPushA(p_Queue, p_D, p_Size, SMB_Q_MAX_SIZE, s_SMBPushFn);
}

static int32_t s_QPushEMB(struct DSPQueue *p_Queue, struct EMBCall *p_D,
                          uint32_t p_Size) {
    return s_QPushA(p_Queue, p_D, p_Size, EMB_Q_MAX_SIZE, s_EMBPushFn);
}

static int32_t s_QPushQMB(struct DSPQueue *p_Queue, struct QMBCall *p_D,
                          uint32_t p_Size) {
    return s_QPushA(p_Queue, p_D, p_Size, QMB_Q_MAX_SIZE, s_QMBPushFn);
}

static int32_t s_QPushHMB(struct DSPQueue *p_Queue, struct HMBCall *p_D,
                          uint32_t p_Size) {
    return s_QPushA(p_Queue, p_D, p_Size, HMB_Q_MAX_SIZE, s_HMBPushFn);
}

static int32_t s_QPushMB(struct DSPQueue *p_Queue, struct MBCall *p_D,
                         uint32_t p_Size) {
    return s_QPushA(p_Queue, p_D, p_Size, MB_Q_MAX_SIZE, s_MBPushFn);
}

static int32_t s_QPushDMB(struct DSPQueue *p_Queue, struct DMBCall *p_D,
                          uint32_t p_Size) {
    return s_QPushA(p_Queue, p_D, p_Size, DMB_Q_MAX_SIZE, s_DMBPushFn);
}

static int32_t s_QPushHGB(struct DSPQueue *p_Queue, struct HGBCall *p_D,
                          uint32_t p_Size) {
    return s_QPushA(p_Queue, p_D, p_Size, HGB_Q_MAX_SIZE, s_HGBPushFn);
}

static int32_t s_QPushGB(struct DSPQueue *p_Queue, struct GBCall *p_D,
                         uint32_t p_Size) {
    return s_QPushA(p_Queue, p_D, p_Size, GB_Q_MAX_SIZE, s_GBPushFn);
}

static int32_t s_QPush(struct ClientCommunicationInfo *p_CmI) {
    switch (p_CmI->cInfo.m_Q->m_Type) {
    case SMBQ:
        return s_QPushSMB(p_CmI->cInfo.m_Q,
                          (struct SMBCall *)p_CmI->cInfo.m_Data,
                          p_CmI->dataSize);
    case EMBQ:
        return s_QPushEMB(p_CmI->cInfo.m_Q,
                          (struct EMBCall *)p_CmI->cInfo.m_Data,
                          p_CmI->dataSize);
    case QMBQ:
        return s_QPushQMB(p_CmI->cInfo.m_Q,
                          (struct QMBCall *)p_CmI->cInfo.m_Data,
                          p_CmI->dataSize);
    case HMBQ:
        return s_QPushHMB(p_CmI->cInfo.m_Q,
                          (struct HMBCall *)p_CmI->cInfo.m_Data,
                          p_CmI->dataSize);
    case MBQ:
        return s_QPushMB(p_CmI->cInfo.m_Q, (struct MBCall *)p_CmI->cInfo.m_Data,
                         p_CmI->dataSize);
    case DMBQ:
        return s_QPushDMB(p_CmI->cInfo.m_Q,
                          (struct DMBCall *)p_CmI->cInfo.m_Data,
                          p_CmI->dataSize);
    case HGBQ:
        return s_QPushHGB(p_CmI->cInfo.m_Q,
                          (struct HGBCall *)p_CmI->cInfo.m_Data,
                          p_CmI->dataSize);
    case GBQ:
        return s_QPushGB(p_CmI->cInfo.m_Q, (struct GBCall *)p_CmI->cInfo.m_Data,
                         p_CmI->dataSize);
    default:
        /**
         * TODO
         */
        return (-1);
    }
}

int32_t
configureClientCallInformation(struct ClientCallInfo *p_CI,
                               struct InstallInformation *p_InstallInfo) {
    int32_t rc = 0;
    aqua_err_t err;
    aqua_file_handle_t callQFd;
    aqua_file_flags_t qFlag;
    aqua_mem_prot_t qProt;
    aqua_file_mode_t qMode;
    size_t qSize;
    void *callQ;

    switch (p_InstallInfo->m_CallQType) {
    case SMBQ:
        qFlag = AQUA_FILE_PERM_RDWR;
        qMode = AQUA_FILE_MODE_USER_READ | AQUA_FILE_MODE_USER_WRITE |
                AQUA_FILE_MODE_GROUP_READ | AQUA_FILE_MODE_GROUP_WRITE |
                AQUA_FILE_MODE_OTHER_READ | AQUA_FILE_MODE_OTHER_WRITE;
        qSize = SMB_Q_MAX_SIZE * sizeof(struct SMBCall);
        qProt = AQUA_MEM_PROT_WRITE;

        break;
    case EMBQ:
        qFlag = AQUA_FILE_PERM_RDWR;
        qMode = AQUA_FILE_MODE_USER_READ | AQUA_FILE_MODE_USER_WRITE |
                AQUA_FILE_MODE_GROUP_READ | AQUA_FILE_MODE_GROUP_WRITE |
                AQUA_FILE_MODE_OTHER_READ | AQUA_FILE_MODE_OTHER_WRITE;
        qSize = EMB_Q_MAX_SIZE * sizeof(struct EMBCall);
        qProt = AQUA_MEM_PROT_WRITE;

        break;
    case QMBQ:
        qFlag = AQUA_FILE_PERM_RDWR;
        qMode = AQUA_FILE_MODE_USER_READ | AQUA_FILE_MODE_USER_WRITE |
                AQUA_FILE_MODE_GROUP_READ | AQUA_FILE_MODE_GROUP_WRITE |
                AQUA_FILE_MODE_OTHER_READ | AQUA_FILE_MODE_OTHER_WRITE;
        qSize = QMB_Q_MAX_SIZE * sizeof(struct QMBCall);
        qProt = AQUA_MEM_PROT_WRITE;

        break;
    case HMBQ:
        qFlag = AQUA_FILE_PERM_RDWR;
        qMode = AQUA_FILE_MODE_USER_READ | AQUA_FILE_MODE_USER_WRITE |
                AQUA_FILE_MODE_GROUP_READ | AQUA_FILE_MODE_GROUP_WRITE |
                AQUA_FILE_MODE_OTHER_READ | AQUA_FILE_MODE_OTHER_WRITE;
        qSize = HMB_Q_MAX_SIZE * sizeof(struct HMBCall);
        qProt = AQUA_MEM_PROT_WRITE;

        break;
    case MBQ:
        qFlag = AQUA_FILE_PERM_RDWR;
        qMode = AQUA_FILE_MODE_USER_READ | AQUA_FILE_MODE_USER_WRITE |
                AQUA_FILE_MODE_GROUP_READ | AQUA_FILE_MODE_GROUP_WRITE |
                AQUA_FILE_MODE_OTHER_READ | AQUA_FILE_MODE_OTHER_WRITE;
        qSize = MB_Q_MAX_SIZE * sizeof(struct MBCall);
        qProt = AQUA_MEM_PROT_WRITE;

        break;
    case DMBQ:
        qFlag = AQUA_FILE_PERM_RDWR;
        qMode = AQUA_FILE_MODE_USER_READ | AQUA_FILE_MODE_USER_WRITE |
                AQUA_FILE_MODE_GROUP_READ | AQUA_FILE_MODE_GROUP_WRITE |
                AQUA_FILE_MODE_OTHER_READ | AQUA_FILE_MODE_OTHER_WRITE;
        qSize = DMB_Q_MAX_SIZE * sizeof(struct DMBCall);
        qProt = AQUA_MEM_PROT_WRITE;

        break;
    case HGBQ:
        qFlag = AQUA_FILE_PERM_RDWR;
        qMode = AQUA_FILE_MODE_USER_READ | AQUA_FILE_MODE_USER_WRITE |
                AQUA_FILE_MODE_GROUP_READ | AQUA_FILE_MODE_GROUP_WRITE |
                AQUA_FILE_MODE_OTHER_READ | AQUA_FILE_MODE_OTHER_WRITE;
        qSize = HGB_Q_MAX_SIZE * sizeof(struct HGBCall);
        qProt = AQUA_MEM_PROT_WRITE;

        break;
    case GBQ:
        qFlag = AQUA_FILE_PERM_RDWR;
        qMode = AQUA_FILE_MODE_USER_READ | AQUA_FILE_MODE_USER_WRITE |
                AQUA_FILE_MODE_GROUP_READ | AQUA_FILE_MODE_GROUP_WRITE |
                AQUA_FILE_MODE_OTHER_READ | AQUA_FILE_MODE_OTHER_WRITE;
        qSize = GB_Q_MAX_SIZE * sizeof(struct GBCall);
        qProt = AQUA_MEM_PROT_WRITE;

        break;
    default:
        /**
         * TODO
         */
        DIE(true, "QType is not recognized");
    }

    callQFd = SharedMemoryObject.create(p_InstallInfo->m_CallQName, qFlag,
                                        qMode, qSize, false);

    createQ(&callQ, qSize, qProt, callQFd);

    err = SharedMemoryObject.close(callQFd);
    DIE(err == AQUA_SHM_OBJ_CLOSE_FAILED, "Could not close callQFd");

    p_CI->m_CallFn = s_QPush;

    p_CI->m_Q.m_Data = callQ;
    p_CI->m_Q.m_Metadata.m_PushIdxPtr = &p_InstallInfo->m_CallQPushIdx;
    p_CI->m_Q.m_Metadata.m_PopIdxPtr = &p_InstallInfo->m_CallQPopIdx;
    p_CI->m_Q.m_Metadata.m_Size = &p_InstallInfo->m_CallQSize;
    p_CI->m_Q.m_Metadata.m_Lock = &p_InstallInfo->m_CallQMutex;
    p_CI->m_Q.m_Metadata.m_FullCond = &p_InstallInfo->m_CallQFullCond;
    p_CI->m_Q.m_Metadata.m_EmptyCond = &p_InstallInfo->m_CallQEmptyCond;
    p_CI->m_Q.m_Type = p_InstallInfo->m_CallQType;

    return rc;
}
