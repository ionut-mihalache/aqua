// Copyright (c) 2024 Contributors to the Eclipse Foundation
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

#ifndef IOX2_EXAMPLES_TRANSMISSION_DATA_BENCHMARK_H
#define IOX2_EXAMPLES_TRANSMISSION_DATA_BENCHMARK_H

#define MSG_COUNT 30000

#include <stdint.h>
#include <time.h>

#define PAYLOAD_SIZE (1024 * 1024)

struct TransmissionData {
    // int32_t x;
    // int32_t y;
    // double funky;
    uint64_t ns;
    uint8_t data[PAYLOAD_SIZE - sizeof(uint64_t)];
};

struct Payload64K {
    uint64_t send_ns;
    uint8_t data[64 * 1024 - sizeof(uint64_t)];
};

struct Payload256K {
    uint64_t send_ns;
    uint8_t data[256 * 1024 - sizeof(uint64_t)];
};

struct Payload1M {
    uint64_t send_ns;
    uint8_t data[1024 * 1024 - sizeof(uint64_t)];
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
