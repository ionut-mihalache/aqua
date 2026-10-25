// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef __DSP_SERVICE_H
#define __DSP_SERVICE_H

#include "aqua-sync.h"
#include "dsp.h"
#include "platform-types.h"

#define INSTALL_SHD "/install-shared-data"

struct InstallCommons {
    uint8_t m_IsInitialized;
};

struct InstallSharedData {
    aqua_mutex_t m_InstallMZoneMx;
    aqua_spinlock_t m_InstallMZoneLk;
};

struct ServerCommunicationInfo {
    struct CommunicationInfo cInfo;
    struct ServerReturnInfo *info;
    uint32_t dataSize;
};

struct ServiceCallInfo {
    struct DSPQueue m_Q;
    int32_t (*m_ReceiveCallFn)(struct CommunicationInfo *);
};

struct ServiceReturnInfo {
    struct ConnectResponseQueue m_ResponseQueue;
    struct ConnectResponseInformation m_ConnectResponseInformation;
    struct DSPQueue m_Q;

    int32_t (*m_SendReturnFn)(struct ServerCommunicationInfo *);
};

struct ServiceConnectInfo {
    struct ConnectQueue m_ConnectQ;
    struct DisconnectQueue m_DisconnectQ;
    struct ConnectionInformation *m_Connections;
    // pthread_spinlock_t *m_ConnectLock;
    aqua_mutex_t *m_ConnectLock;
};

void initService();

AQUA_API_EXPORT int32_t aquaCreateServerConnectInfo();
AQUA_API_EXPORT int32_t aquaCreateServerCallInfo();

AQUA_API_EXPORT void aquaDestroyConnectInfo(int32_t);
AQUA_API_EXPORT void aquaDestroyCallInfo(int32_t);

AQUA_API_EXPORT void aquaInstall(int32_t connectInfoHandle,
                                 int32_t callInfoHandle, const char *strId,
                                 const char *version, int callQType);

AQUA_API_EXPORT void receiveCall(void *, struct ServiceCallInfo *);
AQUA_API_EXPORT void aquaServerSend(int32_t, void *, uint32_t);

AQUA_API_EXPORT int32_t receiveConnectRequest(int32_t);
AQUA_API_EXPORT void aquaServerRecv(void *, int32_t);

AQUA_API_EXPORT void receiveDisconnectRequest(int32_t p_ConnectInfoHandle);

#endif // __DSP_SERVICE_H
