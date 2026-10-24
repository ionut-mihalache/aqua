// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef __DSP_CLIENT_H_
#define __DSP_CLIENT_H_

#include "aqua-sync.h"
#include "dsp.h"
#include "platform-types.h"

struct ClientReturnInfo {
    struct ConnectResponseQueue m_ResponseQueue;
    struct DSPQueue m_Q;
    struct ConnectResponseInformation m_ConnectResponseInformation;

    int32_t (*m_ReturnFn)(struct CommunicationInfo *);

    enum QType m_QType;
};

struct ClientCommunicationInfo {
    struct CommunicationInfo cInfo;
    struct ClientReturnInfo *info;
    uint32_t dataSize;
};

struct ClientCallInfo {
    struct DSPQueue m_Q;

    int32_t (*m_CallFn)(struct ClientCommunicationInfo *);
};

struct ClientConnectRequestInformation {
    char m_ReturnQName[RETURNQ_NAME_MAX_SIZE];
    char m_RequestResponseQName[RETURNQ_NAME_MAX_SIZE];

    uint32_t m_ReturnQSize;
    uint32_t m_ResponseQSize;
    enum QType m_QType;
};

struct ClientConnectInfo {
    struct ConnectQueue m_ConnectQ;
    struct DisconnectQueue m_DisconnectQ;
    struct ConnectionInformation *m_Connections;
    aqua_mutex_t *m_ConnectLock;
    int32_t (*m_SendConnectRequest)(struct ClientReturnInfo *,
                                    struct ClientConnectInfo *,
                                    struct ClientConnectRequestInformation *);
    int32_t (*m_SendDisconnectRequest)(struct ClientConnectInfo *,
                                       struct ConnectResponseInformation *);
};

AQUA_API_EXPORT int32_t aquaCreateClientConnectInfo();
AQUA_API_EXPORT int32_t aquaCreateClientCallInfo();
AQUA_API_EXPORT int32_t aquaCreateClientReturnInfo();
AQUA_API_EXPORT int32_t aquaCreateClientConnRequestInfo();

void aquaDestroyClientConnectInfo(int32_t);
void aquaDestroyClientCallInfo(int32_t);
void aquaDestroyClientReturnInfo(int32_t);
void aquaDestroyClientConnRequestInfo(int32_t);

AQUA_API_EXPORT void
aquaSetClientConnRequestInfo(int32_t connRequestHandle, const char *returnQName,
                             uint32_t returnQSize, const char *reqResponseQName,
                             uint32_t responseQSize, enum QType qType);

AQUA_API_EXPORT void aquaConnect(int32_t returnInfoHandle,
                                 int32_t connectInfoHandle,
                                 int32_t connReqInfoHandle);

AQUA_API_EXPORT void aquaDisconnect(int32_t connectInfoHandle,
                                    int32_t returnInfoHandle);

AQUA_API_EXPORT void aquaClientSend(int32_t callInfoHandle,
                                    int32_t returnInfoHandle, void *data,
                                    uint32_t size);

AQUA_API_EXPORT void aquaClientRecv(void *data, int32_t returnInfoHandle);

AQUA_API_EXPORT void aquaInitClientConnection(int32_t connectInfoHandle,
                                              int32_t callInfoHandle,
                                              const char *serviceStrId);

#endif // __DSP_CLIENT_H
