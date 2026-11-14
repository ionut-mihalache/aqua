#include <chrono>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
#include <ratio>
#include <stdio.h>
#include <string>
#include <thread>
#include <vector>

extern "C" {
#include "dsp-client.h"
#include "dsp.h"
}

int main(int argc, char *argv[]) {
    auto runClient = [&](int32_t clientIdx) {
        uint32_t connectInfoHandle = aquaCreateClientConnectInfo();
        uint32_t callInfoHandle = aquaCreateClientCallInfo();

        aquaInitClientConnection(connectInfoHandle, callInfoHandle,
                                 "xslt-transformation");

        uint32_t returnInfoHandle = aquaCreateClientReturnInfo();
        uint32_t connReqInfoHandle = aquaCreateClientConnRequestInfo();

        auto now = std::chrono::system_clock::now().time_since_epoch();
        auto ms =
            std::chrono::duration_cast<std::chrono::milliseconds>(now).count();

        char returnQName[RETURNQ_NAME_MAX_SIZE];
        snprintf(returnQName, RETURNQ_NAME_MAX_SIZE, "%s-%llu-%d", "return-q",
                 static_cast<long long>(ms), clientIdx);
        std::cout << returnQName << std::endl;

        char reqResponseQName[RETURNQ_NAME_MAX_SIZE];
        snprintf(reqResponseQName, RETURNQ_NAME_MAX_SIZE, "%s-%llu-%d",
                 "response-q", ms, clientIdx);
        std::cout << reqResponseQName << std::endl;

        aquaSetClientConnRequestInfo(connReqInfoHandle, returnQName, 1,
                                     reqResponseQName, 1, SMBQ);

        aquaConnect(returnInfoHandle, connectInfoHandle, connReqInfoHandle);

        auto iiaData =
            "<?xml version=\"1.0\" encoding =\"UTF-8\"?>\n\n<iias-get-response "
            "xmlns=\"https://github.com/erasmus-without-paper/"
            "ewp-specs-api-iias/"
            "blob/stable-v7/endpoints/get-response.xsd\" "
            "xmlns:c=\"https://github.com/erasmus-without-paper/"
            "ewp-specs-types-contact/tree/stable-v1\" "
            "xmlns:xsi=\"http://www.w3.org/2001/"
            "XMLSchema-instance\" "
            "xsi:schemaLocation=\"&#10;        "
            "https://github.com/erasmus-without-paper/"
            "ewp-specs-api-iias/blob/"
            "stable-v7/endpoints/get-response.xsd&#10;        "
            "https://raw.githubusercontent.com/"
            "erasmus-without-paper/"
            "ewp-specs-api-iias/stable-v7/endpoints/get-response.xsd&#10;"
            "\"><iia><partner><hei-id>upb.ro</"
            "hei-id><iia-id>9e01bc4f-f607-43c1-881a-8329c3e26062</"
            "iia-id></"
            "partner><partner><hei-id>brussels.uni-foundation.eu</"
            "hei-id><iia-id>9e01bc4f-f607-43c1-881a-8329c3e26063</"
            "iia-id><signing-contact><c:contact-name>Test "
            "contact</c:contact-name><c:email>test.contact@test.ro</"
            "c:email></"
            "signing-contact></partner><in-effect>false</"
            "in-effect><cooperation-conditions><student-studies-"
            "mobility-spec><"
            "sending-hei-id>brussels.uni-foundation.eu</"
            "sending-hei-id><sending-ounit-id>5</"
            "sending-ounit-id><receiving-hei-id>upb.ro</"
            "receiving-hei-id><receiving-ounit-id>ef711143-b1ab-"
            "4412-8493-"
            "3476d5ff962f</"
            "receiving-ounit-id><receiving-first-academic-year-id>"
            "2026/2027</"
            "receiving-first-academic-year-id><receiving-last-"
            "academic-year-id>"
            "2029/2030</"
            "receiving-last-academic-year-id><mobilities-per-year>2<"
            "/"
            "mobilities-per-year><recommended-language-skill><"
            "language>is</"
            "language><cefr-level>B2</"
            "cefr-level><subject-area><isced-f-code>0007</"
            "isced-f-code><isced-clarification>Engineering, manufacturing and "
            " construction</ isced - clarification></ subject - area> </ "
            "recommended-language-skill><subject-area><isced-f-"
            "code>0819</"
            "isced-f-code><isced-clarification>Agriculture, not elsewhere"
            "classified</isced-clarification></"
            "subject-area><blended>false</"
            "blended></student-studies-mobility-spec></"
            "cooperation-conditions></"
            "iia></iias-get-response>";

        auto start = std::chrono::high_resolution_clock::now();
        aquaClientSend(callInfoHandle, (void *)iiaData, strlen(iiaData));
        auto end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double, std::milli> duration = end - start;

        fprintf(stdout, "Call duration: %lf\n", duration.count());

        char returnData[SMB];
        aquaClientRecv(returnData, returnInfoHandle);

        uint32_t dataLen = *(uint32_t *)returnData;

        std::string data(returnData + 4, dataLen);

        aquaDisconnect(connectInfoHandle, returnInfoHandle);
        fprintf(stdout, "%s\n", data.c_str());

        return 0;
    };

    int32_t clientsNumber = atoi(argv[1]);

    std::vector<std::shared_ptr<std::thread>> clientThreads;
    clientThreads.reserve(clientsNumber);

    for (int32_t i = 0; i < clientsNumber; ++i) {
        // runClient(i);
        std::shared_ptr<std::thread> clientThread(
            new std::thread(runClient, i));
        clientThreads.push_back(clientThread);
    }

    for (auto &clientThread : clientThreads) {
        clientThread->join();
    }

    return 0;
}
