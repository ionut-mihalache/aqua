// SPDX-License-Identifier: LGPL-2.1-or-later

#include <string.h>

#include "call.h"
#include "aqua-sync.h"
#include "aqua-types.h"
#include "commons.h"
#include "macros.h"
#include "platform-types.h"
#include "platform.h"
#include "system-values.h"
#include "utils.h"

static int32_t s_SMBPopFn(void *p_CI, struct DSPQueue *p_Q) {
    int32_t rc = 0;
    struct SMBCall *qData = p_Q->m_Data;

    memcpy(p_CI, &qData[*p_Q->m_Metadata.m_PopIdxPtr], sizeof(struct SMBCall));

    return rc;
}

static int32_t s_EMBPopFn(void *p_CI, struct DSPQueue *p_Q) {
    int32_t rc = 0;
    struct EMBCall *qData = p_Q->m_Data;

    memcpy(p_CI, &qData[*p_Q->m_Metadata.m_PopIdxPtr], sizeof(struct EMBCall));

    return rc;
}

static int32_t s_QMBPopFn(void *p_CI, struct DSPQueue *p_Q) {
    int32_t rc = 0;
    struct QMBCall *qData = p_Q->m_Data;

    memcpy(p_CI, &qData[*p_Q->m_Metadata.m_PopIdxPtr], sizeof(struct QMBCall));

    return rc;
}

static int32_t s_HMBPopFn(void *p_CI, struct DSPQueue *p_Q) {
    int32_t rc = 0;
    struct HMBCall *qData = p_Q->m_Data;

    memcpy(p_CI, &qData[*p_Q->m_Metadata.m_PopIdxPtr], sizeof(struct HMBCall));

    return rc;
}

static int32_t s_MBPopFn(void *p_CI, struct DSPQueue *p_Q) {
    int32_t rc = 0;
    struct MBCall *qData = p_Q->m_Data;

    memcpy(p_CI, &qData[*p_Q->m_Metadata.m_PopIdxPtr], sizeof(struct MBCall));

    return rc;
}

static int32_t s_DMBPopFn(void *p_CI, struct DSPQueue *p_Q) {
    int32_t rc = 0;
    struct DMBCall *qData = p_Q->m_Data;

    memcpy(p_CI, &qData[*p_Q->m_Metadata.m_PopIdxPtr], sizeof(struct DMBCall));

    return rc;
}

static int32_t s_HGBPopFn(void *p_CI, struct DSPQueue *p_Q) {
    int32_t rc = 0;
    struct HGBCall *qData = p_Q->m_Data;

    memcpy(p_CI, &qData[*p_Q->m_Metadata.m_PopIdxPtr], sizeof(struct HGBCall));

    return rc;
}

static int32_t s_GBPopFn(void *p_CI, struct DSPQueue *p_Q) {
    int32_t rc = 0;
    struct GBCall *qData = p_Q->m_Data;

    memcpy(p_CI, &qData[*p_Q->m_Metadata.m_PopIdxPtr], sizeof(struct GBCall));

    return rc;
}

static int32_t s_QPopSMB(struct SMBCall *callInfo, struct DSPQueue *queue);
static int32_t s_QPopEMB(struct EMBCall *callInfo, struct DSPQueue *queue);
static int32_t s_QPopQMB(struct QMBCall *callInfo, struct DSPQueue *queue);
static int32_t s_QPopHMB(struct HMBCall *callInfo, struct DSPQueue *queue);
static int32_t s_QPopMB(struct MBCall *callInfo, struct DSPQueue *queue);
static int32_t s_QPopDMB(struct DMBCall *callInfo, struct DSPQueue *queue);
static int32_t s_QPopHGB(struct HGBCall *callInfo, struct DSPQueue *queue);
static int32_t s_QPopGB(struct GBCall *callInfo, struct DSPQueue *queue);

/**
 * TODO: Return correct error code. This function 'never fails' at the moment.
 */
static int32_t s_QPopA(struct DSPQueue *queue, void *callInfo,
                       uint32_t qMaxSize,
                       int32_t (*fn)(void *, struct DSPQueue *));

static int32_t s_QPop(struct CommunicationInfo *p_CmI) {
    switch (p_CmI->m_Q->m_Type) {
    case SMBQ:
        return s_QPopSMB((struct SMBCall *)p_CmI->m_Data, p_CmI->m_Q);
    case EMBQ:
        return s_QPopEMB((struct EMBCall *)p_CmI->m_Data, p_CmI->m_Q);
    case QMBQ:
        return s_QPopQMB((struct QMBCall *)p_CmI->m_Data, p_CmI->m_Q);
    case HMBQ:
        return s_QPopHMB((struct HMBCall *)p_CmI->m_Data, p_CmI->m_Q);
    case MBQ:
        return s_QPopMB((struct MBCall *)p_CmI->m_Data, p_CmI->m_Q);
    case DMBQ:
        return s_QPopDMB((struct DMBCall *)p_CmI->m_Data, p_CmI->m_Q);
    case HGBQ:
        return s_QPopHGB((struct HGBCall *)p_CmI->m_Data, p_CmI->m_Q);
    case GBQ:
        return s_QPopGB((struct GBCall *)p_CmI->m_Data, p_CmI->m_Q);
    default:
        /**
         * TODO
         */
        return (-1);
    }
}

