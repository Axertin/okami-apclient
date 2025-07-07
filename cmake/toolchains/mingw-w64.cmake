set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Find MinGW-w64 toolchain
find_program(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
find_program(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
find_program(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)

if(NOT CMAKE_C_COMPILER OR NOT CMAKE_CXX_COMPILER)
    message(FATAL_ERROR "MinGW-w64 not found. Please install gcc-mingw-w64-x86-64 and g++-mingw-w64-x86-64")
endif()

# Where to look for libraries and headers
set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32)

# IMPORTANT: Allow vcpkg paths to be searched
if(VCPKG_TARGET_TRIPLET)
    list(APPEND CMAKE_FIND_ROOT_PATH "${CMAKE_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}")
endif()

# Adjust the default behavior of the find commands:
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
# CHANGED: Allow both root path and build tree for packages
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)

# Enable static linking by default for cross-compilation
# set(CMAKE_EXE_LINKER_FLAGS_INIT "-static-libgcc -static-libstdc++")
# set(CMAKE_SHARED_LINKER_FLAGS_INIT "-static-libgcc -static-libstdc++")

# Set minimum Windows version
add_definitions(-D_WIN32_WINNT=0x0601)
