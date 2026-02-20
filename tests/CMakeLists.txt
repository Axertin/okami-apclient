# Okami-Apclient Unit Tests
cmake_minimum_required(VERSION 3.21)

# Find Catch2 from vcpkg
find_package(Catch2 CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# Create testable core library with selective mod sources
add_library(apclient-testable STATIC
    # Reward system
    ${CMAKE_SOURCE_DIR}/src/okami-apclient/rewardman.cpp
    ${CMAKE_SOURCE_DIR}/src/okami-apclient/rewards/brushes.cpp
    ${CMAKE_SOURCE_DIR}/src/okami-apclient/rewards/event_flags.cpp
    ${CMAKE_SOURCE_DIR}/src/okami-apclient/rewards/game_items.cpp

    # Check system
    ${CMAKE_SOURCE_DIR}/src/okami-apclient/checkman.cpp
    ${CMAKE_SOURCE_DIR}/src/okami-apclient/checks/containers.cpp
    ${CMAKE_SOURCE_DIR}/src/okami-apclient/checks/gamestate_monitors.cpp
    ${CMAKE_SOURCE_DIR}/src/okami-apclient/checks/shops.cpp

    # Shop data
    ${CMAKE_SOURCE_DIR}/src/okami-apclient/shopdata.cpp

    # Other testable sources
    ${CMAKE_SOURCE_DIR}/src/okami-apclient/gamestate_accessors.cpp
    ${CMAKE_SOURCE_DIR}/src/okami-apclient/slotconfig.cpp

    # Mock implementations
    mocks/wolf_framework.cpp
    mocks/mock_archipelagosocket.cpp
    mocks/mock_notificationwindow.cpp
)

target_include_directories(apclient-testable PUBLIC
    # Mocks MUST come first to override real headers
    ${CMAKE_CURRENT_SOURCE_DIR}/mocks
    ${CMAKE_SOURCE_DIR}/src/okami-apclient
    ${CMAKE_SOURCE_DIR}/include
)

target_compile_features(apclient-testable PRIVATE cxx_std_23)

# Define __fastcall as empty on non-Windows platforms (it's a Windows calling convention)
if(NOT WIN32)
    target_compile_definitions(apclient-testable PRIVATE __fastcall=)
endif()

target_link_libraries(apclient-testable PUBLIC nlohmann_json::nlohmann_json)

# Main test executable - tests production code
add_executable(apclient-tests
    test_reward_handlers.cpp
    test_rewardman.cpp
    test_checkman.cpp
    test_containers.cpp
    test_gamestate_monitors.cpp
    test_shops.cpp
    test_version_utils.cpp
)

target_include_directories(apclient-tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/mocks
    ${CMAKE_SOURCE_DIR}/src/okami-apclient
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(apclient-tests PRIVATE
    apclient-testable
    Catch2::Catch2WithMain
)

target_compile_features(apclient-tests PRIVATE cxx_std_23)

if(NOT WIN32)
    target_compile_definitions(apclient-tests PRIVATE __fastcall=)
endif()

# Separate target for mock/harness tests - tests the test infrastructure itself
# Run less frequently since these don't test production code
add_executable(apclient-harness-tests
    harness/test_mock_archipelagosocket.cpp
)

target_include_directories(apclient-harness-tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/mocks
    ${CMAKE_SOURCE_DIR}/src/okami-apclient
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(apclient-harness-tests PRIVATE
    apclient-testable
    Catch2::Catch2WithMain
)

target_compile_features(apclient-harness-tests PRIVATE cxx_std_23)

if(NOT WIN32)
    target_compile_definitions(apclient-harness-tests PRIVATE __fastcall=)
endif()

# Enable testing
include(CTest)

# Include Catch2's test discovery
include(Catch)

# When cross-compiling (e.g. Windows EXEs built on Linux), running the executable
# at build time for POST_BUILD test discovery will fail. Defer to PRE_TEST so the
# build succeeds; test discovery runs at CTest invocation time instead.
if(CMAKE_CROSSCOMPILING)
    set(CATCH_DISCOVERY_MODE DISCOVERY_MODE PRE_TEST)
else()
    set(CATCH_DISCOVERY_MODE "")
endif()
catch_discover_tests(apclient-tests ${CATCH_DISCOVERY_MODE})
catch_discover_tests(apclient-harness-tests ${CATCH_DISCOVERY_MODE})
