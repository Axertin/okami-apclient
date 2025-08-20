# CMakeList.txt : CMake project for okami-apclient, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.21)

include(cmake/compiler_setup.cmake)
include(cmake/resource_files.cmake)

# option to build tests only (for Linux development)
option(BUILD_TESTS_ONLY "Build only the test target, skip Windows-specific targets" OFF)

# Generate client source list if needed
execute_process(
  COMMAND ${CMAKE_COMMAND}
  -DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
  -DTARGET_NAME=client
  -DSEARCH_DIR=src/client
  -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/generate_sources.cmake
)
include(${CMAKE_CURRENT_SOURCE_DIR}/src/client/sources.cmake)

# Generate library source list if needed
execute_process(
  COMMAND ${CMAKE_COMMAND}
  -DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
  -DTARGET_NAME=library
  -DSEARCH_DIR=src/library
  -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/generate_sources.cmake
)
include(${CMAKE_CURRENT_SOURCE_DIR}/src/library/sources.cmake)

# Add GitVersion to cmake module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(SimpleGitVersion)

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

set(CXX_STD "23" CACHE STRING "Requested C++ standard")
set(STD_FEATURE "cxx_std_${CXX_STD}")

project("okami-apclient" LANGUAGES CXX)

# Cross-compilation detection  
if(CMAKE_CROSSCOMPILING AND CMAKE_SYSTEM_NAME STREQUAL "Windows")
  set(CROSS_COMPILING_WINDOWS TRUE)
  message(STATUS "Cross-compiling for Windows from ${CMAKE_HOST_SYSTEM_NAME}")
else()
  set(CROSS_COMPILING_WINDOWS FALSE)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CROSSCOMPILING)
  set(VCPKG_APPLOCAL_DEPS OFF CACHE BOOL "Disable vcpkg app-local dependencies" FORCE)
endif()

if(BUILD_TESTS_ONLY)
  message(STATUS "Building tests only - skipping Windows-specific dependencies")
  # Only find packages needed for tests
  find_package(nlohmann_json CONFIG REQUIRED)
  find_package(asio CONFIG REQUIRED)
  find_package(OpenSSL REQUIRED)
  find_package(ZLIB REQUIRED)
  find_package(Catch2 CONFIG REQUIRED)
  find_package(yaml-cpp CONFIG REQUIRED)

else()
  # dependencies
  find_package(minhook CONFIG REQUIRED)
  find_package(nlohmann_json CONFIG REQUIRED)
  find_package(asio CONFIG REQUIRED)
  find_package(OpenSSL REQUIRED)
  find_package(ZLIB REQUIRED)
  find_package(Catch2 CONFIG REQUIRED)
  find_package(yaml-cpp CONFIG REQUIRED)
endif()

file(GLOB_RECURSE tests CONFIGURE_DEPENDS "test/*.cpp")

##################################################
# Game library (memory structures and such)
##################################################
add_library(okami-library STATIC ${library_SOURCES})
target_include_directories(okami-library PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(okami-library PUBLIC ${STD_FEATURE})
target_link_libraries(okami-library PUBLIC yaml-cpp::yaml-cpp)
enable_strict_warnings(okami-library)

if(NOT BUILD_TESTS_ONLY)
  ##################################################
  # imgui
  ##################################################
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

  ##################################################
  # Primary program
  ##################################################
  add_library(okami-apclient SHARED ${client_SOURCES})
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
    ${CMAKE_CURRENT_SOURCE_DIR}/external/apclientpp
    ${CMAKE_CURRENT_SOURCE_DIR}/external/wswrap/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/websocketpp
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/backends)
  target_link_libraries(okami-apclient PRIVATE minhook::minhook nlohmann_json::nlohmann_json asio::asio OpenSSL::SSL OpenSSL::Crypto ZLIB::ZLIB imgui d3d11 dxgi ws2_32 okami-library)
  target_compile_features(okami-apclient PRIVATE ${STD_FEATURE})
  enable_strict_warnings(okami-apclient)
  apply_release_optimizations(okami-apclient)

  if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_link_libraries(okami-apclient PRIVATE Crypt32)
    target_compile_options(okami-apclient PRIVATE /Zc:__cplusplus)
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CROSS_COMPILING_WINDOWS)
    target_link_libraries(okami-apclient PRIVATE dwmapi d3dcompiler)
    target_compile_definitions(okami-apclient PRIVATE
      _WEBSOCKETPP_CPP11_STL_
      _WEBSOCKETPP_CPP11_THREAD_
      _WEBSOCKETPP_NO_BOOST_)
    target_compile_options(okami-apclient PRIVATE -Wa,-mbig-obj -g1)
    target_link_options(okami-apclient PRIVATE
      -static-libgcc
      -static-libstdc++
      -static
      LINKER:--allow-multiple-definition
      LINKER:--enable-stdcall-fixup)
  endif()

  add_git_version_info(okami-apclient)
  get_target_property(_version okami-apclient VERSION_STRING)
  add_windows_resources(okami-apclient)

  ##################################################
  # Injector
  ##################################################
  add_executable(okami-loader
    ${CMAKE_CURRENT_SOURCE_DIR}/src/loader/main.cpp
  )
  target_precompile_headers(okami-loader PRIVATE
    <iostream>
  )
  target_include_directories(okami-loader PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/loader)
  target_compile_features(okami-loader PRIVATE ${STD_FEATURE})
  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CROSS_COMPILING_WINDOWS)
    target_compile_options(okami-loader PRIVATE -Wa,-mbig-obj -g1)
    target_link_options(okami-loader PRIVATE
      -static-libgcc
      -static-libstdc++
      -static
      LINKER:--allow-multiple-definition
      LINKER:--enable-stdcall-fixup)
  endif()
  add_git_version_info(okami-loader)
  enable_strict_warnings(okami-loader)
  apply_release_optimizations(okami-loader)
  add_windows_resources(okami-loader)

  
  ##################################################
  # Proxy DLL
  ##################################################
  add_library(okami-proxy-dll SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/src/proxy-dll/dinput8.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/proxy-dll/modloader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/proxy-dll/dinput8.def
  )
  target_link_libraries(okami-proxy-dll PRIVATE minhook::minhook)
  target_compile_features(okami-proxy-dll PRIVATE ${STD_FEATURE})
  enable_strict_warnings(okami-proxy-dll)
  apply_release_optimizations(okami-proxy-dll)
  add_git_version_info(okami-proxy-dll)
  add_windows_resources(okami-proxy-dll)
