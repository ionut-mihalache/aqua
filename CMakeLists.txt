# SPDX-License-Identifier: LGPL-2.1-or-later

cmake_minimum_required(VERSION 3.25.1)

project(
        aqua
        VERSION 0.0.3
        LANGUAGES C
)

message(STATUS "Project version: ${PROJECT_VERSION}")

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(LIB_NAME aqua)

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

add_library(aqua_objects OBJECT
        ${CORE_SOURCES}
        ${PLATFORM_SOURCES}
)

set(LIB_OUTPUT_DIR ${CMAKE_BINARY_DIR}/lib)

if (CMAKE_CONFIGURATION_TYPES)
        set(SHARED_OUTPUT_DIR ${LIB_OUTPUT_DIR}/$<CONFIG>/shared)
        set(STATIC_OUTPUT_DIR ${LIB_OUTPUT_DIR}/$<CONFIG>/static)
else()
        set(SHARED_OUTPUT_DIR ${LIB_OUTPUT_DIR}/shared)
        set(STATIC_OUTPUT_DIR ${LIB_OUTPUT_DIR}/static)

        file(MAKE_DIRECTORY
                ${SHARED_OUTPUT_DIR}
                ${STATIC_OUTPUT_DIR}
        )
endif()

set_target_properties(aqua_objects PROPERTIES
        POSITION_INDEPENDENT_CODE ON
)

add_library(${LIB_NAME} SHARED
        $<TARGET_OBJECTS:aqua_objects>
)

set_target_properties(${LIB_NAME} PROPERTIES
        OUTPUT_NAME ${LIB_NAME}

        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}

        RUNTIME_OUTPUT_DIRECTORY ${SHARED_OUTPUT_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${SHARED_OUTPUT_DIR}
        ARCHIVE_OUTPUT_DIRECTORY ${SHARED_OUTPUT_DIR}
)

add_library(${LIB_NAME}_static STATIC
        $<TARGET_OBJECTS:aqua_objects>
)

set_target_properties(${LIB_NAME}_static PROPERTIES
        OUTPUT_NAME ${LIB_NAME}

        VERSION ${PROJECT_VERSION}

        ARCHIVE_OUTPUT_DIRECTORY ${STATIC_OUTPUT_DIR}
)

function(configure_aqua_target target)

        if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
                target_compile_options(${target} PRIVATE
                        -fvisibility=hidden
                )
        endif()

        target_include_directories(${target}
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
                target_include_directories(${target}
                PRIVATE
                        platform/linux/include
                )

                target_compile_options(${target} PRIVATE
                        -Wall
                        -Wextra
                        -Werror

                        $<$<CONFIG:Debug>:-O0>
                        $<$<CONFIG:Debug>:-g3>

                        $<$<CONFIG:Release>:-O3>
                )

                target_compile_definitions(${target} PRIVATE
                        _GNU_SOURCE
                )
        elseif(WIN32)
                if(MSVC)
                        target_compile_options(${target} PRIVATE /MP)
                endif()

                target_include_directories(${target}
                PRIVATE
                        platform/windows/include
                )
                target_compile_options(${target} PRIVATE
                        /W4
                        /WX

                        $<$<CONFIG:Debug>:/Od>
                        $<$<CONFIG:Debug>:/Zi>

                        $<$<CONFIG:Release>:/O2>
                )

                target_compile_definitions(${target} PRIVATE
                        _CRT_SECURE_NO_WARNINGS
                )
        endif()

        target_compile_definitions(${target} PRIVATE
                $<$<CONFIG:Debug>:__AQUA_DEBUG__>
        )

endfunction()

configure_aqua_target(aqua_objects)
