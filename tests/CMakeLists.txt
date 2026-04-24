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
    ${CMAKE_SOURCE_DIR}/src/okami-apclient/checks/brushes.cpp
    ${CMAKE_SOURCE_DIR}/src/okami-apclient/checks/containers.cpp
    ${CMAKE_SOURCE_DIR}/src/okami-apclient/checks/gamestate_monitors.cpp
    ${CMAKE_SOURCE_DIR}/src/okami-apclient/checks/shops.cpp

    # Shop data
    ${CMAKE_SOURCE_DIR}/src/okami-apclient/data/shopdata.cpp

    # Save system
    ${CMAKE_SOURCE_DIR}/src/okami-apclient/saveman.cpp

    # Other testable sources
    ${CMAKE_SOURCE_DIR}/src/okami-apclient/gamestate_accessors.cpp
    ${CMAKE_SOURCE_DIR}/src/okami-apclient/slotconfig.cpp
    ${CMAKE_SOURCE_DIR}/src/okami-apclient/itempatch.cpp
    ${CMAKE_SOURCE_DIR}/src/okami-apclient/data/msd.cpp
    ${CMAKE_SOURCE_DIR}/src/okami-apclient/data/blowfish.cpp
    ${CMAKE_SOURCE_DIR}/src/okami-apclient/data/resourcepkg.cpp
    ${CMAKE_SOURCE_DIR}/src/okami-apclient/data/customiconpkg.cpp
    ${CMAKE_SOURCE_DIR}/src/okami-apclient/data/custommodelpkg.cpp

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
    test_itempatch.cpp
    test_resourcepkg.cpp
    test_customiconpkg.cpp
    test_saveman.cpp
)

target_include_directories(apclient-tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/mocks
    ${CMAKE_SOURCE_DIR}/src/okami-apclient
    ${CMAKE_SOURCE_DIR}/include
)

target_compile_definitions(apclient-tests PRIVATE
    TEST_FIXTURES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/fixtures"
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

# When cross-compiling with llvm-mingw, statically link libc++/libunwind so the test
# EXEs have no dependency on libc++.dll or libunwind.dll
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CROSS_COMPILING_WINDOWS)
    target_link_libraries(apclient-tests PRIVATE -l:libc++.a -l:libc++abi.a -l:libunwind.a)
    target_link_libraries(apclient-harness-tests PRIVATE -l:libc++.a -l:libc++abi.a -l:libunwind.a)
endif()

# Enable testing
include(CTest)

# Include Catch2's test discovery
include(Catch)

# When cross-compiling for Windows, use Wine to run test executables if available.
# If Wine is not found, skip test discovery entirely (tests still build, just aren't registered with CTest).
if(CMAKE_CROSSCOMPILING AND CMAKE_SYSTEM_NAME STREQUAL "Windows")
    find_program(WINE_EXECUTABLE NAMES wine64 wine)
    if(WINE_EXECUTABLE)
        # Wine needs an X display to initialize its prefix, even for console-only executables.
        if(DEFINED ENV{DISPLAY})
            # A display is already available; use Wine directly.
            set(_wine_emulator "${WINE_EXECUTABLE}")
            message(STATUS "Wine found: ${WINE_EXECUTABLE} (DISPLAY=$ENV{DISPLAY}), test discovery deferred to ctest invocation")
        else()
            # No display in environment; wrap with xvfb-run if available, else warn and use directly
            find_program(XVFB_RUN xvfb-run)
            if(XVFB_RUN)
                set(_wine_emulator "${XVFB_RUN};-a;${WINE_EXECUTABLE}")
                message(STATUS "Wine found: ${WINE_EXECUTABLE} (via xvfb-run), test discovery deferred to ctest invocation")
            else()
                set(_wine_emulator "${WINE_EXECUTABLE}")
                message(WARNING "Wine found: ${WINE_EXECUTABLE} but $DISPLAY is not set and xvfb could not be found, expect some errors.")
                message(STATUS "test discovery deferred to ctest invocation")
            endif()
        endif()
        set_target_properties(apclient-tests apclient-harness-tests
            PROPERTIES CROSSCOMPILING_EMULATOR "${_wine_emulator}") # But CMake, WINE Is Not an Emulator!
        catch_discover_tests(apclient-tests DISCOVERY_MODE PRE_TEST)
        catch_discover_tests(apclient-harness-tests DISCOVERY_MODE PRE_TEST)
    else()
        message(STATUS "Wine not found, skipping CTest discovery for cross-compiled Windows tests")
    endif()
else()
    catch_discover_tests(apclient-tests)
    catch_discover_tests(apclient-harness-tests)
endif()
