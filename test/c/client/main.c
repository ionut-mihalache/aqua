#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include "dsp.h"
#include "dsp-client.h"

#define PAYLOAD_SIZE_64K (64 * 1024)
#define PAYLOAD_SIZE_256K (256 * 1024)
#define PAYLOAD_SIZE_1M (1024 * 1024)

#define MSG_COUNT 20000

struct TransmissionData {
    uint64_t ns;
    uint8_t data[PAYLOAD_SIZE_1M - sizeof(uint64_t)];
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

int main() {
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

    uint64_t *latency = malloc(sizeof(uint64_t) * MSG_COUNT);

    dspConnect(&connectInfo, &callInfo, "c-benchmark-testing");

    sprintf(requestInfo.m_ReturnQName, "%s", "return-q-123");
    requestInfo.m_ReturnQSize = 1;

    sprintf(requestInfo.m_RequestResponseQName, "%s", "response-q-123");
    requestInfo.m_ResponseQSize = 1;
    requestInfo.m_QType = MBQ;

    sendConnectRequest(&returnInfo, &connectInfo, &requestInfo);

    callData.m_Metadata.m_ConnId = returnInfo.m_ConnectResponseInformation.m_Id;

    uint64_t samples = 0;

    for (size_t i = 0; i < MSG_COUNT; ++i) {
        // uint64_t now = now_ns();
        callPayload->ns = now_ns();

        // uint64_t t0 = now_ns();
        callFn(&callInfo, &callData);
        // uint64_t t1 = now_ns();
        // printf("t1 - t0: %lu\n", t1 - t0);

        // printf("now: %lu\n", now);
        returnFn(&returnData, &returnInfo);

        // now = now_ns();
        // uint64_t return_ns = *(uint64_t *)returnData.m_CallInfo;

        latency[i] = now_ns() - returnPayload->ns;
        samples++;
    }

    sendDisconnectRequest(&connectInfo,
                          &returnInfo.m_ConnectResponseInformation);

    qsort(latency, samples, sizeof(uint64_t), cmp_u64);

    // printf("P0 = %lu ns\n", latency[(size_t)(samples * 0.0)]);

    printf("Samples = %zu\n", samples);
    printf("P50     = %lu ns\n", latency[(size_t)(samples * 0.50)]);
    printf("P90     = %lu ns\n", latency[(size_t)(samples * 0.90)]);
    printf("P99     = %lu ns\n", latency[(size_t)(samples * 0.99)]);
    printf("P99.9   = %lu ns\n", latency[(size_t)(samples * 0.999)]);

    free(latency);
    latency = NULL;

    return 0;
}
