#include <ecal_c/ecal.h>
#include <ecal_c/service/client.h>

#include <stdio.h>  // printf()
#include <string.h> // strlen
#include <stdlib.h> // free
#include <time.h>

#define PAYLOAD_SIZE_64K (64 * 1024)
#define PAYLOAD_SIZE_256K (256 * 1024)
#define PAYLOAD_SIZE_1M (1024 * 1024)
#define MSG_COUNT 20000

struct TransmissionData {
    uint64_t ns;
    uint8_t data[PAYLOAD_SIZE_1M - sizeof(uint64_t)];
};

static inline uint64_t now_ns() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);

    return ((uint64_t)ts.tv_sec * 1000000000ULL) + ts.tv_nsec;
}

static int cmp_u64(const void *a, const void *b) {
    uint64_t aa = *(const uint64_t *)a;
    uint64_t bb = *(const uint64_t *)b;

    return (aa > bb) - (aa < bb);
}

int main(int argc, char *argv[]) {
    eCAL_Initialize("benchmark_client", NULL, NULL);

    struct eCAL_SServiceMethodInformation method;

    memset(&method, 0, sizeof(method));
    method.method_name = "echo";

    eCAL_ServiceClient *client =
        eCAL_ServiceClient_New("benchmark", &method, 1, NULL);

    while (!eCAL_ServiceClient_IsConnected(client)) {
        eCAL_Process_SleepMS(100);
    }

    struct TransmissionData request;

    uint64_t *latency = malloc(MSG_COUNT * sizeof(uint64_t));

    size_t response_count;
    struct eCAL_SServiceResponse *responses;

    for (size_t i = 0; i < MSG_COUNT; ++i) {
        request.ns = now_ns();

        responses = NULL;
        response_count = 0;

        int rc = eCAL_ServiceClient_CallWithResponse(
            client, "echo", &request, sizeof(request), &responses,
            &response_count, NULL);

        if (rc != 0 || response_count == 0) {
            printf("call failed\n");
            continue;
        }

        struct TransmissionData *resp =
            (struct TransmissionData *)responses[0].response;

        latency[i] = now_ns() - resp->ns;

        eCAL_Free(responses);
    }

    qsort(latency, MSG_COUNT, sizeof(uint64_t), cmp_u64);

    printf("Samples = %d\n", MSG_COUNT);
    printf("P50   = %lu ns\n", latency[(size_t)(MSG_COUNT * 0.50)]);
    printf("P90   = %lu ns\n", latency[(size_t)(MSG_COUNT * 0.90)]);
    printf("P99   = %lu ns\n", latency[(size_t)(MSG_COUNT * 0.99)]);
    printf("P99.9 = %lu ns\n", latency[(size_t)(MSG_COUNT * 0.999)]);

    free(latency);

    eCAL_ServiceClient_Delete(client);

    eCAL_Finalize();

    return 0;
}
