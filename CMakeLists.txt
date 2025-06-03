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
file(GLOB_RECURSE sources src/*.cpp)
file(GLOB_RECURSE tests test/*.cpp)
file(GLOB_RECURSE include inc/*.h)

add_library (okami-apclient SHARED ${sources} ${include})
add_version_info(TARGET okami-apclient)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET okami-apclient PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.


set(CPACK_MONOLITHIC_INSTALL 1)

include(CPack)
