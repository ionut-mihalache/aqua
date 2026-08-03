// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef __AQUA_WINDOWS_TYPES_H_
#define __AQUA_WINDOWS_TYPES_H_

#include <Windows.h>

#define AQUA_API_EXPORT __declspec(dllexport)

typedef SIZE_T aqua_size_t;
typedef SSIZE_T aqua_ssize_t;

typedef DWORD64 aqua_off_t;
typedef DWORD64 aqua_off64_t;
typedef DWORDLONG aqua_loff_t;

typedef DWORD aqua_pid_t;

typedef HANDLE aqua_file_handle_t;

typedef LONG aqua_sem_cnt_t;

enum {
    INSTALL_DATA_HANDLE,
    CONNECT_LIST_HANDLE,
    CONNECT_HANDLE,
    CONNECT_FULL_HANDLE,
    CONNECT_EMPTY_HANDLE,
    SEND_HANDLE,
    SEND_FULL_HANDLE,
    SEND_EMPTY_HANDLE,
    RECEIVE_HANDLE,
    RECEIVE_FULL_HANDLE,
    RECEIVE_EMPTY_HANDLE,
    CONNECT_RESPONSE_HANDLE,
    CONNECT_RESPONSE_FULL_HANDLE,
    CONNECT_RESPONSE_EMPTY_HANDLE,
    DISCONNECT_HANDLE,
    DISCONNECT_FULL_HANDLE,
    DISCONNECT_EMPTY_HANDLE,
    HANDLES_TOTAL // This needs to be always the last value
};

#endif // __AQUA_WINDOWS_TYPES_H_
