# x64-mingw-llvm-static: like x64-mingw-static but for llvm-mingw (UCRT) toolchain.
# Using a distinct triplet name prevents vcpkg from reusing GCC/MSVCRT-built packages.
set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)
set(VCPKG_ENV_PASSTHROUGH PATH LLVM_MINGW_ROOT)

set(VCPKG_CMAKE_SYSTEM_NAME MinGW)

# Tell vcpkg to use the llvm-mingw Clang toolchain for cross-compiling packages.
# Without this, vcpkg falls back to whatever MinGW GCC it finds on PATH, which causes
# conflicts with our -D_vsnprintf=vsnprintf flag (GCC stdio.h declares vsnprintf with
# C++ linkage, causing conflicting-declaration errors at compile time).
set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE "${CMAKE_CURRENT_LIST_DIR}/../../toolchains/llvm-mingw.cmake")

# llvm-mingw UCRT headers declare _vsnprintf as __declspec(dllimport) via _CRTIMP, but
# provide no import library stub for it. Redirect to the standard vsnprintf (C99, no DLL
# import) so packages like OpenSSL don't generate unresolvable __imp__vsnprintf references.
set(VCPKG_C_FLAGS "-D_vsnprintf=vsnprintf")
set(VCPKG_CXX_FLAGS "-D_vsnprintf=vsnprintf")
