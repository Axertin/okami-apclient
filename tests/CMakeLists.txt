# Okami-Apclient Unit Tests
cmake_minimum_required(VERSION 3.21)

# Find Catch2 from vcpkg
find_package(Catch2 CONFIG REQUIRED)

# Get generated AP items sources from main build
set(APITEMS_GENERATED_CPP "${CMAKE_SOURCE_DIR}/src/okami-apclient/apitems_generated.cpp")
set(APITEMS_GENERATED_HEADER "${CMAKE_SOURCE_DIR}/src/okami-apclient/apitems_generated.hpp")

# Create testable core library with selective mod sources
add_library(apclient-testable STATIC
    # Generated item definitions
    ${APITEMS_GENERATED_CPP}

    # Testable mod sources (no imgui/minhook/direct game dependencies)
    ${CMAKE_SOURCE_DIR}/src/okami-apclient/gamestate_accessors.cpp
    ${CMAKE_SOURCE_DIR}/src/okami-apclient/item_handlers.cpp
    ${CMAKE_SOURCE_DIR}/src/okami-apclient/aplocationmonitor.cpp

    # Mock implementations
    mocks/wolf_framework.cpp
)

target_include_directories(apclient-testable PUBLIC
    # Mocks MUST come first to override real headers
    ${CMAKE_CURRENT_SOURCE_DIR}/mocks
    ${CMAKE_SOURCE_DIR}/src/okami-apclient
    ${CMAKE_SOURCE_DIR}/include
)

target_compile_features(apclient-testable PRIVATE cxx_std_23)

# Test executable
add_executable(apclient-tests
    test_apitems.cpp
    test_item_handlers.cpp
    test_aplocationmonitor.cpp
)

target_include_directories(apclient-tests PRIVATE
    # Mocks MUST come first to override real headers
    ${CMAKE_CURRENT_SOURCE_DIR}/mocks
    ${CMAKE_SOURCE_DIR}/src/okami-apclient
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(apclient-tests PRIVATE
    apclient-testable
    Catch2::Catch2WithMain
)

target_compile_features(apclient-tests PRIVATE cxx_std_23)

# Enable testing
include(CTest)

# Include Catch2's test discovery
include(Catch)
catch_discover_tests(apclient-tests)
