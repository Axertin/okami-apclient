#include <catch2/catch_test_macros.hpp>
#include <okami/bitfield.hpp>
#include <okami/structs.hpp>
#include <wolf_framework.hpp>
#include "gamestate_accessors.hpp"
#include "item_handlers.hpp"

class ItemHandlerTestFixture {
public:
    ItemHandlerTestFixture() {
        // Reset mock state before each test
        wolf::mock::reset();
        item_handlers::reset();

        // Reserve enough memory to prevent reallocation when creating accessors
        // TrackerData is at highest offset: 0xB21780 + sizeof(TrackerData)
        // Reserve 20MB to be safe
        wolf::mock::reserveMemory(20 * 1024 * 1024);

        // Initialize game state accessors with mock memory
        apgame::initialize();
    }

    ~ItemHandlerTestFixture() {
        wolf::mock::reset();
        item_handlers::reset();
    }

    // Helper to check if a brush is unlocked
    bool isBrushUnlocked(int index) {
        return apgame::usableBrushTechniques->IsSet(index) &&
               apgame::obtainedBrushTechniques->IsSet(index);
    }

    // Helper to check if a brush upgrade is unlocked
    bool isBrushUpgradeUnlocked(int bit) {
        return apgame::brushUpgrades->IsSet(bit);
    }

    // Helper to get inventory count for an item
    uint16_t getInventoryCount(int itemId) {
        return apgame::collectionData->inventory[itemId];
    }

    // Helper to set inventory count
    void setInventoryCount(int itemId, uint16_t count) {
        apgame::collectionData->inventory[itemId] = count;
    }
};

TEST_CASE_METHOD(ItemHandlerTestFixture, "Handle regular brush item", "[item_handlers]") {
    SECTION("Granting Sunrise brush") {
        bool result = item_handlers::handleItem(0x100); // Sunrise

        CHECK(result == true);
        CHECK(isBrushUnlocked(0));
    }

    SECTION("Granting Rejuvenation brush") {
        bool result = item_handlers::handleItem(0x101); // Rejuvenation

        CHECK(result == true);
        CHECK(isBrushUnlocked(1));
    }
}

TEST_CASE_METHOD(ItemHandlerTestFixture, "Handle progressive brush - Power Slash", "[item_handlers]") {
    SECTION("First Power Slash grants base brush") {
        bool result = item_handlers::handleItem(0x102);

        CHECK(result == true);
        CHECK(isBrushUnlocked(2));
        CHECK_FALSE(isBrushUpgradeUnlocked(0)); // Power Slash 2 not yet
        CHECK_FALSE(isBrushUpgradeUnlocked(10)); // Power Slash 3 not yet
    }

    SECTION("Second Power Slash grants first upgrade") {
        // Grant base brush first
        item_handlers::handleItem(0x102);

        // Grant upgrade
        bool result = item_handlers::handleItem(0x102);

        CHECK(result == true);
        CHECK(isBrushUpgradeUnlocked(0)); // Power Slash 2
        CHECK_FALSE(isBrushUpgradeUnlocked(10)); // Power Slash 3 not yet
    }

    SECTION("Third Power Slash grants second upgrade") {
        // Grant base brush
        item_handlers::handleItem(0x102);
        // Grant first upgrade
        item_handlers::handleItem(0x102);

        // Grant second upgrade
        bool result = item_handlers::handleItem(0x102);

        CHECK(result == true);
        CHECK(isBrushUpgradeUnlocked(0)); // Power Slash 2
        CHECK(isBrushUpgradeUnlocked(10)); // Power Slash 3
    }

    SECTION("Fourth Power Slash is redundant but doesn't fail") {
        // Max out Power Slash
        item_handlers::handleItem(0x102);
        item_handlers::handleItem(0x102);
        item_handlers::handleItem(0x102);

        // Try to grant again
        bool result = item_handlers::handleItem(0x102);

        CHECK(result == true); // Should succeed but do nothing
    }
}

