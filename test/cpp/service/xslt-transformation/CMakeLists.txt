cmake_minimum_required(VERSION 3.25)

project(aqua_test_service LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

get_filename_component(AQUA_ROOT
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../.."
    ABSOLUTE
)

add_executable(aqua_test_service
    main.cpp
)

target_include_directories(aqua_test_service PRIVATE
    "${AQUA_ROOT}/include"
    "${AQUA_ROOT}/core/service/include"
    "${AQUA_ROOT}/core/client/include"

    # Aqua platform-specific headers
    "${AQUA_ROOT}/platform/include"
    "${AQUA_ROOT}/platform/windows/include"
)

target_link_libraries(aqua_test_service PRIVATE
    "${AQUA_ROOT}/build/lib/shared/aqua.lib"
)

add_custom_command(TARGET aqua_test_service POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${AQUA_ROOT}/build/lib/shared/aqua.dll"
        "$<TARGET_FILE_DIR:aqua_test_service>"
)