endif()

##################################################
# Test package
##################################################
if(WIN32)
  set(TESTED_UNITS ${client_SOURCES} ${library_SOURCES})
else()
  set(TESTED_UNITS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/client/checks.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/client/logger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/client/loginwindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/client/receive.cpp
  )
endif()

# Tests
add_executable(okami-tests ${tests} ${TESTED_UNITS})
target_compile_definitions(okami-tests PRIVATE ASIO_STANDALONE AP_NO_SCHEMA)

if(BUILD_TESTS_ONLY)
  # Linux-only test build - exclude Windows-specific libraries
  target_link_libraries(okami-tests PRIVATE
    okami-library
    Catch2::Catch2WithMain
    nlohmann_json::nlohmann_json
    asio::asio
    OpenSSL::SSL
    OpenSSL::Crypto
    ZLIB::ZLIB
  )
else()
  # Full Windows build
  target_compile_definitions(okami-tests PRIVATE _WIN32_WINNT=0x0601)
  target_link_libraries(okami-tests PRIVATE
    okami-library
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
    dwmapi
    d3dcompiler
    ws2_32
  )

  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CROSS_COMPILING_WINDOWS)
    # target_link_libraries(okami-tests PRIVATE -static-libgcc -static-libstdc++)
    target_link_libraries(okami-apclient PRIVATE dwmapi d3dcompiler)
    target_compile_definitions(okami-tests PRIVATE
      _WEBSOCKETPP_CPP11_STL_
      _WEBSOCKETPP_CPP11_THREAD_
      _WEBSOCKETPP_NO_BOOST_)
    target_compile_options(okami-tests PRIVATE -Wa,-mbig-obj -g1)
    target_link_options(okami-tests PRIVATE
      -static-libgcc
      -static-libstdc++
      -static
      LINKER:--allow-multiple-definition
      LINKER:--enable-stdcall-fixup)
  endif()
endif()

target_include_directories(okami-tests SYSTEM PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src/client
  ${CMAKE_CURRENT_SOURCE_DIR}/external/apclientpp
  ${CMAKE_CURRENT_SOURCE_DIR}/external/wswrap/include
  ${CMAKE_CURRENT_SOURCE_DIR}/external/websocketpp
  ${CMAKE_CURRENT_BINARY_DIR}/generated)

# Only include imgui directories if not building tests only
if(NOT BUILD_TESTS_ONLY)
  target_include_directories(okami-tests SYSTEM PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/backends)
endif()
target_compile_features(okami-tests PRIVATE ${STD_FEATURE})

include(CTest)
include(Catch)
if(NOT CMAKE_CROSSCOMPILING)
  catch_discover_tests(okami-tests)
endif()

##################################################
# Install
##################################################
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set_property(CACHE CMAKE_INSTALL_PREFIX PROPERTY VALUE "d:/SteamLibrary/steamapps/common/Okami")
endif()

if(DEFINED ENV{OKAMI_PATH})
  set_property(CACHE CMAKE_INSTALL_PREFIX PROPERTY VALUE "$ENV{OKAMI_PATH}")
endif()

if(NOT BUILD_TESTS_ONLY)
  install(TARGETS okami-loader RUNTIME DESTINATION .)
  install(PROGRAMS $<TARGET_FILE:okami-proxy-dll> DESTINATION . RENAME dinput8.dll)
  install(TARGETS okami-apclient RUNTIME DESTINATION mods/apclient)
  install(FILES ${CMAKE_BINARY_DIR}/cacert.pem DESTINATION mods/apclient)
  install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/library/game-data DESTINATION mods/apclient)
endif()