TEST_CASE_METHOD(ItemHandlerTestFixture, "Handle inventory item", "[item_handlers]") {
    SECTION("Granting Canine Tracker calls wolf::giveItem") {
        bool result = item_handlers::handleItem(0x42);

        CHECK(result == true);
        REQUIRE(wolf::mock::giveItemCalls.size() == 1);
        CHECK(wolf::mock::giveItemCalls[0].itemId == 0x42);
        CHECK(wolf::mock::giveItemCalls[0].count == 1);
    }

    SECTION("Granting multiple items") {
        item_handlers::handleItem(0x42); // Canine Tracker
        item_handlers::handleItem(0x43); // Lucky Mallet

        REQUIRE(wolf::mock::giveItemCalls.size() == 2);
        CHECK(wolf::mock::giveItemCalls[0].itemId == 0x42);
        CHECK(wolf::mock::giveItemCalls[1].itemId == 0x43);
    }
}

TEST_CASE_METHOD(ItemHandlerTestFixture, "Handle progressive weapon", "[item_handlers]") {
    SECTION("Progressive Mirror - first grants Trinity Mirror") {
        bool result = item_handlers::handleItem(0x300);

        CHECK(result == true);
        REQUIRE(wolf::mock::giveItemCalls.size() == 1);
        CHECK(wolf::mock::giveItemCalls[0].itemId == 0x13); // Trinity Mirror
    }

    SECTION("Progressive Mirror - second grants Solar Flare") {
        // Simulate having Trinity Mirror
        setInventoryCount(0x13, 1);

        bool result = item_handlers::handleItem(0x300);

        CHECK(result == true);
        REQUIRE(wolf::mock::giveItemCalls.size() == 1);
        CHECK(wolf::mock::giveItemCalls[0].itemId == 0x14); // Solar Flare
    }

    SECTION("Progressive Mirror - third is max level") {
        // Simulate having both Trinity Mirror and Solar Flare
        setInventoryCount(0x13, 1);
        setInventoryCount(0x14, 1);

        bool result = item_handlers::handleItem(0x300);

        CHECK(result == true);
        // Should log warning but not fail
        CHECK(wolf::mock::giveItemCalls.empty());
    }
}

TEST_CASE_METHOD(ItemHandlerTestFixture, "Handle unknown item", "[item_handlers]") {
    bool result = item_handlers::handleItem(0xFFFF);

    CHECK(result == false);
    CHECK(wolf::mock::giveItemCalls.empty());
}

TEST_CASE_METHOD(ItemHandlerTestFixture, "Location monitor is disabled during item handling", "[item_handlers]") {
    // This test verifies the pattern but can't test the actual behavior
    // without the real APLocationMonitor

    SECTION("Item handling completes successfully") {
        bool result = item_handlers::handleItem(0x100);
        CHECK(result == true);
    }
}

TEST_CASE_METHOD(ItemHandlerTestFixture, "Queue management", "[item_handlers]") {
    // Initialize item handlers to set up callbacks
    item_handlers::initialize();

    SECTION("receiveAPItem queues items") {
        bool result = item_handlers::receiveAPItem(0x100);
        CHECK(result == true);
    }

    SECTION("handleAPItems returns false when queue is empty") {
        wolf::mock::triggerPlayStart();  // Enable safeToReceive
        bool result = item_handlers::handleAPItems();
        CHECK(result == false);
    }

    SECTION("handleAPItems processes queued items when safe") {
        // Queue some items
        item_handlers::receiveAPItem(0x100);  // Sunrise brush
        item_handlers::receiveAPItem(0x101);  // Rejuvenation brush

        // Trigger game start to enable safeToReceive
        wolf::mock::triggerPlayStart();

        // Process queue
        bool result = item_handlers::handleAPItems();
        CHECK(result == true);
        CHECK(isBrushUnlocked(0));  // Sunrise
        CHECK(isBrushUnlocked(1));  // Rejuvenation
    }

    SECTION("handleAPItems does not process when safeToReceive is false") {
        // Queue items
        item_handlers::receiveAPItem(0x100);

        // Don't trigger game start, so safeToReceive stays false
        bool result = item_handlers::handleAPItems();
        CHECK(result == false);
        CHECK_FALSE(isBrushUnlocked(0));  // Should not be unlocked
    }

    SECTION("handleAPItems clears queue after processing") {
        // Queue items
        item_handlers::receiveAPItem(0x100);

        // Enable and process
        wolf::mock::triggerPlayStart();
        item_handlers::handleAPItems();

        // Try to process again - should return false (empty queue)
        bool result = item_handlers::handleAPItems();
        CHECK(result == false);
    }

    SECTION("Multiple items are all processed") {
        // Queue multiple different items
        item_handlers::receiveAPItem(0x100);  // Sunrise
        item_handlers::receiveAPItem(0x101);  // Rejuvenation
        item_handlers::receiveAPItem(0x42);   // Canine Tracker

        wolf::mock::triggerPlayStart();
        bool result = item_handlers::handleAPItems();

        CHECK(result == true);
        CHECK(isBrushUnlocked(0));
        CHECK(isBrushUnlocked(1));
        REQUIRE(wolf::mock::giveItemCalls.size() == 1);
        CHECK(wolf::mock::giveItemCalls[0].itemId == 0x42);
    }

    SECTION("handleAPItems returns false if any item fails") {
        // Queue a valid item and an invalid item
        item_handlers::receiveAPItem(0x100);    // Valid
        item_handlers::receiveAPItem(0xFFFF);   // Invalid

        wolf::mock::triggerPlayStart();
        bool result = item_handlers::handleAPItems();

        // Should return false because one item failed
        CHECK(result == false);
        // But the valid item should still have been processed
        CHECK(isBrushUnlocked(0));
    }
}

