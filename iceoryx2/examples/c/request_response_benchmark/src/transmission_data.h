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

#ifndef IOX2_EXAMPLES_TRANSMISSION_DATA_H
#define IOX2_EXAMPLES_TRANSMISSION_DATA_H

#define PAYLOAD_SIZE_64K (64 * 1024)
#define PAYLOAD_SIZE_256K (256 * 1024)
#define PAYLOAD_SIZE_1M (1024 * 1024)
#define MSG_COUNT 30000

#include <stdint.h>
#include <time.h>

struct TransmissionData {
    // int32_t x;
    // int32_t y;
    // double funky;
    uint64_t ns;
    uint8_t data[PAYLOAD_SIZE_64K - sizeof(uint64_t)];
};

static inline uint64_t now_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);

    return ((uint64_t)ts.tv_sec * 1000000000ULL) + ts.tv_nsec;
}

static int cmp_u64(const void* a, const void* b) {
    uint64_t aa = *(const uint64_t*)a;
    uint64_t bb = *(const uint64_t*)b;

    return (aa > bb) - (aa < bb);
}

#endif
