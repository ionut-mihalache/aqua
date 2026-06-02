#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dsp.h"
#include "dsp-service.h"

#define PAYLOAD_SIZE_64K (64 * 1024)
#define PAYLOAD_SIZE_256K (256 * 1024)
#define PAYLOAD_SIZE_1M (1024 * 1024)

#if defined(USE_64K)
#define PAYLOAD_SIZE PAYLOAD_SIZE_64K
#elif defined(USE_256K)
#define PAYLOAD_SIZE PAYLOAD_SIZE_256K
#else
#define PAYLOAD_SIZE PAYLOAD_SIZE_1M
#endif

static int QTYPE = QMBQ;

struct TransmissionData {
    uint64_t ns;
    uint8_t data[PAYLOAD_SIZE - sizeof(uint64_t)];
};

static enum QType sf_GetQType(char *p_Arg) {
    if (!strcmp(p_Arg, "SMB")) {
        return SMBQ;
    }

    if (!strcmp(p_Arg, "EMB")) {
        return EMBQ;
    }

    if (!strcmp(p_Arg, "QMB")) {
        return QMBQ;
    }

    if (!strcmp(p_Arg, "HMB")) {
        return HMBQ;
    }

    if (!strcmp(p_Arg, "MB")) {
        return MBQ;
    }

    if (!strcmp(p_Arg, "DMB")) {
        return DMBQ;
    }

    if (!strcmp(p_Arg, "HGB")) {
        return HGBQ;
    }

    if (!strcmp(p_Arg, "GB")) {
        return GBQ;
    }

    return -1;
}

int main(int argc, char *argv[]) {
    struct ServiceConnectInfo connectInfo;
    struct ServiceCallInfo callInfo;
    struct ServiceReturnInfo returnInfo;
    struct MBCall callData;
    struct MBCall returnData;

    struct TransmissionData *callPayload =
        (struct TransmissionData *)callData.m_CallInfo;
    struct TransmissionData *returnPayload =
        (struct TransmissionData *)returnData.m_CallInfo;

    if (argc < 1) {
        fprintf(
            stdout,
            "usage: ./service [SMB | EMB | QMB | HMB | MB | DMB | HGB | GB]\n");
        return 0;
    }

    QTYPE = sf_GetQType(argv[1]);
    if (QTYPE == -1) {
        fprintf(stderr, "Queue type not recognized!\n");
        return 0;
    }

    fprintf(stdout, "Starting service with (payload = %u, qtype= %s)\n", PAYLOAD_SIZE, argv[1]);
    return 0;

    dspInstall(&connectInfo, &callInfo, "c-benchmark-testing", "v0.0.2", QTYPE);

    connectInfo.m_ReceiveConnectRequest(&returnInfo, &connectInfo);

    while (true) {
        receiveCall(&callData, &callInfo);

        returnPayload->ns = callPayload->ns;

        sendReturn(&returnInfo, &returnData);
    }

    connectInfo.m_ReceiveDisconnectRequest(&connectInfo);

    return 0;
}
