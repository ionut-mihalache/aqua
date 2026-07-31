#ifndef __AQUA_WINDOWS_UTILS_H_
#define __AQUA_WINDOWS_UTILS_H_

#include <stdio.h>
#include <Windows.h>

#define DIE(assertion, call_description)                                       \
    do {                                                                       \
        if (assertion) {                                                       \
            DWORD errNumber = GetLastError();                                  \
            LPVOID errorStr = NULL;                                            \
            FormatMessage(                                                     \
                FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |  \
                    FORMAT_MESSAGE_IGNORE_INSERTS,                             \
                NULL, errNumber, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),    \
                (LPTSTR) & errorStr, 0, NULL);                                 \
                                                                               \
            fprintf(stderr, "%s (%d): %s - %s\n", __FILE__, __LINE__,          \
                    call_description, (char *)errorStr);                       \
            if (errorStr) {                                                    \
                LocalFree(errorStr);                                           \
            }                                                                  \
            ExitProcess(EXIT_FAILURE);                                         \
        }                                                                      \
    } while (0)

#endif // __AQUA_WINDOWS_UTILS_H_
