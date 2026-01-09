#include <catch2/catch_test_macros.hpp>

#include "checks/check_types.hpp"
#include "checks/gamestate_monitors.hpp"
#include "wolf_framework.hpp"

// ============================================================================
// GameProgress monitor tests
// ============================================================================

TEST_CASE("GameProgress monitor creation", "[gamestate_monitors]")
{
    wolf::mock::reset();

    std::vector<int64_t> receivedChecks;
    auto callback = [&](int64_t checkId) { receivedChecks.push_back(checkId); };

    auto handle = checks::createGameProgressMonitor(callback);
    REQUIRE(handle != nullptr);

    wolf::destroyBitfieldMonitor(handle);
    wolf::mock::reset();
}

TEST_CASE("GameProgress monitor triggers callback on bit change 0->1", "[gamestate_monitors]")
{
    wolf::mock::reset();

    std::vector<int64_t> receivedChecks;
    auto callback = [&](int64_t checkId) { receivedChecks.push_back(checkId); };

    auto handle = checks::createGameProgressMonitor(callback);
    REQUIRE(handle != nullptr);

    SECTION("Bit 0 triggers correct check ID")
    {
        wolf::mock::triggerBitfieldChange(handle, 0, false, true);
        REQUIRE(receivedChecks.size() == 1);
        CHECK(receivedChecks[0] == checks::getGameProgressCheckId(0));
    }

    SECTION("Bit 42 triggers correct check ID")
    {
        wolf::mock::triggerBitfieldChange(handle, 42, false, true);
        REQUIRE(receivedChecks.size() == 1);
        CHECK(receivedChecks[0] == checks::getGameProgressCheckId(42));
    }

    SECTION("Bit change 1->0 does NOT trigger callback")
    {
        wolf::mock::triggerBitfieldChange(handle, 5, true, false);
        CHECK(receivedChecks.empty());
    }

    SECTION("Bit change 1->1 does NOT trigger callback")
    {
        wolf::mock::triggerBitfieldChange(handle, 5, true, true);
        CHECK(receivedChecks.empty());
    }

    wolf::destroyBitfieldMonitor(handle);
    wolf::mock::reset();
}

// ============================================================================
// GlobalFlags monitor tests
// ============================================================================

TEST_CASE("GlobalFlags monitor creation", "[gamestate_monitors]")
{
    wolf::mock::reset();

    std::vector<int64_t> receivedChecks;
    auto callback = [&](int64_t checkId) { receivedChecks.push_back(checkId); };

    auto handle = checks::createGlobalFlagsMonitor(callback);
    REQUIRE(handle != nullptr);

    wolf::destroyBitfieldMonitor(handle);
    wolf::mock::reset();
}

TEST_CASE("GlobalFlags monitor triggers callback on bit change 0->1", "[gamestate_monitors]")
{
    wolf::mock::reset();

    std::vector<int64_t> receivedChecks;
    auto callback = [&](int64_t checkId) { receivedChecks.push_back(checkId); };

    auto handle = checks::createGlobalFlagsMonitor(callback);
    REQUIRE(handle != nullptr);

    SECTION("Bit 0 triggers correct check ID")
    {
        wolf::mock::triggerBitfieldChange(handle, 0, false, true);
        REQUIRE(receivedChecks.size() == 1);
        CHECK(receivedChecks[0] == checks::getGlobalFlagCheckId(0));
    }

    SECTION("Bit 42 triggers correct check ID")
    {
        wolf::mock::triggerBitfieldChange(handle, 42, false, true);
        REQUIRE(receivedChecks.size() == 1);
        CHECK(receivedChecks[0] == checks::getGlobalFlagCheckId(42));
    }

    SECTION("Bit change 1->0 does NOT trigger callback")
    {
        wolf::mock::triggerBitfieldChange(handle, 5, true, false);
        CHECK(receivedChecks.empty());
    }

    wolf::destroyBitfieldMonitor(handle);
    wolf::mock::reset();
}

// ============================================================================
// WorldState monitors tests
// ============================================================================

TEST_CASE("WorldState monitors creation", "[gamestate_monitors]")
{
    wolf::mock::reset();

    std::vector<int64_t> receivedChecks;
    auto callback = [&](int64_t checkId) { receivedChecks.push_back(checkId); };

    auto handles = checks::createWorldStateMonitors(callback);
    REQUIRE_FALSE(handles.empty());

    // All handles should be non-null
    for (auto handle : handles)
    {
        REQUIRE(handle != nullptr);
    }

    for (auto handle : handles)
    {
        wolf::destroyBitfieldMonitor(handle);
    }
    wolf::mock::reset();
}

