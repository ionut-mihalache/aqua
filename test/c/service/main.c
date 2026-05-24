#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "dsp.h"
#include "dsp-service.h"

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
    struct ServiceConnectInfo connectInfo;
    struct ServiceCallInfo callInfo;
    struct ServiceReturnInfo returnInfo;
    struct MBCall callData;
    struct MBCall returnData;

    dspInstall(&connectInfo, &callInfo, "c-benchmark-testing", "v0.0.2", MBQ);

    uint64_t *latency = malloc(sizeof(uint64_t) * MSG_COUNT);

    connectInfo.m_ReceiveConnectRequest(&returnInfo, &connectInfo);


    for (size_t s = 0; s < 1; ++s) {

        for (uint64_t i = 0; i < MSG_COUNT; ++i) {
            // uint64_t t0 = now_ns();
            receiveCall(&callData, &callInfo);
            // uint64_t t1 = now_ns();
            // printf("t1 - t0: %lu\n", t1 - t0);
            uint64_t now = now_ns();
            uint64_t call_ns = *(uint64_t *)callData.m_CallInfo;
            latency[i] = now - call_ns;
            // printf("now - old_now: %lu - %lu\n", now, call_ns);

            *(uint64_t *)returnData.m_CallInfo = now_ns();
            sendReturn(&returnInfo, &returnData);
        }
    }
    connectInfo.m_ReceiveDisconnectRequest(&connectInfo);

    qsort(latency, MSG_COUNT, sizeof(uint64_t), cmp_u64);

    // printf("P0 = %lu ns\n", latency[(size_t)(MSG_COUNT * 0.0)]);

    printf("P50 = %lu ns\n", latency[(size_t)(MSG_COUNT * 0.50)]);

    printf("P99 = %lu ns\n", latency[(size_t)(MSG_COUNT * 0.99)]);

    printf("P99.9 = %lu ns\n", latency[(size_t)(MSG_COUNT * 0.999)]);

    free(latency);
    latency = NULL;

    return 0;
}
