// Copyright (c) 2025 Contributors to the Eclipse Foundation
//
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
//
// This program and the accompanying materials are made available under the
// terms of the Apache Software License 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0, or the MIT license
// which is available at https://opensource.org/licenses/MIT.
//
// SPDX-License-Identifier: Apache-2.0 OR MIT

#include "iox2/iceoryx2.h"
#include "transmission_data.h"

#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__) || defined(_WIN64)
#define alignof __alignof
#else
#include <stdalign.h>
#endif
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) { // NOLINT
    size_t msgCount = 0;

    if (argc < 2) {
        fprintf(stdout, "usage: ./client MSG_COUNT OUTPUT_FILE\n");
        return 0;
    }

    msgCount = atol(argv[1]);
    uint64_t* latency = malloc(msgCount * sizeof(uint64_t));

    FILE* outfile = fopen(argv[2], "a");

    // Setup logging
    iox2_set_log_level_from_env_or(iox2_log_level_e_INFO);
    int ret_val = 0;

    // Create new node
    iox2_node_builder_h node_builder_handle = iox2_node_builder_new(NULL);
    iox2_node_h node_handle = NULL;
    ret_val = iox2_node_builder_create(node_builder_handle, NULL, iox2_service_type_e_IPC, &node_handle);
    if (ret_val != IOX2_OK) {
        printf("Could not create node! Error: %d\n", ret_val);
        goto end;
    }

    // Create service name
    const char* service_name_value = "My/Funk/ServiceName";
    iox2_service_name_h service_name = NULL;
    ret_val = iox2_service_name_new(NULL, service_name_value, strlen(service_name_value), &service_name);
    if (ret_val != IOX2_OK) {
        printf("Unable to create service name! Error: %d\n", ret_val);
        goto drop_node;
    }

    // Create service builder
    iox2_service_name_ptr service_name_ptr = iox2_cast_service_name_ptr(service_name);
    iox2_service_builder_h service_builder = iox2_node_service_builder(&node_handle, NULL, service_name_ptr);
    iox2_service_builder_request_response_h service_builder_request_response =
        iox2_service_builder_request_response(service_builder);

    // Set request and response type details
    const char* request_type_name = "TransmissionData";
    const char* response_type_name = "TransmissionData";

    ret_val = iox2_service_builder_request_response_set_request_payload_type_details(&service_builder_request_response,
                                                                                     iox2_type_variant_e_FIXED_SIZE,
                                                                                     request_type_name,
                                                                                     strlen(request_type_name),
                                                                                     sizeof(struct TransmissionData),
                                                                                     alignof(struct TransmissionData));
    if (ret_val != IOX2_OK) {
        printf("Unable to set request type details! Error: %d\n", ret_val);
        goto drop_service_name;
    }

    ret_val = iox2_service_builder_request_response_set_response_payload_type_details(&service_builder_request_response,
                                                                                      iox2_type_variant_e_FIXED_SIZE,
                                                                                      response_type_name,
                                                                                      strlen(response_type_name),
                                                                                      sizeof(struct TransmissionData),
                                                                                      alignof(struct TransmissionData));
    if (ret_val != IOX2_OK) {
        printf("Unable to set response type details! Error: %d\n", ret_val);
        goto drop_service_name;
    }

    // Create service
    iox2_port_factory_request_response_h service = NULL;
    ret_val = iox2_service_builder_request_response_open_or_create(service_builder_request_response, NULL, &service);
    if (ret_val != IOX2_OK) {
        printf("Unable to create service! Error: %d\n", ret_val);
        goto drop_service_name;
    }

    // Create client
    iox2_port_factory_client_builder_h client_builder =
        iox2_port_factory_request_response_client_builder(&service, NULL);
    iox2_client_h client = NULL;
    ret_val = iox2_port_factory_client_builder_create(client_builder, NULL, &client);
    if (ret_val != IOX2_OK) {
        printf("Unable to create client! Error: %d\n", ret_val);
        goto drop_service;
    }

    // Start sending requests
    uint64_t samples = 0;

    for (size_t i = 0; i < msgCount; ++i) {
        // Loan request sample
        iox2_request_mut_h request = NULL;
        ret_val = iox2_client_loan_slice_uninit(&client, NULL, &request, 1);
        if (ret_val != IOX2_OK) {
            printf("Failed to loan request! Error: %d\n", ret_val);
            goto drop_client;
        }

        struct TransmissionData* payload = NULL;

        // Write payload
        iox2_request_mut_payload_mut(&request, (void**) &payload, NULL);
        payload->ns = now_ns();

        iox2_pending_response_h pending_response = NULL;

        // Send request
        ret_val = iox2_request_mut_send(request, NULL, &pending_response);
        if (ret_val != IOX2_OK) {
            printf("Failed to send request! Error: %d\n", ret_val);
            goto drop_client;
        }

        iox2_response_h response = NULL;

        while (response == NULL) {
            ret_val = iox2_pending_response_receive(&pending_response, NULL, &response);

            if (ret_val != IOX2_OK) {
                continue;
            }
        }

        const struct TransmissionData* response_data = NULL;

        iox2_response_payload(&response, (const void**) &response_data, NULL);

        latency[i] = now_ns() - response_data->ns;

        iox2_response_drop(response);

        iox2_pending_response_drop(pending_response);
        samples++;
    }

    qsort(latency, samples, sizeof(uint64_t), cmp_u64);

    // printf("Samples = %zu\n", samples);
    // printf("P50     = %lu ns\n", latency[(size_t) (samples * 0.50)]);
    // printf("P90     = %lu ns\n", latency[(size_t) (samples * 0.90)]);
    // printf("P99     = %lu ns\n", latency[(size_t) (samples * 0.99)]);
    // printf("P99.9   = %lu ns\n", latency[(size_t) (samples * 0.999)]);
    // fprintf(outfile, "msg_count,P50,P90,P99,P99.9\n");
    fprintf(outfile,
            "%lu,%lu,%lu,%lu,%lu\n",
            samples,
            latency[(size_t) (samples * 0.50)],
            latency[(size_t) (samples * 0.90)],
            latency[(size_t) (samples * 0.99)],
            latency[(size_t) (samples * 0.999)]);

drop_client:
    iox2_client_drop(client);

drop_service:
    iox2_port_factory_request_response_drop(service);

drop_service_name:
    iox2_service_name_drop(service_name);

drop_node:
    iox2_node_drop(node_handle);

end:
    free(latency);
    latency = NULL;
    fclose(outfile);

    return 0;
}
