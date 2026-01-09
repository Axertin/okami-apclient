#include <catch2/catch_test_macros.hpp>

#include "checkman.h"
#include "checks/check_types.hpp"
#include "mock_archipelagosocket.h"

// ============================================================================
// Check ID calculation tests
// ============================================================================

TEST_CASE("Check ID calculations", "[checkman][check_types]")
{
    SECTION("getItemCheckId formula")
    {
        CHECK(checks::getItemCheckId(0) == 100000);
        CHECK(checks::getItemCheckId(1) == 100001);
        CHECK(checks::getItemCheckId(42) == 100042);
        CHECK(checks::getItemCheckId(255) == 100255);
    }

    SECTION("getBrushCheckId formula")
    {
        CHECK(checks::getBrushCheckId(0) == 200000);
        CHECK(checks::getBrushCheckId(1) == 200001);
        CHECK(checks::getBrushCheckId(13) == 200013);
    }

    SECTION("getShopCheckId formula")
    {
        CHECK(checks::getShopCheckId(0, 0) == 300000);
        CHECK(checks::getShopCheckId(0, 1) == 300001);
        CHECK(checks::getShopCheckId(1, 0) == 301000);
        CHECK(checks::getShopCheckId(5, 10) == 305010);
        CHECK(checks::getShopCheckId(42, 99) == 342099);
    }

    SECTION("getWorldStateCheckId formula")
    {
        CHECK(checks::getWorldStateCheckId(0, 0) == 400000);
        CHECK(checks::getWorldStateCheckId(0, 1) == 400001);
        CHECK(checks::getWorldStateCheckId(1, 0) == 410000);
        CHECK(checks::getWorldStateCheckId(5, 100) == 450100);
    }

    SECTION("getCollectedObjectCheckId formula")
    {
        CHECK(checks::getCollectedObjectCheckId(0, 0) == 500000);
        CHECK(checks::getCollectedObjectCheckId(0, 1) == 500001);
        CHECK(checks::getCollectedObjectCheckId(1, 0) == 510000);
        CHECK(checks::getCollectedObjectCheckId(5, 50) == 550050);
    }

    SECTION("getAreaRestoredCheckId formula")
    {
        CHECK(checks::getAreaRestoredCheckId(0, 0) == 600000);
        CHECK(checks::getAreaRestoredCheckId(0, 1) == 600001);
        CHECK(checks::getAreaRestoredCheckId(1, 0) == 610000);
        CHECK(checks::getAreaRestoredCheckId(5, 50) == 650050);
    }

    SECTION("getGlobalFlagCheckId formula")
    {
        CHECK(checks::getGlobalFlagCheckId(0) == 700000);
        CHECK(checks::getGlobalFlagCheckId(1) == 700001);
        CHECK(checks::getGlobalFlagCheckId(42) == 700042);
        CHECK(checks::getGlobalFlagCheckId(85) == 700085);
    }

    SECTION("getGameProgressCheckId formula")
    {
        CHECK(checks::getGameProgressCheckId(0) == 800000);
        CHECK(checks::getGameProgressCheckId(1) == 800001);
        CHECK(checks::getGameProgressCheckId(42) == 800042);
        CHECK(checks::getGameProgressCheckId(95) == 800095);
    }

    SECTION("getContainerCheckId formula")
    {
        // BASE + (levelId << 8) + spawnIdx
        CHECK(checks::getContainerCheckId(0, 0) == 900000);
        CHECK(checks::getContainerCheckId(6, 0) == 901536);
        CHECK(checks::getContainerCheckId(6, 5) == 901541);
    }

    SECTION("Check ID formulas are constexpr")
    {
        static_assert(checks::getItemCheckId(42) == 100042);
        static_assert(checks::getBrushCheckId(5) == 200005);
        static_assert(checks::getContainerCheckId(6, 5) == 901541);
    }
}

TEST_CASE("Check category detection", "[checkman][check_types]")
{
    CHECK(checks::getCheckCategory(100042) == checks::CheckCategory::ItemPickup);
    CHECK(checks::getCheckCategory(200005) == checks::CheckCategory::BrushAcquisition);
    CHECK(checks::getCheckCategory(305010) == checks::CheckCategory::ShopPurchase);
    CHECK(checks::getCheckCategory(410005) == checks::CheckCategory::WorldState);
    CHECK(checks::getCheckCategory(510050) == checks::CheckCategory::CollectedObject);
    CHECK(checks::getCheckCategory(610050) == checks::CheckCategory::AreaRestored);
    CHECK(checks::getCheckCategory(700042) == checks::CheckCategory::GlobalFlag);
    CHECK(checks::getCheckCategory(800042) == checks::CheckCategory::GameProgress);
    CHECK(checks::getCheckCategory(901536) == checks::CheckCategory::Container);
}

