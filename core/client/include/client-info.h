// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef __AQUA_CLIENT_INFO_H
#define __AQUA_CLIENT_INFO_H

#include "dsp-client.h"

#include <stdint.h>

void initClientInfo(const char *strId);

int32_t createReturnInfo();

struct ClientConnectInfo *getClientConnectInfo(int32_t);

struct ClientCallInfo *getClientCallInfo(int32_t);

struct ClientReturnInfo *getClientReturnInfo(int32_t);

struct ClientConnectRequestInformation *getClientConnRequestInfo(int32_t);

#endif // __AQUA_CLIENT_INFO_H
