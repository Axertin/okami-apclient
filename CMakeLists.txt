# CMakeList.txt : CMake project for okami-apclient, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.21)

function(enable_strict_warnings target)
  set_property(GLOBAL PROPERTY ENABLE_STRICT_WARNINGS_DEFINED TRUE)
  get_target_property(target_type ${target} TYPE)

  if(NOT target_type STREQUAL "INTERFACE_LIBRARY")
    set(extra_warnings TRUE)
  endif()

  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "GNU")
    target_compile_options(${target} PRIVATE -Wall)

    if(extra_warnings)
      target_compile_options(${target} PRIVATE -Wextra -Wshadow -Wconversion)
    endif()

    if(CMAKE_BUILD_TYPE MATCHES "Release|RelWithDebInfo")
      # target_compile_options(${target} PRIVATE -Werror)
    endif()

  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" OR CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    target_compile_options(${target} PRIVATE /W4 /permissive-)

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
      target_compile_options(${target} PRIVATE
        /clang:-Wno-c++98-compat
        /clang:-Wno-c++98-compat-pedantic
        /clang:-Wno-c++17-extensions
        /clang:-Wno-unused-lambda-capture
      )

      if(extra_warnings)
        target_compile_options(${target} PRIVATE
          /clang:-Wold-style-cast
          /clang:-Wextra-semi
          /clang:-Wmicrosoft-include
          /clang:-Wshadow
          /clang:-Wunused-parameter
          /clang:-Wnon-virtual-dtor
          /clang:-Wmissing-variable-declarations
          /clang:-Wheader-hygiene
          /clang:-Wnonportable-include-path
          /clang:-Wpragma-pack
        )
      endif()
    endif()

    if(CMAKE_BUILD_TYPE MATCHES "Release|RelWithDebInfo")
      # target_compile_options(${target} PRIVATE /WX)
    endif()

    # Prevent Edit-and-Continue even in Debug
    if(POLICY CMP0141)
      set_property(TARGET ${target} PROPERTY MSVC_DEBUG_INFORMATION_FORMAT ProgramDatabase)
    endif()

  endif()
endfunction()

# option to build tests only (for Linux development)
option(BUILD_TESTS_ONLY "Build only the test target, skip Windows-specific targets" OFF)

# Generate client source list if needed
execute_process(
  COMMAND ${CMAKE_COMMAND} -DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/generate_sources.cmake
)
include(${CMAKE_CURRENT_SOURCE_DIR}/src/client/sources.cmake)

# Add GitVersion to cmake module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(GitVersion)

