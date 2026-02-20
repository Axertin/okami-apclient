set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Find the llvm-mingw installation.
# Set LLVM_MINGW_ROOT via environment variable or CMake cache variable.
# Falls back to /opt/llvm-mingw if neither is set.
# Download releases from https://github.com/mstorsjo/llvm-mingw/releases
if(NOT DEFINED LLVM_MINGW_ROOT)
    if(DEFINED ENV{LLVM_MINGW_ROOT})
        set(LLVM_MINGW_ROOT "$ENV{LLVM_MINGW_ROOT}" CACHE PATH "llvm-mingw installation root")
    else()
        set(LLVM_MINGW_ROOT "/opt/llvm-mingw" CACHE PATH "llvm-mingw installation root")
    endif()
endif()

find_program(CMAKE_C_COMPILER
    NAMES x86_64-w64-mingw32-clang
    PATHS "${LLVM_MINGW_ROOT}/bin"
    NO_DEFAULT_PATH)
find_program(CMAKE_CXX_COMPILER
    NAMES x86_64-w64-mingw32-clang++
    PATHS "${LLVM_MINGW_ROOT}/bin"
    NO_DEFAULT_PATH)
find_program(CMAKE_RC_COMPILER
    NAMES x86_64-w64-mingw32-windres llvm-windres
    PATHS "${LLVM_MINGW_ROOT}/bin"
    NO_DEFAULT_PATH)

if(NOT CMAKE_C_COMPILER OR NOT CMAKE_CXX_COMPILER)
    message(FATAL_ERROR
        "llvm-mingw not found at '${LLVM_MINGW_ROOT}'.\n"
        "Download a Linux x86_64 release (ucrt variant) from:\n"
        "  https://github.com/mstorsjo/llvm-mingw/releases\n"
        "Extract it and set LLVM_MINGW_ROOT to the directory, e.g.:\n"
        "  export LLVM_MINGW_ROOT=/opt/llvm-mingw")
endif()

# The sysroot lives inside the llvm-mingw installation
set(CMAKE_FIND_ROOT_PATH "${LLVM_MINGW_ROOT}/x86_64-w64-mingw32")

# Allow vcpkg paths to be searched alongside the sysroot
if(VCPKG_TARGET_TRIPLET)
    list(APPEND CMAKE_FIND_ROOT_PATH "${CMAKE_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}")
endif()

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)

# Do NOT use -static here: UCRT is a Windows system DLL (always present on Win10+) and
# is only available as a DLL import — forcing -static breaks __declspec(dllimport) resolution.
# libc++ and libunwind are linked statically in the target CMakeLists instead.

add_definitions(-D_WIN32_WINNT=0x0601)