int32_t configureServiceCallInformation(struct ServiceCallInfo *p_CI,
                                        struct InstallInformation *p_InI) {
    aqua_err_t err;
    int32_t rc = 0;
    aqua_file_handle_t callQFd;
    aqua_file_flags_t qFlag;
    aqua_mem_prot_t qProt;
    aqua_file_mode_t qMode;
    size_t qSize;
    void *callQ;
    char callMutexName[AQUA_MUTEX_MEM_SIZE];
    char callCondName[AQUA_COND_MEM_SIZE];
    uint64_t nameHash = hashString64(p_InI->m_StrId);

    switch (p_InI->m_CallQType) {
    case SMBQ:
        qFlag = AQUA_FILE_PERM_RDWR;
        qMode = AQUA_FILE_MODE_USER_READ | AQUA_FILE_MODE_USER_WRITE |
                AQUA_FILE_MODE_GROUP_READ | AQUA_FILE_MODE_GROUP_WRITE |
                AQUA_FILE_MODE_OTHER_READ | AQUA_FILE_MODE_OTHER_WRITE;
        qSize = SMB_Q_MAX_SIZE * sizeof(struct SMBCall);
        qProt = AQUA_MEM_PROT_READ;

        break;
    case EMBQ:
        qFlag = AQUA_FILE_PERM_RDWR;
        qMode = AQUA_FILE_MODE_USER_READ | AQUA_FILE_MODE_USER_WRITE |
                AQUA_FILE_MODE_GROUP_READ | AQUA_FILE_MODE_GROUP_WRITE |
                AQUA_FILE_MODE_OTHER_READ | AQUA_FILE_MODE_OTHER_WRITE;
        qSize = EMB_Q_MAX_SIZE * sizeof(struct EMBCall);
        qProt = AQUA_MEM_PROT_READ;

        break;
    case QMBQ:
        qFlag = AQUA_FILE_PERM_RDWR;
        qMode = AQUA_FILE_MODE_USER_READ | AQUA_FILE_MODE_USER_WRITE |
                AQUA_FILE_MODE_GROUP_READ | AQUA_FILE_MODE_GROUP_WRITE |
                AQUA_FILE_MODE_OTHER_READ | AQUA_FILE_MODE_OTHER_WRITE;
        qSize = QMB_Q_MAX_SIZE * sizeof(struct QMBCall);
        qProt = AQUA_MEM_PROT_READ;

        break;
    case HMBQ:
        qFlag = AQUA_FILE_PERM_RDWR;
        qMode = AQUA_FILE_MODE_USER_READ | AQUA_FILE_MODE_USER_WRITE |
                AQUA_FILE_MODE_GROUP_READ | AQUA_FILE_MODE_GROUP_WRITE |
                AQUA_FILE_MODE_OTHER_READ | AQUA_FILE_MODE_OTHER_WRITE;
        qSize = HMB_Q_MAX_SIZE * sizeof(struct HMBCall);
        qProt = AQUA_MEM_PROT_READ;

        break;
    case MBQ:
        qFlag = AQUA_FILE_PERM_RDWR;
        qMode = AQUA_FILE_MODE_USER_READ | AQUA_FILE_MODE_USER_WRITE |
                AQUA_FILE_MODE_GROUP_READ | AQUA_FILE_MODE_GROUP_WRITE |
                AQUA_FILE_MODE_OTHER_READ | AQUA_FILE_MODE_OTHER_WRITE;
        qSize = MB_Q_MAX_SIZE * sizeof(struct MBCall);
        qProt = AQUA_MEM_PROT_READ;

        break;
    case DMBQ:
        qFlag = AQUA_FILE_PERM_RDWR;
        qMode = AQUA_FILE_MODE_USER_READ | AQUA_FILE_MODE_USER_WRITE |
                AQUA_FILE_MODE_GROUP_READ | AQUA_FILE_MODE_GROUP_WRITE |
                AQUA_FILE_MODE_OTHER_READ | AQUA_FILE_MODE_OTHER_WRITE;
        qSize = DMB_Q_MAX_SIZE * sizeof(struct DMBCall);
        qProt = AQUA_MEM_PROT_READ;

        break;
    case HGBQ:
        qFlag = AQUA_FILE_PERM_RDWR;
        qMode = AQUA_FILE_MODE_USER_READ | AQUA_FILE_MODE_USER_WRITE |
                AQUA_FILE_MODE_GROUP_READ | AQUA_FILE_MODE_GROUP_WRITE |
                AQUA_FILE_MODE_OTHER_READ | AQUA_FILE_MODE_OTHER_WRITE;
        qSize = HGB_Q_MAX_SIZE * sizeof(struct HGBCall);
        qProt = AQUA_MEM_PROT_READ;

        break;
    case GBQ:
        qFlag = AQUA_FILE_PERM_RDWR;
        qMode = AQUA_FILE_MODE_USER_READ | AQUA_FILE_MODE_USER_WRITE |
                AQUA_FILE_MODE_GROUP_READ | AQUA_FILE_MODE_GROUP_WRITE |
                AQUA_FILE_MODE_OTHER_READ | AQUA_FILE_MODE_OTHER_WRITE;
        qSize = GB_Q_MAX_SIZE * sizeof(struct GBCall);
        qProt = AQUA_MEM_PROT_READ;

        break;
    default:
        /**
         * TODO
         */
        DIE(true, "QType is not recognized");
    }

    callQFd = SharedMemoryObject.create(p_InI->m_CallQName, qFlag, qMode, qSize,
                                        true);

    createQ(&callQ, qSize, qProt, callQFd);

    Memory.triggerPageFaults(callQ, qSize, qProt);

    err = SharedMemoryObject.close(callQFd);
    DIE(err == AQUA_SHM_OBJ_CLOSE_FAILED, "Could not close callQFd");

    p_CI->m_ReceiveCallFn = s_QPop;
    p_CI->m_Q.m_Data = callQ;
    p_CI->m_Q.m_Metadata.m_PushIdxPtr = &p_InI->m_CallQPushIdx;
    p_CI->m_Q.m_Metadata.m_PopIdxPtr = &p_InI->m_CallQPopIdx;
    p_CI->m_Q.m_Metadata.m_Size = &p_InI->m_CallQSize;
    p_CI->m_Q.m_Type = p_InI->m_CallQType;

    // TODO: This needs to be checked in order to make sure that the NULL
    // terminator is properly set
    snprintf(callMutexName, sizeof(callMutexName), "__aqua_%016llx%u", nameHash,
             AQUA_CALL_MUTEX_TAG);
    Sync.createMutex(&p_InI->m_CallQMutex, callMutexName, SEND_HANDLE);

    // TODO: This needs to be checked in order to make sure that the NULL
    // terminator is properly set
    snprintf(callCondName, sizeof(callCondName), "__aqua_%016llx%u", nameHash,
             AQUA_CALL_COND_FULL_TAG);
    Sync.createCond(&p_InI->m_CallQFullCond, callCondName, SEND_FULL_HANDLE);

    // TODO: This needs to be checked in order to make sure that the NULL
    // terminator is properly set
    snprintf(callCondName, sizeof(callCondName), "__aqua_%016llx%u", nameHash,
             AQUA_CALL_COND_EMPTY_TAG);
    Sync.createCond(&p_InI->m_CallQEmptyCond, callCondName, SEND_EMPTY_HANDLE);

    p_CI->m_Q.m_Metadata.m_Lock = &p_InI->m_CallQMutex;
    p_CI->m_Q.m_Metadata.m_FullCond = &p_InI->m_CallQFullCond;
    p_CI->m_Q.m_Metadata.m_EmptyCond = &p_InI->m_CallQEmptyCond;

    return rc;
}

