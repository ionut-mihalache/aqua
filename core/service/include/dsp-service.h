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

struct ServiceCallInfo {
    struct DSPQueue m_Q;
    int32_t (*m_ReceiveCallFn)(struct CommunicationInfo *);
};

struct ServiceReturnInfo {
    struct ConnectResponseQueue m_ResponseQueue;
    struct ConnectResponseInformation m_ConnectResponseInformation;
    struct DSPQueue m_Q;

    int32_t (*m_SendReturnFn)(struct CommunicationInfo *);
};

struct ServiceConnectInfo {
    struct ConnectQueue m_ConnectQ;
    struct DisconnectQueue m_DisconnectQ;
    struct ConnectionInformation *m_Connections;
    // pthread_spinlock_t *m_ConnectLock;
    aqua_mutex_t *m_ConnectLock;
    // int32_t (*m_ReceiveConnectRequest)(struct ServiceReturnInfo *,
    //                                    struct ServiceConnectInfo *);
    // int32_t (*m_ReceiveConnectRequestV2)(int32_t);
    // int32_t (*m_ReceiveDisconnectRequest)(struct ServiceConnectInfo *);
};

void initService();

AQUA_API_EXPORT int32_t aquaCreateServerConnectInfo();
AQUA_API_EXPORT int32_t aquaCreateServerCallInfo();

AQUA_API_EXPORT void aquaDestroyConnectInfo(int32_t);
AQUA_API_EXPORT void aquaDestroyCallInfo(int32_t);

AQUA_API_EXPORT void aquaInstall(struct ServiceConnectInfo *connectInfo,
                                 struct ServiceCallInfo *callInfo,
                                 const char *strId, const char *version,
                                 int callQType);

AQUA_API_EXPORT void aquaInstallV2(int32_t connectInfoHandle,
                                   int32_t callInfoHandle, const char *strId,
                                   const char *version, int callQType);

AQUA_API_EXPORT void receiveCall(void *, struct ServiceCallInfo *);
AQUA_API_EXPORT void sendReturn(struct ServiceReturnInfo *, void *);
AQUA_API_EXPORT void sendReturnV2(int32_t, void *);
AQUA_API_EXPORT void aquaServerSend(int32_t, int32_t, void *, uint32_t);

AQUA_API_EXPORT int32_t receiveConnectRequestV2(int32_t);
AQUA_API_EXPORT void aquaServerRecv(void *, int32_t);

AQUA_API_EXPORT enum QType getReturnInfoQType(int32_t);

AQUA_API_EXPORT void receiveDisconnectRequest(int32_t p_ConnectInfoHandle);

#endif // __DSP_SERVICE_H