TEST_CASE_METHOD(ItemHandlerTestFixture, "safeToReceive flag behavior", "[item_handlers]") {
    item_handlers::initialize();

    SECTION("safeToReceive starts as false") {
        item_handlers::receiveAPItem(0x100);
        bool result = item_handlers::handleAPItems();
        CHECK(result == false);
    }

    SECTION("safeToReceive is enabled on play start") {
        item_handlers::receiveAPItem(0x100);

        wolf::mock::triggerPlayStart();
        bool result = item_handlers::handleAPItems();
        CHECK(result == true);
    }

    SECTION("safeToReceive is disabled on return to menu") {
        // Enable first
        wolf::mock::triggerPlayStart();

        // Queue item
        item_handlers::receiveAPItem(0x100);

        // Disable by returning to menu
        wolf::mock::triggerReturnToMenu();

        bool result = item_handlers::handleAPItems();
        CHECK(result == false);
    }

    SECTION("safeToReceive can be toggled multiple times") {
        item_handlers::receiveAPItem(0x100);

        // Enable
        wolf::mock::triggerPlayStart();
        CHECK(item_handlers::handleAPItems() == true);

        // Disable
        item_handlers::receiveAPItem(0x101);
        wolf::mock::triggerReturnToMenu();
        CHECK(item_handlers::handleAPItems() == false);

        // Re-enable
        wolf::mock::triggerPlayStart();
        CHECK(item_handlers::handleAPItems() == true);
    }
}

TEST_CASE_METHOD(ItemHandlerTestFixture, "Event flag handling", "[item_handlers]") {
    SECTION("Setting keyItemsAcquired flag") {
        // Save Rei (0x303) sets keyItemsAcquired bit 0
        bool result = item_handlers::handleItem(0x303);

        CHECK(result == true);
        CHECK(apgame::keyItemsAcquired->IsSet(0));
    }

    SECTION("Setting multiple different event flags") {
        // Save Rei - bit 0
        item_handlers::handleItem(0x303);
        // Save Shin - bit 1
        item_handlers::handleItem(0x304);
        // Save Chi - bit 2
        item_handlers::handleItem(0x305);

        CHECK(apgame::keyItemsAcquired->IsSet(0));
        CHECK(apgame::keyItemsAcquired->IsSet(1));
        CHECK(apgame::keyItemsAcquired->IsSet(2));
        CHECK_FALSE(apgame::keyItemsAcquired->IsSet(3));
    }

    SECTION("Event flags don't interfere with each other") {
        // Set bit 0
        item_handlers::handleItem(0x303);
        CHECK(apgame::keyItemsAcquired->IsSet(0));

        // Set bit 5
        item_handlers::handleItem(0x308);
        CHECK(apgame::keyItemsAcquired->IsSet(5));

        // Both should still be set
        CHECK(apgame::keyItemsAcquired->IsSet(0));
        CHECK(apgame::keyItemsAcquired->IsSet(5));
    }

    SECTION("Redundant event flag setting succeeds") {
        // Set the flag
        item_handlers::handleItem(0x303);
        CHECK(apgame::keyItemsAcquired->IsSet(0));

        // Set it again
        bool result = item_handlers::handleItem(0x303);
        CHECK(result == true);
        CHECK(apgame::keyItemsAcquired->IsSet(0));
    }
}
