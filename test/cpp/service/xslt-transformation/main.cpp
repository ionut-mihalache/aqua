#include <iostream>
#include <memory>
#include <mutex>
#include <stdio.h>
#include <thread>
#include <unordered_map>

extern "C" {
#include "dsp-service.h"
#include "dsp.h"
}

template <typename K, typename V> class ConcurrentHashMap {
  private:
    std::unordered_map<K, V> map;
    mutable std::mutex mtx;

  public:
    void insert(const K &key, const V &value) {
        std::lock_guard<std::mutex> lock(mtx);
        map[key] = value;
    }

    bool get(const K &key, V &value) const {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = map.find(key);
        if (it == map.end())
            return false;
        value = it->second;
        return true;
    }

    void erase(const K &key) {
        std::lock_guard<std::mutex> lock(mtx);
        map.erase(key);
    }
};

static std::shared_ptr<ServiceConnectInfo> connectInfo;
static std::shared_ptr<ServiceCallInfo> callInfo;
static ConcurrentHashMap<uint32_t, uint32_t> connections;

int main() {
    int32_t connectInfoHandle = aquaCreateServerConnectInfo();
    int32_t callInfoHandle = aquaCreateServerCallInfo();
    // connectInfo = std::make_shared<ServiceConnectInfo>();
    // callInfo = std::make_shared<ServiceCallInfo>();

    // aquaInstall(connectInfo.get(), callInfo.get(), "xslt-transformation",
    //            "v0.0.1", SMBQ);
    aquaInstall(connectInfoHandle, callInfoHandle, "xslt-transformation",
                "v0.0.1", SMBQ);

    auto handleConnect = [&]() {
        while (true) {
            // std::shared_ptr<ServiceReturnInfo> returnInfo =
            //     std::make_shared<ServiceReturnInfo>();

            // connectInfo->m_ReceiveConnectRequest(returnInfo.get(),
            //                                      connectInfo.get());
            uint32_t returnHandle = receiveConnectRequest(connectInfoHandle);
            std::cout << "Connection request received.\n";

            // uint32_t connId = returnInfo->m_ConnectResponseInformation.m_Id;
            uint32_t connId = getConnectionId(returnHandle);

            connections.insert(connId, returnHandle);
        }
    };

    auto handleDisconnect = [&]() {
        while (true) {
            receiveDisconnectRequest(connectInfoHandle);
        }
    };

    // auto handleCall = [&](std::unique_ptr<SMBCall> callData) {
    //     // std::cout << callData->m_CallInfo << std::endl;

    //     std::unique_ptr<SMBCall> returnData(new SMBCall);

    //     int size = snprintf(
    //         (char *)returnData->m_CallInfo, sizeof(returnData->m_CallInfo),
    //         "It looks like it works! - %u", callData->m_Metadata.m_ConnId);
    //     returnData->m_Metadata.m_Size = size;
    //     returnData->m_Metadata.m_ConnId = callData->m_Metadata.m_ConnId;

    //     std::shared_ptr<ServiceReturnInfo> returnInfo;
    //     if (!connections.get(callData->m_Metadata.m_ConnId, returnInfo)) {
    //         std::cout << "Could not get connection "
    //                   << callData->m_Metadata.m_ConnId << std::endl;
    //     }
    //     // sendReturn(returnInfo.get(), returnData.get());
    //     aquaServerSend(int32_t, void *, uint32_t);
    //     std::cout << "Return sent to client" << std::endl;
    // };

    auto handleCall = [&](std::unique_ptr<byte[]> callData) {
        uint32_t callDataSize = *(uint32_t *)callData.get();
        uint32_t connId =
            *(uint32_t *)((char *)callData.get() + sizeof(uint32_t));
        char returnData[256];

        int size =
            snprintf(returnData, 256, "It looks like it works! - %u", connId);

        uint32_t returnInfoHandle;
        if (!connections.get(connId, returnInfoHandle)) {
            std::cout << "Could not get connection " << connId << std::endl;
        }
        aquaServerSend(returnInfoHandle, returnData, size);
        std::cout << "Return sent to client" << std::endl;
    };

    std::thread connectThread(handleConnect);
    std::thread disconnectThread(handleDisconnect);

    while (true) {
        // std::unique_ptr<SMBCall> callData(new SMBCall);
        std::unique_ptr<byte[]> callData(new byte[65000]);

        std::cout << "Waiting for call request..." << std::endl;
        // receiveCall(callData.get(), callInfo.get());
        aquaServerRecv(callData.get(), callInfoHandle);
        std::cout << "Call request received..." << std::endl;

        // handleCall(std::move(callData));

        std::thread callThread(handleCall, std::move(callData));
        callThread.detach();
    }

    connectThread.join();
    disconnectThread.join();
}
