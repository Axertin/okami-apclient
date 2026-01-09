#include <catch2/catch_test_macros.hpp>

#include "checkman.h"
#include "checks/check_types.hpp"
#include "mock_archipelagosocket.h"

// ============================================================================
// Check ID category boundaries (compile-time verified, runtime tests boundaries only)
// ============================================================================

// Compile-time verification that formulas work - no runtime tests needed for arithmetic
static_assert(checks::getItemCheckId(0) == checks::kItemPickupBase);
static_assert(checks::getBrushCheckId(0) == checks::kBrushAcquisitionBase);
static_assert(checks::getShopCheckId(0, 0) == checks::kShopPurchaseBase);
static_assert(checks::getWorldStateCheckId(0, 0) == checks::kWorldStateBase);
static_assert(checks::getCollectedObjectCheckId(0, 0) == checks::kCollectedObjectBase);
static_assert(checks::getAreaRestoredCheckId(0, 0) == checks::kAreaRestoredBase);
static_assert(checks::getGlobalFlagCheckId(0) == checks::kGlobalFlagBase);
static_assert(checks::getGameProgressCheckId(0) == checks::kGameProgressBase);
static_assert(checks::getContainerCheckId(0, 0) == checks::kContainerBase);

TEST_CASE("Check category detection at boundaries", "[checkman][check_types]")
{
    // Test boundary transitions where category changes - these catch off-by-one errors
    SECTION("Category transitions")
    {
        // Just below each base -> previous category (or Unknown for first)
        CHECK(checks::getCheckCategory(checks::kItemPickupBase - 1) == checks::CheckCategory::Unknown);
        CHECK(checks::getCheckCategory(checks::kBrushAcquisitionBase - 1) == checks::CheckCategory::ItemPickup);
        CHECK(checks::getCheckCategory(checks::kShopPurchaseBase - 1) == checks::CheckCategory::BrushAcquisition);
        CHECK(checks::getCheckCategory(checks::kWorldStateBase - 1) == checks::CheckCategory::ShopPurchase);
        CHECK(checks::getCheckCategory(checks::kCollectedObjectBase - 1) == checks::CheckCategory::WorldState);
        CHECK(checks::getCheckCategory(checks::kAreaRestoredBase - 1) == checks::CheckCategory::CollectedObject);
        CHECK(checks::getCheckCategory(checks::kGlobalFlagBase - 1) == checks::CheckCategory::AreaRestored);
        CHECK(checks::getCheckCategory(checks::kGameProgressBase - 1) == checks::CheckCategory::GlobalFlag);
        CHECK(checks::getCheckCategory(checks::kContainerBase - 1) == checks::CheckCategory::GameProgress);

        // At each base -> correct category
        CHECK(checks::getCheckCategory(checks::kItemPickupBase) == checks::CheckCategory::ItemPickup);
        CHECK(checks::getCheckCategory(checks::kBrushAcquisitionBase) == checks::CheckCategory::BrushAcquisition);
        CHECK(checks::getCheckCategory(checks::kShopPurchaseBase) == checks::CheckCategory::ShopPurchase);
        CHECK(checks::getCheckCategory(checks::kWorldStateBase) == checks::CheckCategory::WorldState);
        CHECK(checks::getCheckCategory(checks::kCollectedObjectBase) == checks::CheckCategory::CollectedObject);
        CHECK(checks::getCheckCategory(checks::kAreaRestoredBase) == checks::CheckCategory::AreaRestored);
        CHECK(checks::getCheckCategory(checks::kGlobalFlagBase) == checks::CheckCategory::GlobalFlag);
        CHECK(checks::getCheckCategory(checks::kGameProgressBase) == checks::CheckCategory::GameProgress);
        CHECK(checks::getCheckCategory(checks::kContainerBase) == checks::CheckCategory::Container);
    }
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
// Connection and sending state tests
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

    SECTION("No send when sending disabled")
    {
        socket.setConnected(true);
        checkMan.enableSending(false);
        socket.clearSentLocations();
        checkMan.onBrushAcquired(10);
        CHECK(socket.getSentLocationCount() == 0);
    }
}

// ============================================================================
// Event handler integration tests
// ============================================================================

TEST_CASE("Event handlers send to socket", "[checkman]")
{
    mock::MockArchipelagoSocket socket;
    socket.setConnected(true);
    CheckMan checkMan(socket);
    checkMan.enableSending(true);

    SECTION("onItemPickup sends check")
    {
        socket.clearSentLocations();
        checkMan.onItemPickup(0x42, 1);
        CHECK(socket.getSentLocationCount() == 1);
        CHECK(socket.wasLocationSent(checks::getItemCheckId(0x42)));
    }

    SECTION("onBrushAcquired sends check")
    {
        socket.clearSentLocations();
        checkMan.onBrushAcquired(5);
        CHECK(socket.getSentLocationCount() == 1);
        CHECK(socket.wasLocationSent(checks::getBrushCheckId(5)));
    }

    SECTION("onShopPurchase sends check")
    {
        socket.clearSentLocations();
        checkMan.onShopPurchase(2, 3, 0x50);
        CHECK(socket.getSentLocationCount() == 1);
        CHECK(socket.wasLocationSent(checks::getShopCheckId(2, 3)));
    }
}
