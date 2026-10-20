// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef __DSP_CLIENT_H_
#define __DSP_CLIENT_H_

#include "aqua-sync.h"
#include "dsp.h"
#include "platform-types.h"

struct ClientCallInfo {
    struct DSPQueue m_Q;

    int32_t (*m_CallFn)(struct CommunicationInfo *);
};

struct ClientReturnInfo {
    struct ConnectResponseQueue m_ResponseQueue;
    struct DSPQueue m_Q;
    struct ConnectResponseInformation m_ConnectResponseInformation;

    int32_t (*m_ReturnFn)(struct CommunicationInfo *);

    enum QType m_QType;
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

AQUA_API_EXPORT void aquaDestroyClientConnectInfo(int32_t);
AQUA_API_EXPORT void aquaDestroyClientCallInfo(int32_t);
AQUA_API_EXPORT void aquaDestroyClientReturnInfo(int32_t);
AQUA_API_EXPORT void aquaDestroyClientConnRequestInfo(int32_t);

AQUA_API_EXPORT void
aquaSetClientConnRequestInfo(int32_t connRequestHandle, const char *returnQName,
                             uint32_t returnQSize, const char *reqResponseQName,
                             uint32_t responseQSize, enum QType qType);

AQUA_API_EXPORT void
aquaConnect(struct ClientReturnInfo *p_ReturnInfo,
            struct ClientConnectInfo *p_ConnectInfo,
            struct ClientConnectRequestInformation *p_RequestInfo);

AQUA_API_EXPORT void aquaConnectV2(int32_t returnInfoHandle,
                                   int32_t connectInfoHandle,
                                   int32_t connReqInfoHandle);

AQUA_API_EXPORT void
aquaDisconnect(struct ClientConnectInfo *p_ConnectInfo,
               struct ConnectResponseInformation *p_requestResponseInfo);

AQUA_API_EXPORT void aquaDisconnectV2(int32_t connectInfoHandle,
                                      int32_t returnInfoHandle);

AQUA_API_EXPORT void callFn(struct ClientCallInfo *p_CallInfo,
                            void *p_CallData);

AQUA_API_EXPORT void send(int32_t callInfoHandle, void *data);

AQUA_API_EXPORT void sendV2(int32_t callInfoHandle, int32_t returnInfoHandle,
                            void *data, uint32_t size);

AQUA_API_EXPORT void returnFn(void *p_ReturnData,
                              struct ClientReturnInfo *p_ReturnInfo);

AQUA_API_EXPORT void recv(void *data, int32_t returnInfoHandle);

AQUA_API_EXPORT void *recvV2(int32_t returnInfoHandle);

AQUA_API_EXPORT int32_t setCallData(int p_Type, void *p_CallInfo,
                                    uint8_t *p_Data, uint32_t p_Size);

AQUA_API_EXPORT int32_t setCallDataV2(int p_Type, void *p_CallInfo,
                                      uint8_t *p_Data, uint32_t p_Size,
                                      int32_t returnInfoHandle);

AQUA_API_EXPORT void aquaInitConnection(struct ClientConnectInfo *p_ConnectInfo,
                                        struct ClientCallInfo *p_CallInfo,
                                        const char *p_ServiceStrId);

AQUA_API_EXPORT void aquaInitConnectionV2(int32_t connectInfoHandle,
                                          int32_t callInfoHandle,
                                          const char *serviceStrId);

AQUA_API_EXPORT void
retriveInitInformation(struct ClientConnectInfo *p_ConnectInfo,
                       struct ClientCallInfo *p_CallInfo,
                       const char *p_ServiceStrId);

AQUA_API_EXPORT struct ConnectResponseInformation *
getConnectResponse(struct ClientReturnInfo *p_ReturnInfo);

#endif // __DSP_CLIENT_H
