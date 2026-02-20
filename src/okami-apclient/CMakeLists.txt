execute_process(
    COMMAND ${CMAKE_COMMAND}
    -DSOURCE_DIR=${CMAKE_SOURCE_DIR}
    -DTARGET_NAME=okami-apclient
    -DSEARCH_DIR=src/okami-apclient
    -P ${CMAKE_SOURCE_DIR}/cmake/generate_sources.cmake
)
include(${CMAKE_CURRENT_SOURCE_DIR}/sources.cmake)

find_package(minhook CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(asio CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

add_library(okami-apclient SHARED ${okami-apclient_SOURCES})
target_compile_definitions(okami-apclient PRIVATE
    ASIO_STANDALONE
    AP_NO_SCHEMA
    _WIN32_WINNT=0x0601
    _WEBSOCKETPP_CPP11_STL_
    _WEBSOCKETPP_CPP11_THREAD_
    _WEBSOCKETPP_NO_BOOST_)
target_compile_features(okami-apclient PUBLIC ${STD_FEATURE})
target_include_directories(okami-apclient PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}/generated
)
target_link_libraries(okami-apclient PRIVATE minhook::minhook nlohmann_json::nlohmann_json asio::asio OpenSSL::SSL OpenSSL::Crypto ZLIB::ZLIB imgui ws2_32)
target_include_directories(okami-apclient SYSTEM PRIVATE
    ${CMAKE_SOURCE_DIR}/external/apclientpp
    ${CMAKE_SOURCE_DIR}/external/wswrap/include
    ${CMAKE_SOURCE_DIR}/external/websocketpp
    ${CMAKE_SOURCE_DIR}/external/imgui
    ${CMAKE_SOURCE_DIR}/external/imgui/backends
)

# Add D3D11 libraries for ImGui backend
target_link_libraries(okami-apclient PRIVATE d3d11 dxgi)

enable_strict_warnings(okami-apclient)
apply_release_optimizations(okami-apclient)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_link_libraries(okami-apclient PRIVATE Crypt32)
    target_compile_options(okami-apclient PRIVATE /Zc:__cplusplus)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CROSS_COMPILING_WINDOWS)
    target_link_libraries(okami-apclient PRIVATE dwmapi d3dcompiler)
    target_compile_options(okami-apclient PRIVATE -Wa,-mbig-obj -g1)
    target_link_options(okami-apclient PRIVATE
        -static-libgcc
        -static-libstdc++
        -static
        LINKER:--allow-multiple-definition
        LINKER:--enable-stdcall-fixup)
    # websocketpp uses template-id ctor/dtor syntax (e.g. basic<T,U>()) which is ill-formed
    # in C++20 per CWG DR 2037. GCC 13.3+ demotes this to a suppressible warning via
    # -Wno-template-id-cdtor. GCC 13.2 and earlier treat it as a hard parse error with no
    # workaround short of modifying the library. Ubuntu ships GCC 13.2 for mingw-w64 across
    # all releases through 25.10 with no newer package available via apt.
    # Minimum required MinGW-w64 cross-compiler: GCC 13.3.0
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "13.3.0")
        message(FATAL_ERROR
            "MinGW-w64 cross-compiler GCC ${CMAKE_CXX_COMPILER_VERSION} is not supported.\n"
            "websocketpp uses template-id ctor/dtor syntax (CWG DR 2037) that GCC < 13.3 "
            "rejects as a hard parse error in C++20 mode with no pragma-level workaround.\n"
            "Ubuntu 24.04–25.10 only ship GCC 13.2 for mingw-w64 (no newer apt package).\n"
            "To cross-compile, use llvm-mingw (cmake/toolchains/llvm-mingw.cmake) "
            "or a system with GCC 13.3+ mingw-w64 (e.g. Fedora), "
            "or build using the Windows/MSVC presets instead.")
    else()
        target_compile_options(okami-apclient PRIVATE -Wno-template-id-cdtor)
    endif()
    # Remove lib prefix and skip import library generation
    set_target_properties(okami-apclient PROPERTIES
        PREFIX ""
        IMPORT_SUFFIX ""
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/implib")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CROSS_COMPILING_WINDOWS)
    # llvm-mingw cross-compilation: Clang uses the LLVM integrated assembler (no -Wa,-mbig-obj
    # needed) and has no libgcc (no -static-libgcc). websocketpp compiles cleanly with Clang.
    # UCRT is a Windows system DLL (present on Win10+); don't force -static which would break
    # its __declspec(dllimport) symbols. Instead, link libc++/libunwind as static archives so
    # the DLL has no dependency on libc++.dll or libunwind.dll.
    target_link_libraries(okami-apclient PRIVATE dwmapi d3dcompiler)
    target_compile_options(okami-apclient PRIVATE -g1)
    target_link_options(okami-apclient PRIVATE
        LINKER:--allow-multiple-definition
        LINKER:--enable-stdcall-fixup)
    target_link_libraries(okami-apclient PRIVATE -l:libc++.a -l:libc++abi.a -l:libunwind.a)
    # Remove lib prefix and skip import library generation
    set_target_properties(okami-apclient PROPERTIES
        PREFIX ""
        IMPORT_SUFFIX ""
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/implib")
endif()

# Add git version info and Windows resources
add_git_version_info(okami-apclient)

# Only install the DLL, not the import library
install(TARGETS okami-apclient RUNTIME DESTINATION apclient/)
