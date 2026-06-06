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

#if defined(USE_64K)
#define PAYLOAD_SIZE PAYLOAD_SIZE_64K
#elif defined(USE_256K)
#define PAYLOAD_SIZE PAYLOAD_SIZE_256K
#else
#define PAYLOAD_SIZE PAYLOAD_SIZE_1M
#endif

struct TransmissionData {
    uint64_t ns;
    uint8_t data[PAYLOAD_SIZE - sizeof(uint64_t)];
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

    size_t msgCount = 0;

    if (argc < 2) {
        fprintf(stdout, "usage: ./client MSG_COUNT OUTPUT_FILE\n");
        return 0;
    }

    msgCount = atol(argv[1]);
    uint64_t *latency = malloc(msgCount * sizeof(uint64_t));

    FILE *outfile = fopen(argv[2], "a");

    size_t response_count;
    struct eCAL_SServiceResponse *responses;
    size_t samples = 0;

    for (size_t i = 0; i < msgCount; ++i) {
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
        samples++;
    }

    qsort(latency, samples, sizeof(uint64_t), cmp_u64);

    // fprintf(outfile, "msg_count,P50,P90,P99,P99.9\n");
    fprintf(
        outfile, "%lu,%lu,%lu,%lu,%lu\n", samples,
        latency[(size_t)(samples * 0.50)], latency[(size_t)(samples * 0.90)],
        latency[(size_t)(samples * 0.99)], latency[(size_t)(samples * 0.999)]);

    free(latency);
    fclose(outfile);

    eCAL_ServiceClient_Delete(client);

    eCAL_Finalize();

    return 0;
}
