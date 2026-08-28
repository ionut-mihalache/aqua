// SPDX-License-Identifier: LGPL-2.1-or-later

#include "dsp-service.h"

#include <stdbool.h>
#include <string.h>

#include "aqua-sync.h"
#include "aqua-types.h"
#include "call.h"
#include "commons.h"
#include "dsp.h"
#include "install.h"
#include "log.h"
#include "platform-types.h"
#include "platform.h"
#include "system-values.h"
#include "utils.h"

static struct InstallSharedData *installShdata = NULL;

void initService() {
    aqua_err_t err;
    aqua_file_handle_t installShdFd;
    char spinLockName[AQUA_SPINLOCK_MEM_SIZE];

    installShdFd = SharedMemoryObject.create(
        INSTALL_MZONE "INIT", AQUA_FILE_PERM_RDWR,
        AQUA_FILE_MODE_USER_READ | AQUA_FILE_MODE_USER_WRITE |
            AQUA_FILE_MODE_GROUP_READ | AQUA_FILE_MODE_GROUP_WRITE |
            AQUA_FILE_MODE_OTHER_READ | AQUA_FILE_MODE_OTHER_WRITE,
        sizeof(struct InstallSharedData), true);
    DIE(installShdFd < 0, "Could not create install shared memory object");

    err = Allocator.memmap((aqua_void_ptr_t *)&installShdata, NULL,
                           sizeof(struct InstallSharedData),
                           AQUA_MEM_PROT_READ | AQUA_MEM_PROT_WRITE,
                           AQUA_MEM_SHARED, installShdFd, 0);
    DIE(err == AQUA_MEM_MAP_FAILED || installShdata == NULL,
        "Could not mmap install shared data object");

    Memory.triggerPageFaults(installShdata, sizeof(struct InstallSharedData),
                             AQUA_MEM_PROT_READ | AQUA_MEM_PROT_WRITE);

    err = SharedMemoryObject.close(installShdFd);
    DIE(err == AQUA_SHM_OBJ_CLOSE_FAILED, "Could not close installShdFd");

    memset(spinLockName, 0, AQUA_MUTEX_MEM_SIZE);
    // TODO: This needs to be checked in order to make sure that the NULL
    // terminator is properly set
    sprintf(spinLockName, "__aqua_install_data_lk");
    Sync.createSpinLock(&installShdata->m_InstallMZoneLk, spinLockName,
                        INSTALL_DATA_HANDLE);
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

void aquaInstall(struct ServiceConnectInfo *p_ConnectInfo,
                 struct ServiceCallInfo *p_CallInfo, const char *p_StrId,
                 const char *p_Version, int p_CallQType) {
    int rc;
    aqua_err_t err;
    aqua_file_handle_t installShmFd;
    uint8_t bytesnr = SERVICES_NUMBER >> 3;

    initService();

    aqua_size_t installArenaSize = sf_GetInstallArenaSize();

    installShmFd = SharedMemoryObject.create(
        INSTALL_MZONE, AQUA_FILE_PERM_RDWR,
        AQUA_FILE_MODE_USER_READ | AQUA_FILE_MODE_USER_WRITE |
            AQUA_FILE_MODE_GROUP_READ | AQUA_FILE_MODE_GROUP_WRITE |
            AQUA_FILE_MODE_OTHER_READ | AQUA_FILE_MODE_OTHER_WRITE,
        installArenaSize, true);
    DIE(installShmFd < 0,
        "Could not open install memory zone shared memory object");

    struct InstallInfo *installMemZone = NULL;

    err = Allocator.memmap((aqua_void_ptr_t *)&installMemZone, installShdata,
                           installArenaSize,
                           AQUA_MEM_PROT_READ | AQUA_MEM_PROT_WRITE,
                           AQUA_MEM_SHARED, installShmFd, 0);
    DIE(err == AQUA_MEM_MAP_FAILED, "Could not mmap install memory zone");

    int32_t freeIdx = -1;
    uint8_t *freeBytePtr = NULL;
    uint16_t freeByteIdx = 0;

    Sync.spinLock(&installShdata->m_InstallMZoneLk);
    for (uint8_t i = 0; i < bytesnr; ++i) {
        freeBytePtr = &installMemZone->m_InstallMap[i];

        for (uint8_t j = 7; j > 0; --j) {
            freeByteIdx++;

            if (((*freeBytePtr) & (1 << j)) == 0) {
                /**
                 * We set the bit index for the current byte
                 */
                freeIdx = j;
                goto check_free_index;
            }
        }
    }

check_free_index:
    if (freeIdx < 0) {
        ELOGF("Cannot install a new service!\n");
        goto spin_lock_unlock;
    }

    *freeBytePtr = (*freeBytePtr) | (1 << freeIdx);

spin_lock_unlock:
    Sync.spinUnlock(&installShdata->m_InstallMZoneLk);

    rc = Allocator.memunmap(installMemZone, sizeof(struct InstallInfo));
    DIE(rc != 0, "Could not unmap install memory zone");

    /**
     * Map only the information of the service
     */
    struct InstallInformation *installInfo = NULL;
    err = Allocator.memmap(
        (aqua_void_ptr_t *)&installInfo, NULL,
        alignUp(sizeof(struct InstallInformation), Memory.getMapGranularity()),
        AQUA_MEM_PROT_READ | AQUA_MEM_PROT_WRITE, AQUA_MEM_SHARED, installShmFd,
        sf_GetServiceOff(freeByteIdx));
    DIE(err == AQUA_MEM_MAP_FAILED, "Could not map service information");

    err = SharedMemoryObject.close(installShmFd);
    DIE(err == AQUA_SHM_OBJ_CLOSE_FAILED, "Could not close installShmFd");

    installInfo->m_ProcId = Process.getPid();
    installInfo->m_Available = true;
    size_t strIdLen = strlen(p_StrId);
    if (strIdLen > STRING_ID_MAX_LENGTH - 1) {
        ELOGF("Service string id %s is too long. Max length is %i.\n", p_StrId,
              STRING_ID_MAX_LENGTH - 1);
        return;
    }
    memset(installInfo->m_StrId, 0, STRING_ID_MAX_LENGTH);
    memcpy(installInfo->m_StrId, p_StrId, strIdLen);

    size_t versionLen = strlen(p_Version);
    if (versionLen > VERSION_MAX_LENGTH - 1) {
        ELOGF("Version string %s is too long. Max length is %u.\n", p_Version,
              VERSION_MAX_LENGTH - 1);
        return;
    }
    memset(installInfo->m_Version, 0, VERSION_MAX_LENGTH);
    memcpy(installInfo->m_Version, p_Version, versionLen);

    /**
     * Map memory for the call and return queues
     */
    if (strIdLen + versionLen + 10 > CALLQ_NAME_MAX_SIZE) {
        ELOGF("Could not create call queue.\n");
        return;
    }
    memset(installInfo->m_CallQName, 0, CALLQ_NAME_MAX_SIZE);
    sprintf(installInfo->m_CallQName, "%s-%s-call-q", p_StrId, p_Version);

    if (strIdLen + versionLen + 10 > RETURNQ_NAME_MAX_SIZE) {
        ELOGF("Could not create call queue.\n");
        return;
    }

    memset(installInfo->m_ConnectQName, 0, CONNECTQ_NAME_MAX_SIZE);
    sprintf(installInfo->m_ConnectQName, "%s-%s-connect-q", p_StrId, p_Version);
    memset(installInfo->m_DisconnectQName, 0, CONNECTQ_NAME_MAX_SIZE);
    sprintf(installInfo->m_DisconnectQName, "%s-%s-disconnect-q", p_StrId,
            p_Version);

    installInfo->m_CallQPushIdx = 0;
    installInfo->m_CallQPopIdx = 0;
    installInfo->m_CallQSize = 0;
    installInfo->m_CallQType = p_CallQType;

    initializeServiceConnections(installInfo);
    configureServiceConnectInformation(p_ConnectInfo, installInfo);
    configureServiceCallInformation(p_CallInfo, installInfo);

    LOGF("Successfully installed new service: (%s, %s).\n", p_StrId, p_Version);
}

void receiveCall(void *p_CallData, struct ServiceCallInfo *p_CallInfo) {
    struct CommunicationInfo cInfo;

    cInfo.m_Q = &(p_CallInfo->m_Q);
    cInfo.m_Data = p_CallData;

    p_CallInfo->m_ReceiveCallFn(&cInfo);
}

void sendReturn(struct ServiceReturnInfo *p_ReturnInfo, void *p_ReturnData) {
    struct CommunicationInfo cInfo;

    cInfo.m_Q = &(p_ReturnInfo->m_Q);
    cInfo.m_Data = p_ReturnData;

    p_ReturnInfo->m_SendReturnFn(&cInfo);
}

void dspReturn() {}