// ============================================================================
// CheckMan construction and basic state tests
// ============================================================================

TEST_CASE("CheckMan sending disabled by default", "[checkman]")
{
    mock::MockArchipelagoSocket socket;
    CheckMan checkMan(socket);
    REQUIRE_FALSE(checkMan.isSendingEnabled());
}

TEST_CASE("CheckMan sending can be enabled", "[checkman]")
{
    mock::MockArchipelagoSocket socket;
    CheckMan checkMan(socket);
    checkMan.enableSending(true);
    REQUIRE(checkMan.isSendingEnabled());
}

TEST_CASE("CheckMan starts with zero sent count", "[checkman]")
{
    mock::MockArchipelagoSocket socket;
    CheckMan checkMan(socket);
    REQUIRE(checkMan.getSentCount() == 0);
}

// ============================================================================
// Duplicate tracking tests
// ============================================================================

TEST_CASE("Duplicate tracking", "[checkman]")
{
    mock::MockArchipelagoSocket socket;
    socket.setConnected(true);
    CheckMan checkMan(socket);
    checkMan.enableSending(true);

    SECTION("First send succeeds")
    {
        socket.clearSentLocations();
        checkMan.onBrushAcquired(1);
        CHECK(socket.getSentLocationCount() == 1);
        CHECK(socket.wasLocationSent(checks::getBrushCheckId(1)));
    }

    SECTION("Second identical send is blocked")
    {
        socket.clearSentLocations();
        checkMan.onBrushAcquired(1);
        checkMan.onBrushAcquired(1); // Same brush again
        CHECK(socket.getSentLocationCount() == 1);
    }

    SECTION("Reset clears cache, allows resend")
    {
        socket.clearSentLocations();
        checkMan.onBrushAcquired(1);
        CHECK(socket.getSentLocationCount() == 1);

        checkMan.reset();
        socket.clearSentLocations();
        checkMan.onBrushAcquired(1);
        CHECK(socket.getSentLocationCount() == 1);
    }

    SECTION("Different checks tracked separately")
    {
        socket.clearSentLocations();
        checkMan.onBrushAcquired(1);
        checkMan.onBrushAcquired(2);
        checkMan.onBrushAcquired(3);
        CHECK(socket.getSentLocationCount() == 3);
    }
}

// ============================================================================
// Connection requirements tests
// ============================================================================

TEST_CASE("Connection requirements", "[checkman]")
{
    mock::MockArchipelagoSocket socket;
    CheckMan checkMan(socket);
    checkMan.enableSending(true);

    SECTION("No send when socket disconnected")
    {
        socket.setConnected(false);
        socket.clearSentLocations();
        checkMan.onBrushAcquired(10);
        CHECK(socket.getSentLocationCount() == 0);
    }

    SECTION("Send when socket connected")
    {
        socket.setConnected(true);
        socket.clearSentLocations();
        checkMan.onBrushAcquired(10);
        CHECK(socket.getSentLocationCount() == 1);
    }
}

// ============================================================================
// Event handler tests
// ============================================================================

TEST_CASE("Event handlers", "[checkman]")
{
    mock::MockArchipelagoSocket socket;
    socket.setConnected(true);
    CheckMan checkMan(socket);
    checkMan.enableSending(true);

    SECTION("onItemPickup sends correct check ID")
    {
        socket.clearSentLocations();
        checkMan.onItemPickup(0x42, 1);
        CHECK(socket.getSentLocationCount() == 1);
        CHECK(socket.wasLocationSent(checks::getItemCheckId(0x42)));
    }

    SECTION("onBrushAcquired sends correct check ID")
    {
        socket.clearSentLocations();
        checkMan.onBrushAcquired(5);
        CHECK(socket.getSentLocationCount() == 1);
        CHECK(socket.wasLocationSent(checks::getBrushCheckId(5)));
    }

    SECTION("onShopPurchase sends correct check ID")
    {
        socket.clearSentLocations();
        checkMan.onShopPurchase(2, 3, 0x50);
        CHECK(socket.getSentLocationCount() == 1);
        CHECK(socket.wasLocationSent(checks::getShopCheckId(2, 3)));
    }

    SECTION("Handlers respect sendingEnabled flag")
    {
        checkMan.enableSending(false);
        socket.clearSentLocations();
        checkMan.onBrushAcquired(1);
        checkMan.onItemPickup(0x42, 1);
        checkMan.onShopPurchase(0, 0, 0x10);
        CHECK(socket.getSentLocationCount() == 0);
    }
}
