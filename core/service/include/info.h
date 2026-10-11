// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef __AQUA_SERVICE_INFO_H
#define __AQUA_SERVICE_INFO_H

#include "dsp-service.h"

int32_t createReturnInfo();

struct ServiceConnectInfo *getConnectInfo(int32_t);

struct ServiceCallInfo *getCallInfo(int32_t);

struct ServiceReturnInfo *getReturnInfo(int32_t);

void freeConnectInfo(int32_t connectInfoHandle);

void freeCallInfo(int32_t callInfoHandle);

void freeReturnInfo(int32_t returnInfoHandle);

#endif // __AQUA_SERVICE_INFO_H
