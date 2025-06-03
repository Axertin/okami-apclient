# CMakeList.txt : CMake project for okami-apclient, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.10)

# Add GitVersion to cmake module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(GitVersion)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project("okami-apclient" LANGUAGES CXX)

# Glob tree for sources and includes, and put them in lists
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE tests CONFIGURE_DEPENDS "test/*.cpp")

add_library(okami-apclient SHARED ${sources})
target_include_directories(okami-apclient PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_compile_options(okami-apclient PUBLIC -WX)
add_version_info(TARGET okami-apclient)
set_target_properties(okami-apclient PROPERTIES VERSION ${okami-apclient_VERSION_MAJOR}.${okami-apclient_VERSION_MINOR}.${okami-apclient_VERSION_PATCH})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET okami-apclient PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests

# TODO: Allow override of install_prefix
set(CMAKE_INSTALL_PREFIX "d:/SteamLibrary/steamapps/common/Okami")
install(TARGETS okami-apclient DESTINATION mods)
