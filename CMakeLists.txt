# CMakeList.txt : CMake project for okami-apclient, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.15)

function(enable_strict_warnings target)
  set_property(GLOBAL PROPERTY ENABLE_STRICT_WARNINGS_DEFINED TRUE)
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(${target} PRIVATE -Wall)

    if(CMAKE_BUILD_TYPE MATCHES "Release|RelWithDebInfo")
      target_compile_options(${target} PRIVATE -Werror)
    endif()

  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${target} PRIVATE /W4)

    if(CMAKE_BUILD_TYPE MATCHES "Release|RelWithDebInfo")
      target_compile_options(${target} PRIVATE /WX)
    endif()

    # Prevent Edit-and-Continue even in Debug
    if(POLICY CMP0141)
      set_property(TARGET ${target} PROPERTY MSVC_DEBUG_INFORMATION_FORMAT ProgramDatabase)
    endif()
  endif()
endfunction()

# Add GitVersion to cmake module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(GitVersion)

if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/cacert.pem)
  file(DOWNLOAD https://curl.se/ca/cacert.pem ${CMAKE_CURRENT_BINARY_DIR}/cacert.pem SHOW_PROGRESS)
endif()

# Enable Hot Reload for MSVC compilers if supported.
if(POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>")
  elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC")
    # Handles clang-cl
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>")
  endif()
endif()

if(POLICY CMP0091)
  cmake_policy(SET CMP0091 NEW)
endif()

project("okami-apclient" LANGUAGES CXX)

# dependencies
find_package(minhook CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(asio CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

add_compile_definitions(ASIO_STANDALONE AP_NO_SCHEMA _WIN32_WINNT=0x0601)

# Glob tree for sources and includes, and put them in lists
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "src/client/*.cpp")
file(GLOB_RECURSE tests CONFIGURE_DEPENDS "test/*.cpp")

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

# Primary program
add_library(okami-apclient SHARED ${sources})
target_include_directories(okami-apclient SYSTEM PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/external/apclientpp
  ${CMAKE_CURRENT_SOURCE_DIR}/external/wswrap/include
  ${CMAKE_CURRENT_SOURCE_DIR}/external/websocketpp
  ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui
  ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/backends)
target_link_libraries(okami-apclient PRIVATE minhook::minhook nlohmann_json::nlohmann_json asio::asio OpenSSL::SSL OpenSSL::Crypto ZLIB::ZLIB imgui d3d11 dxgi ws2_32)
enable_strict_warnings(okami-apclient)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  target_link_libraries(okami-apclient PRIVATE Crypt32)
  target_compile_options(okami-apclient PRIVATE /Zc:__cplusplus)
  set_property(TARGET okami-apclient PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

add_version_info(TARGET okami-apclient)
get_target_property(_version okami-apclient VERSION_STRING)


# Write out version to file (for CI use)
file(WRITE ${CMAKE_BINARY_DIR}/version.txt
  "${_version}")

# Bootstrap loader DLL
add_library(apclient-loader SHARED ${CMAKE_CURRENT_SOURCE_DIR}/src/loader/dllmain.cpp)
target_link_libraries(apclient-loader PRIVATE SHlwapi)
target_include_directories(apclient-loader PRIVATE)

set_target_properties(okami-apclient PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES)

# TODO: Add tests

# TODO: Allow override of install_prefix
set(CMAKE_INSTALL_PREFIX "d:/SteamLibrary/steamapps/common/Okami")
install(TARGETS apclient-loader DESTINATION mods)
install(TARGETS okami-apclient DESTINATION mods/apclient)
install(FILES ${CMAKE_BINARY_DIR}/cacert.pem DESTINATION mods/apclient)