TEST_CASE("WorldState monitor triggers callback with correct mapId", "[gamestate_monitors]")
{
    wolf::mock::reset();

    std::vector<int64_t> receivedChecks;
    auto callback = [&](int64_t checkId) { receivedChecks.push_back(checkId); };

    auto handles = checks::createWorldStateMonitors(callback);
    REQUIRE(handles.size() > 5);

    SECTION("Map 0, bit 10 triggers correct check ID")
    {
        wolf::mock::triggerBitfieldChange(handles[0], 10, false, true);
        REQUIRE(receivedChecks.size() == 1);
        CHECK(receivedChecks[0] == checks::getWorldStateCheckId(0, 10));
    }

    SECTION("Map 5, bit 42 triggers correct check ID")
    {
        wolf::mock::triggerBitfieldChange(handles[5], 42, false, true);
        REQUIRE(receivedChecks.size() == 1);
        CHECK(receivedChecks[0] == checks::getWorldStateCheckId(5, 42));
    }

    SECTION("Bit change 1->0 does NOT trigger callback")
    {
        wolf::mock::triggerBitfieldChange(handles[0], 5, true, false);
        CHECK(receivedChecks.empty());
    }

    for (auto handle : handles)
    {
        wolf::destroyBitfieldMonitor(handle);
    }
    wolf::mock::reset();
}

// ============================================================================
// CollectedObject monitors tests
// ============================================================================

TEST_CASE("CollectedObject monitors creation", "[gamestate_monitors]")
{
    wolf::mock::reset();

    std::vector<int64_t> receivedChecks;
    auto callback = [&](int64_t checkId) { receivedChecks.push_back(checkId); };

    auto handles = checks::createCollectedObjectMonitors(callback);
    REQUIRE_FALSE(handles.empty());

    for (auto handle : handles)
    {
        REQUIRE(handle != nullptr);
    }

    for (auto handle : handles)
    {
        wolf::destroyBitfieldMonitor(handle);
    }
    wolf::mock::reset();
}

TEST_CASE("CollectedObject monitor triggers callback with correct mapId", "[gamestate_monitors]")
{
    wolf::mock::reset();

    std::vector<int64_t> receivedChecks;
    auto callback = [&](int64_t checkId) { receivedChecks.push_back(checkId); };

    auto handles = checks::createCollectedObjectMonitors(callback);
    REQUIRE(handles.size() > 5);

    SECTION("Map 0, bit 10 triggers correct check ID")
    {
        wolf::mock::triggerBitfieldChange(handles[0], 10, false, true);
        REQUIRE(receivedChecks.size() == 1);
        CHECK(receivedChecks[0] == checks::getCollectedObjectCheckId(0, 10));
    }

    SECTION("Map 5, bit 42 triggers correct check ID")
    {
        wolf::mock::triggerBitfieldChange(handles[5], 42, false, true);
        REQUIRE(receivedChecks.size() == 1);
        CHECK(receivedChecks[0] == checks::getCollectedObjectCheckId(5, 42));
    }

    for (auto handle : handles)
    {
        wolf::destroyBitfieldMonitor(handle);
    }
    wolf::mock::reset();
}

// ============================================================================
// AreaRestored monitors tests
// ============================================================================

TEST_CASE("AreaRestored monitors creation", "[gamestate_monitors]")
{
    wolf::mock::reset();

    std::vector<int64_t> receivedChecks;
    auto callback = [&](int64_t checkId) { receivedChecks.push_back(checkId); };

    auto handles = checks::createAreaRestoredMonitors(callback);
    REQUIRE_FALSE(handles.empty());

    for (auto handle : handles)
    {
        REQUIRE(handle != nullptr);
    }

    for (auto handle : handles)
    {
        wolf::destroyBitfieldMonitor(handle);
    }
    wolf::mock::reset();
}

TEST_CASE("AreaRestored monitor triggers callback with correct mapId", "[gamestate_monitors]")
{
    wolf::mock::reset();

    std::vector<int64_t> receivedChecks;
    auto callback = [&](int64_t checkId) { receivedChecks.push_back(checkId); };

    auto handles = checks::createAreaRestoredMonitors(callback);
    REQUIRE(handles.size() > 5);

    SECTION("Map 0, bit 10 triggers correct check ID")
    {
        wolf::mock::triggerBitfieldChange(handles[0], 10, false, true);
        REQUIRE(receivedChecks.size() == 1);
        CHECK(receivedChecks[0] == checks::getAreaRestoredCheckId(0, 10));
    }

    SECTION("Map 5, bit 42 triggers correct check ID")
    {
        wolf::mock::triggerBitfieldChange(handles[5], 42, false, true);
        REQUIRE(receivedChecks.size() == 1);
        CHECK(receivedChecks[0] == checks::getAreaRestoredCheckId(5, 42));
    }

    for (auto handle : handles)
    {
        wolf::destroyBitfieldMonitor(handle);
    }
    wolf::mock::reset();
}
