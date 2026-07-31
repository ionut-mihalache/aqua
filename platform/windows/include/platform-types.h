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

#endif // __AQUA_WINDOWS_TYPES_H_
