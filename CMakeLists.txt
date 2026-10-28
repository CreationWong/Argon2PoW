# SPDX-License-Identifier: GPL-3.0-only
cmake_minimum_required(VERSION 3.21)

project(Argon2PoW VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Threads REQUIRED)

qt_standard_project_setup()

# ---------------------------------------------------------------------------
# Argon2 reference implementation (CC0 / Apache-2.0), vendored under
# third_party/argon2. The optimized x86-64 code path is used on GNU/Clang;
# MSVC and non-x86 targets fall back to the portable reference path.
# ---------------------------------------------------------------------------
set(ARGON2_SOURCES
    third_party/argon2/src/argon2.c
    third_party/argon2/src/core.c
    third_party/argon2/src/encoding.c
    third_party/argon2/src/thread.c
    third_party/argon2/src/blake2/blake2b.c
)

if(MSVC)
    list(APPEND ARGON2_SOURCES third_party/argon2/src/ref.c)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64|amd64)$")
    list(APPEND ARGON2_SOURCES third_party/argon2/src/opt.c)
else()
    list(APPEND ARGON2_SOURCES third_party/argon2/src/ref.c)
endif()

add_library(argon2_vendor STATIC ${ARGON2_SOURCES})
target_include_directories(argon2_vendor
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/argon2/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/argon2/src
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/argon2/src/blake2
)
target_link_libraries(argon2_vendor PUBLIC Threads::Threads)
if(MSVC)
    target_compile_options(argon2_vendor PRIVATE /O2 /W0)
else()
    target_compile_options(argon2_vendor PRIVATE -O3 -fno-strict-aliasing -Wno-unused-parameter)
endif()
set_target_properties(argon2_vendor PROPERTIES C_VISIBILITY_PRESET hidden)

# ---------------------------------------------------------------------------
# Shared PoW core: challenge parsing, exact reference-compatible hashing,
# verification, and an ordered multi-threaded search engine.
# ---------------------------------------------------------------------------
add_library(pow_core STATIC
    src/common/pow.cpp
    src/common/pow.h
)
target_link_libraries(pow_core
    PUBLIC Qt6::Core
    PRIVATE argon2_vendor Threads::Threads
)

add_library(pow_ui STATIC
    src/common/ui.cpp
    src/common/ui.h
    src/common/i18n.cpp
    src/common/i18n.h
)
target_link_libraries(pow_ui
    PUBLIC Qt6::Widgets pow_core
)

# ---------------------------------------------------------------------------
# Applications
# ---------------------------------------------------------------------------
add_library(challenge_ui STATIC
    src/generator/challengewindow.cpp
    src/generator/challengewindow.h
)
target_link_libraries(challenge_ui PUBLIC pow_ui)
qt_add_executable(Argon2PoWChallenge WIN32 src/generator/main.cpp)
target_link_libraries(Argon2PoWChallenge PRIVATE challenge_ui)
set_target_properties(Argon2PoWChallenge PROPERTIES
    OUTPUT_NAME "Argon2PoW-Challenge"
    WIN32_EXECUTABLE TRUE
)

add_library(solver_ui STATIC
    src/solver/solverwindow.cpp
    src/solver/solverwindow.h
    src/solver/solverworker.cpp
    src/solver/solverworker.h
)
target_link_libraries(solver_ui PUBLIC pow_ui)
qt_add_executable(Argon2PoWSolver WIN32 src/solver/main.cpp)
target_link_libraries(Argon2PoWSolver PRIVATE solver_ui)
set_target_properties(Argon2PoWSolver PROPERTIES
    OUTPUT_NAME "Argon2PoW-Solver"
    WIN32_EXECUTABLE TRUE
)

# ---------------------------------------------------------------------------
# Tests and installation
# ---------------------------------------------------------------------------
qt_add_lrelease(pow_ui
    TS_FILES translations/argon2pow_en.ts
    QM_FILES_OUTPUT_VARIABLE app_translations
    OPTIONS -nounfinished
)
qt_add_resources(pow_ui translations
    PREFIX "/i18n"
    BASE "${CMAKE_CURRENT_BINARY_DIR}"
    FILES ${app_translations}
)

foreach(target pow_core pow_ui challenge_ui solver_ui Argon2PoWChallenge Argon2PoWSolver)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /utf-8)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic
            "$<$<CONFIG:Release>:-ffunction-sections;-fdata-sections>")
        target_link_options(${target} PRIVATE "$<$<CONFIG:Release>:-Wl,--gc-sections>")
    endif()
endforeach()

include(CTest)
if(BUILD_TESTING)
    add_executable(pow_tests tests/pow_tests.cpp)
    target_link_libraries(pow_tests PRIVATE pow_core)
    add_test(NAME pow_tests COMMAND pow_tests)
    set_tests_properties(pow_tests PROPERTIES TIMEOUT 120)

    add_executable(ui_tests tests/ui_tests.cpp)
    target_link_libraries(ui_tests PRIVATE challenge_ui solver_ui)
    add_test(NAME ui_tests_zh COMMAND ui_tests --lang zh_CN)
    add_test(NAME ui_tests_en COMMAND ui_tests --lang en)

    add_executable(i18n_tests tests/i18n_tests.cpp)
    target_link_libraries(i18n_tests PRIVATE pow_ui)
    add_test(NAME i18n_tests COMMAND i18n_tests "${CMAKE_CURRENT_SOURCE_DIR}/translations/argon2pow_en.ts")
    set_tests_properties(ui_tests_zh ui_tests_en i18n_tests PROPERTIES
        TIMEOUT 60 ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
endif()

include(GNUInstallDirs)
install(TARGETS Argon2PoWChallenge Argon2PoWSolver
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(FILES third_party/argon2/LICENSE
    DESTINATION ${CMAKE_INSTALL_DATADIR}/Argon2PoW/licenses
    RENAME LICENSE-argon2.txt
)
install(FILES README.md LICENSE
    DESTINATION ${CMAKE_INSTALL_DATADIR}/Argon2PoW
)
