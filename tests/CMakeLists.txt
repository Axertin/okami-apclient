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

    # Mock implementations
    mocks/wolf_framework.cpp
    mocks/mock_archipelagosocket.cpp
)

target_include_directories(apclient-testable PUBLIC
    # Mocks MUST come first to override real headers
    ${CMAKE_CURRENT_SOURCE_DIR}/mocks
    ${CMAKE_SOURCE_DIR}/src/okami-apclient
    ${CMAKE_SOURCE_DIR}/include
)

target_compile_features(apclient-testable PRIVATE cxx_std_23)

target_link_libraries(apclient-testable PUBLIC nlohmann_json::nlohmann_json)

# Test executable
add_executable(apclient-tests
    # Unit tests for new architecture
    test_reward_handlers.cpp
    test_rewardman.cpp
    test_checkman.cpp
    test_containers.cpp
    test_gamestate_monitors.cpp
    test_shops.cpp

    # Harness/mock tests (tests for the test infrastructure itself)
    harness/test_mock_archipelagosocket.cpp
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
