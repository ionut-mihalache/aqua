# SPDX-License-Identifier: LGPL-2.1-or-later

cmake_minimum_required(VERSION 3.25.1)

project(dsp-library C)

set(CMAKE_C_STANDARD 23)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(LIB_VERSION "0.0.2")
set(LIB_NAME dsp)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
file(MAKE_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

set(CORE_SOURCES
        core/service/dsp-service.c
        core/service/install.c
        core/service/call.c
        core/service/return.c
        core/client/dsp-client.c
        core/client/client-connect.c
        core/client/client-call.c
        utils/commons.c
)

if (UNIX AND NOT APPLE)
        set(PLATFORM_SOURCES
                platform/linux/memory/memory.c
                platform/linux/alloc/alloc.c
                platform/linux/sync/sync.c
                platform/linux/shared-memory-object/shared-memory-object.c
                platform/linux/process/process.c
        )
elseif(WIN32)
        set(PLATFORM_SOURCES
                platform/windows/memory/memory.c
                platform/windows/alloc/alloc.c
                platform/windows/sync/sync.c
                platform/windows/shared-memory-object/shared-memory-object.c
                platform/windows/process/process.c
        )
endif()

add_library(${LIB_NAME} SHARED
        ${CORE_SOURCES}
        ${PLATFORM_SOURCES}
)

if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${LIB_NAME} PRIVATE
                -fvisibility=hidden
        )
endif()

set_target_properties(${LIB_NAME} PROPERTIES
        VERSION ${LIB_VERSION}
        SOVERSION ${LIB_VERSION}
        POSITION_INDEPENDENT_CODE ON
)

target_include_directories(${LIB_NAME}
PUBLIC
        utils
        utils/macros
        utils/hashmap
        utils/exit
        utils/log
        include
        core/service/include
        core/client/include

PRIVATE
        platform/include
)

if (UNIX AND NOT APPLE)
        target_include_directories(${LIB_NAME}
        PRIVATE
                platform/linux/include
        )

        target_compile_options(${LIB_NAME} PRIVATE
                -Wall
                -Wextra
                -Werror

                $<$<CONFIG:Debug>:-O0>
                $<$<CONFIG:Debug>:-g3>

                $<$<CONFIG:Release>:-O3>
        )

        target_compile_definitions(${LIB_NAME} PRIVATE
                _GNU_SOURCE
        )
elseif(WIN32)
        target_include_directories(${LIB_NAME}
        PRIVATE
                platform/windows/include
        )
        target_compile_options(${LIB_NAME} PRIVATE
                /W4
                /WX

                $<$<CONFIG:Debug>:/Od>
                $<$<CONFIG:Debug>:/Zi>

                $<$<CONFIG:Release>:/O2>
        )

        target_compile_definitions(${LIB_NAME} PRIVATE
                _CRT_SECURE_NO_WARNINGS
        )
endif()

target_compile_definitions(${LIB_NAME} PRIVATE
        $<$<CONFIG:Debug>:__AQUA_DEBUG__>
)