# grab SSL keys
if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/cacert.pem)
  file(DOWNLOAD https://curl.se/ca/cacert.pem ${CMAKE_CURRENT_BINARY_DIR}/cacert.pem SHOW_PROGRESS)
endif()

# Enable Hot Reload for MSVC compilers if supported.
if(POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" OR CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>")
  endif()
endif()

if(POLICY CMP0091)
  cmake_policy(SET CMP0091 NEW)
endif()

# Fallback default if not passed via preset
set(CXX_STD "23" CACHE STRING "Requested C++ standard")
set(STD_FEATURE "cxx_std_${CXX_STD}")

project("okami-apclient" LANGUAGES CXX)

if(BUILD_TESTS_ONLY)
  message(STATUS "Building tests only - skipping Windows-specific dependencies")
  # Only find packages needed for tests
  find_package(nlohmann_json CONFIG REQUIRED)
  find_package(asio CONFIG REQUIRED)
  find_package(OpenSSL REQUIRED)
  find_package(ZLIB REQUIRED)
  find_package(Catch2 CONFIG REQUIRED)

else()
  # dependencies
  find_package(minhook CONFIG REQUIRED)
  find_package(nlohmann_json CONFIG REQUIRED)
  find_package(asio CONFIG REQUIRED)
  find_package(OpenSSL REQUIRED)
  find_package(ZLIB REQUIRED)
  find_package(Catch2 CONFIG REQUIRED)
endif()

# Glob tree for sources and includes, and put them in lists
file(GLOB_RECURSE tests CONFIGURE_DEPENDS "test/*.cpp")

if(NOT BUILD_TESTS_ONLY)
  # imgui
  add_library(imgui STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/imgui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/imgui_draw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/imgui_widgets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/imgui_tables.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/backends/imgui_impl_dx11.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/backends/imgui_impl_win32.cpp
  )

  target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/backends)
  target_compile_features(imgui PRIVATE ${STD_FEATURE})

  # Primary program
  add_library(okami-apclient SHARED ${CLIENT_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/include/okami/memorymap.cpp)
  target_precompile_headers(okami-apclient PRIVATE
    <algorithm>
    <array>
    <chrono>
    <cstdint>
    <functional>
    <iostream>
    <memory>
    <optional>
    <string>
    <thread>
    <unordered_map>
    <vector>
    "${CMAKE_CURRENT_SOURCE_DIR}/src/client/framework.h"
    [["imgui.h"]]
    [["nlohmann/json.hpp"]]
  )
  target_compile_definitions(okami-apclient PRIVATE ASIO_STANDALONE AP_NO_SCHEMA _WIN32_WINNT=0x0601)
  target_include_directories(okami-apclient SYSTEM PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/apclientpp
    ${CMAKE_CURRENT_SOURCE_DIR}/external/wswrap/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/websocketpp
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/backends)
  target_link_libraries(okami-apclient PRIVATE minhook::minhook nlohmann_json::nlohmann_json asio::asio OpenSSL::SSL OpenSSL::Crypto ZLIB::ZLIB imgui d3d11 dxgi ws2_32)
  target_compile_features(okami-apclient PRIVATE ${STD_FEATURE})
  enable_strict_warnings(okami-apclient)

  if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_link_libraries(okami-apclient PRIVATE Crypt32)
    target_compile_options(okami-apclient PRIVATE /Zc:__cplusplus)
  endif()

  add_version_info(TARGET okami-apclient)
  get_target_property(_version okami-apclient VERSION_STRING)

  # Write out version to file (for CI use)
  file(WRITE ${CMAKE_BINARY_DIR}/version.txt
    "${_version}")

  # Bootstrap loader DLL
  add_library(apclient-loader SHARED ${CMAKE_CURRENT_SOURCE_DIR}/src/loader/dllmain.cpp)
  target_precompile_headers(apclient-loader PRIVATE
    <iostream>
    "${CMAKE_CURRENT_SOURCE_DIR}/src/loader/framework.h"
  )
  target_compile_definitions(apclient-loader PRIVATE ASIO_STANDALONE AP_NO_SCHEMA _WIN32_WINNT=0x0601)
  target_link_libraries(apclient-loader PRIVATE SHlwapi)
  target_include_directories(apclient-loader PRIVATE)
  target_compile_features(apclient-loader PRIVATE ${STD_FEATURE})
  enable_strict_warnings(apclient-loader)

endif()

if(WIN32)
  set(TESTED_UNITS
    ${CLIENT_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/include/okami/memorymap.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/out/version/okami_apclient/okami_apclient-GitVersion.cc)
else()
  set(TESTED_UNITS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/okami/memorymap.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/client/checks.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/client/logger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/client/loginwindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/client/receive.cpp
  )
endif()


# Tests
add_executable(okami-tests ${tests} ${TESTED_UNITS})
target_compile_definitions(okami-tests PRIVATE ASIO_STANDALONE AP_NO_SCHEMA)

# Conditional linking based on platform and build mode
if(BUILD_TESTS_ONLY)
  # Linux-only test build - exclude Windows-specific libraries
  target_link_libraries(okami-tests PRIVATE
    Catch2::Catch2WithMain
    nlohmann_json::nlohmann_json
    asio::asio
    OpenSSL::SSL
    OpenSSL::Crypto
    ZLIB::ZLIB
  )
  # target_include_directories(okami-tests SYSTEM PRIVATE /usr/x86_64-w64-mingw32ucrt/sys-root/mingw/include)
  # Don't set _WIN32_WINNT for Linux builds
else()
  # Full Windows build
  target_compile_definitions(okami-tests PRIVATE _WIN32_WINNT=0x0601)
  target_link_libraries(okami-tests PRIVATE
    Catch2::Catch2WithMain
    minhook::minhook
    nlohmann_json::nlohmann_json
    asio::asio
    OpenSSL::SSL
    OpenSSL::Crypto
    ZLIB::ZLIB
    imgui
    d3d11
    dxgi
    ws2_32
  )
endif()

target_include_directories(okami-tests PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src/client
  ${CMAKE_CURRENT_SOURCE_DIR}/external/apclientpp
  ${CMAKE_CURRENT_SOURCE_DIR}/external/wswrap/include
  ${CMAKE_CURRENT_SOURCE_DIR}/external/websocketpp
  ${CMAKE_CURRENT_SOURCE_DIR}/out/version/okami_apclient)

# Only include imgui directories if not building tests only
if(NOT BUILD_TESTS_ONLY)
  target_include_directories(okami-tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/backends)
endif()
target_compile_features(okami-tests PRIVATE ${STD_FEATURE})

include(CTest)
include(Catch)
catch_discover_tests(okami-tests)

# Install
# TODO: Allow override of install_prefix
if(NOT BUILD_TESTS_ONLY)
  set(CMAKE_INSTALL_PREFIX "d:/SteamLibrary/steamapps/common/Okami")
  install(TARGETS apclient-loader DESTINATION mods)
  install(TARGETS okami-apclient DESTINATION mods/apclient)
  install(FILES ${CMAKE_BINARY_DIR}/cacert.pem DESTINATION mods/apclient)
endif()
