#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include "dsp.h"
#include "dsp-client.h"

#define MSG_COUNT 30000

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

    uint64_t *latency = malloc(sizeof(uint64_t) * MSG_COUNT);

    dspConnect(&connectInfo, &callInfo, "c-benchmark-testing");

    sprintf(requestInfo.m_ReturnQName, "%s", "return-q-123");
    requestInfo.m_ReturnQSize = 1;

    sprintf(requestInfo.m_RequestResponseQName, "%s", "response-q-123");
    requestInfo.m_ResponseQSize = 1;
    requestInfo.m_QType = MBQ;

    sendConnectRequest(&returnInfo, &connectInfo, &requestInfo);

    callData.m_Metadata.m_ConnId = returnInfo.m_ConnectResponseInformation.m_Id;

    for (size_t s = 0; s < 1; ++s) {
        for (size_t i = 0; i < MSG_COUNT; ++i) {
            uint64_t now = now_ns();
            *(uint64_t *)callData.m_CallInfo = now;

            // uint64_t t0 = now_ns();
            callFn(&callInfo, &callData);
            // uint64_t t1 = now_ns();
            // printf("t1 - t0: %lu\n", t1 - t0);

            // printf("now: %lu\n", now);
            returnFn(&returnData, &returnInfo);

            now = now_ns();
            uint64_t return_ns = *(uint64_t *)returnData.m_CallInfo;
            latency[i] = now - return_ns;
        }
    }

    qsort(latency, MSG_COUNT, sizeof(uint64_t), cmp_u64);

    // printf("P0 = %lu ns\n", latency[(size_t)(MSG_COUNT * 0.0)]);

    printf("P50 = %lu ns\n", latency[(size_t)(MSG_COUNT * 0.50)]);

    printf("P99 = %lu ns\n", latency[(size_t)(MSG_COUNT * 0.99)]);

    printf("P99.9 = %lu ns\n", latency[(size_t)(MSG_COUNT * 0.999)]);

    sendDisconnectRequest(&connectInfo,
                          &returnInfo.m_ConnectResponseInformation);

    free(latency);
    latency = NULL;

    return 0;
}
