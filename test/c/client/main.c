#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "dsp.h"
#include "dsp-client.h"

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

#define MSG_COUNT 20000

static int QTYPE = QMBQ;

struct TransmissionData {
    uint64_t ns;
    uint8_t data[PAYLOAD_SIZE - sizeof(uint64_t)];
};

static inline uint64_t now_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);

    return ((uint64_t)ts.tv_sec * 1000000000ULL) + ts.tv_nsec;
}

static int cmp_u64(const void *a, const void *b) {
    uint64_t aa = *(uint64_t *)a;
    uint64_t bb = *(uint64_t *)b;

    return (aa > bb) - (aa < bb);
}

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
    struct ClientConnectInfo connectInfo;
    struct ClientCallInfo callInfo;
    struct ClientReturnInfo returnInfo;
    struct ClientConnectRequestInformation requestInfo;
    struct MBCall callData;
    struct MBCall returnData;

    struct TransmissionData *callPayload =
        (struct TransmissionData *)callData.m_CallInfo;
    struct TransmissionData *returnPayload =
        (struct TransmissionData *)returnData.m_CallInfo;

    uint64_t *latency = NULL;
    size_t msgCount = 1;

    if (argc < 2) {
        fprintf(stdout, "usage: ./service [SMB | EMB | QMB | HMB | MB | DMB | "
                        "HGB | GB] MSG_COUNT\n");
        return 0;
    }

    QTYPE = sf_GetQType(argv[1]);
    if (QTYPE == -1) {
        fprintf(stderr, "Queue type not recognized!\n");
        return 0;
    }

    msgCount = atol(argv[2]);

    fprintf(stdout,
            "Starting service with (payload = %u, qtype= %s, msg_count=%lu)\n",
            PAYLOAD_SIZE, argv[1], msgCount);
    return 0;

    latency = malloc(sizeof(uint64_t) * msgCount);

    dspConnect(&connectInfo, &callInfo, "c-benchmark-testing");

    sprintf(requestInfo.m_ReturnQName, "%s", "return-q-123");
    requestInfo.m_ReturnQSize = 1;

    sprintf(requestInfo.m_RequestResponseQName, "%s", "response-q-123");
    requestInfo.m_ResponseQSize = 1;
    requestInfo.m_QType = MBQ;

    sendConnectRequest(&returnInfo, &connectInfo, &requestInfo);

    callData.m_Metadata.m_ConnId = returnInfo.m_ConnectResponseInformation.m_Id;

    uint64_t samples = 0;

    for (size_t i = 0; i < msgCount; ++i) {
        callPayload->ns = now_ns();

        callFn(&callInfo, &callData);

        returnFn(&returnData, &returnInfo);

        latency[i] = now_ns() - returnPayload->ns;
        samples++;
    }

    sendDisconnectRequest(&connectInfo,
                          &returnInfo.m_ConnectResponseInformation);

    qsort(latency, samples, sizeof(uint64_t), cmp_u64);

    printf("Samples = %zu\n", samples);
    printf("P50     = %lu ns\n", latency[(size_t)(samples * 0.50)]);
    printf("P90     = %lu ns\n", latency[(size_t)(samples * 0.90)]);
    printf("P99     = %lu ns\n", latency[(size_t)(samples * 0.99)]);
    printf("P99.9   = %lu ns\n", latency[(size_t)(samples * 0.999)]);

    free(latency);
    latency = NULL;

    return 0;
}