static int32_t s_QPopSMB(struct SMBCall *p_CI, struct DSPQueue *p_Q) {
    return s_QPopA(p_Q, p_CI, SMB_Q_MAX_SIZE, s_SMBPopFn);
}

static int32_t s_QPopEMB(struct EMBCall *p_CI, struct DSPQueue *p_Q) {
    return s_QPopA(p_Q, p_CI, EMB_Q_MAX_SIZE, s_EMBPopFn);
}

static int32_t s_QPopQMB(struct QMBCall *p_CI, struct DSPQueue *p_Q) {
    return s_QPopA(p_Q, p_CI, QMB_Q_MAX_SIZE, s_QMBPopFn);
}

static int32_t s_QPopHMB(struct HMBCall *p_CI, struct DSPQueue *p_Q) {
    return s_QPopA(p_Q, p_CI, HMB_Q_MAX_SIZE, s_HMBPopFn);
}

static int32_t s_QPopMB(struct MBCall *p_CI, struct DSPQueue *p_Q) {
    return s_QPopA(p_Q, p_CI, MB_Q_MAX_SIZE, s_MBPopFn);
}

static int32_t s_QPopDMB(struct DMBCall *p_CI, struct DSPQueue *p_Q) {
    return s_QPopA(p_Q, p_CI, DMB_Q_MAX_SIZE, s_DMBPopFn);
}

static int32_t s_QPopHGB(struct HGBCall *p_CI, struct DSPQueue *p_Q) {
    return s_QPopA(p_Q, p_CI, HGB_Q_MAX_SIZE, s_HGBPopFn);
}

static int32_t s_QPopGB(struct GBCall *p_CI, struct DSPQueue *p_Q) {
    return s_QPopA(p_Q, p_CI, GB_Q_MAX_SIZE, s_GBPopFn);
}

/**
 * TODO: Return correct error code. This function 'never fails' at the moment.
 */
static int32_t s_QPopA(struct DSPQueue *p_Q, void *p_CI, uint32_t p_QMaxSize,
                       int32_t (*p_Fn)(void *, struct DSPQueue *)) {
    int32_t rc = 0;

    QPOP(p_Q, p_QMaxSize, do { rc = p_Fn(p_CI, p_Q); } while (0));

    return rc;
}
