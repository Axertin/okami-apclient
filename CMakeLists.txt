cmake_minimum_required(VERSION 3.21)

include(cmake/compiler_setup.cmake)

project("apclient" LANGUAGES CXX)

# Add GitVersion to cmake module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(SimpleGitVersion)

if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/cacert.pem)
  file(DOWNLOAD https://curl.se/ca/cacert.pem ${CMAKE_CURRENT_BINARY_DIR}/cacert.pem SHOW_PROGRESS)
endif()

# Cross-compilation detection  
if(CMAKE_CROSSCOMPILING AND CMAKE_SYSTEM_NAME STREQUAL "Windows")
  set(CROSS_COMPILING_WINDOWS TRUE)
  message(STATUS "Cross-compiling for Windows from ${CMAKE_HOST_SYSTEM_NAME}")
else()
  set(CROSS_COMPILING_WINDOWS FALSE)
endif()

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set_property(CACHE CMAKE_INSTALL_PREFIX PROPERTY VALUE "d:/SteamLibrary/steamapps/common/Okami/mods")
endif()

if(DEFINED ENV{OKAMI_PATH})
  set_property(CACHE CMAKE_INSTALL_PREFIX PROPERTY VALUE "$ENV{OKAMI_PATH}")
endif()

set(CXX_STD "23" CACHE STRING "Requested C++ standard")
set(STD_FEATURE "cxx_std_${CXX_STD}")

# Option to build only tests (for native Linux builds)
option(BUILD_TESTS_ONLY "Build only tests, skip mod build" OFF)

# Find Python for code generation
find_package(Python3 COMPONENTS Interpreter REQUIRED)

if(NOT BUILD_TESTS_ONLY)
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

  add_subdirectory(src/okami-apclient) # apclient.dll

  install(FILES ${CMAKE_BINARY_DIR}/cacert.pem DESTINATION apclient/)
endif()

# Always configure tests for CTest
enable_testing()
add_subdirectory(tests)

# Create symlink for compile_commands.json in project root for clangd
if(CMAKE_EXPORT_COMPILE_COMMANDS)
    file(CREATE_LINK
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_SOURCE_DIR}/compile_commands.json
        SYMBOLIC
    )
endif()
