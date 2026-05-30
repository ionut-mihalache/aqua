#include <ecal_c/ecal.h>

#include <stdio.h>  //printf()
#include <string.h> //memcpy(), memset()

#define PAYLOAD_SIZE_64K (64 * 1024)
#define PAYLOAD_SIZE_256K (256 * 1024)
#define PAYLOAD_SIZE_1M (1024 * 1024)
#define MSG_COUNT 100

struct TransmissionData {
    // int32_t x;
    // int32_t y;
    // double funky;
    uint64_t ns;
    uint8_t data[PAYLOAD_SIZE_1M - sizeof(uint64_t)];
};

static int
OnEchoCallback(const struct eCAL_SServiceMethodInformation *method_info,
               const void *request, size_t request_length, void **response,
               size_t *response_length, void *user_argument) {
    (void)method_info;
    (void)user_argument;

    *response = eCAL_Malloc(request_length);

    if (*response == NULL)
        return -1;

    memcpy(*response, request, request_length);

    *response_length = request_length;

    return 0;
}

int main(void) {
    eCAL_Initialize("benchmark_server", NULL, NULL);

    eCAL_ServiceServer *server = eCAL_ServiceServer_New("benchmark", NULL);

    struct eCAL_SServiceMethodInformation method;

    memset(&method, 0, sizeof(method));
    method.method_name = "echo";

    eCAL_ServiceServer_SetMethodCallback(server, &method, OnEchoCallback, NULL);

    while (eCAL_Ok()) {
        eCAL_Process_SleepMS(100);
    }

    eCAL_ServiceServer_Delete(server);

    eCAL_Finalize();

    return 0;
}
