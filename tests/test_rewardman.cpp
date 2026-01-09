#include <catch2/catch_test_macros.hpp>

#include "gamestate_accessors.hpp"
#include "rewardman.h"
#include "rewards/brushes.hpp"
#include "rewards/game_items.hpp"
#include "wolf_framework.hpp"

// ============================================================================
// Test fixture for RewardMan tests
// ============================================================================

class RewardManFixture
{
  protected:
    std::unique_ptr<RewardMan> rewardMan_;
    std::vector<bool> checkSendingStates_;

    void SetUp()
    {
        wolf::mock::reset();
        checkSendingStates_.clear();

        // Reserve enough memory for all game state accessors
        // CollectionData at 0xB205D0, TrackerData at 0xB21780
        wolf::mock::reserveMemory(0xB21780 + sizeof(okami::TrackerData) + 1024);

        // Initialize game state accessors (points them to mock memory)
        apgame::initialize();

        rewardMan_ = std::make_unique<RewardMan>([this](bool enabled) { checkSendingStates_.push_back(enabled); });
    }

    void TearDown()
    {
        rewardMan_.reset();
        wolf::mock::reset();
    }

    // Helper to check if a brush technique is unlocked
    bool isBrushUnlocked(int brushIndex)
    {
        return apgame::usableBrushTechniques->IsSet(brushIndex) && apgame::obtainedBrushTechniques->IsSet(brushIndex);
    }

    // Helper to check if a brush upgrade is unlocked
    bool isBrushUpgradeUnlocked(int upgradeIndex)
    {
        return apgame::brushUpgrades->IsSet(upgradeIndex);
    }

    // Helper to get inventory count
    uint16_t getInventoryCount(int itemId)
    {
        return apgame::collectionData->inventory[itemId];
    }

    // Helper to check key item acquired
    bool isKeyItemAcquired(int bit)
    {
        return apgame::keyItemsAcquired->IsSet(bit);
    }

    // Helper to check gold dust acquired
    bool isGoldDustAcquired(int bit)
    {
        return apgame::goldDustsAcquired->IsSet(bit);
    }
};

// ============================================================================
// Construction and basic state tests
// ============================================================================

TEST_CASE("RewardMan granting disabled by default", "[rewardman]")
{
    RewardMan rewardMan([](bool) {});
    REQUIRE_FALSE(rewardMan.isGrantingEnabled());
}

TEST_CASE("RewardMan granting can be enabled", "[rewardman]")
{
    RewardMan rewardMan([](bool) {});
    rewardMan.setGrantingEnabled(true);
    REQUIRE(rewardMan.isGrantingEnabled());
}

TEST_CASE("RewardMan starts with empty queue", "[rewardman]")
{
    RewardMan rewardMan([](bool) {});
    REQUIRE(rewardMan.getQueuedCount() == 0);
}

// ============================================================================
// Queue management tests
// ============================================================================

TEST_CASE("Queue management", "[rewardman]")
{
    wolf::mock::reset();
    wolf::mock::reserveMemory(0xB21780 + sizeof(okami::TrackerData) + 1024);
    apgame::initialize();

    RewardMan rewardMan([](bool) {});

    SECTION("queueReward adds to queue")
    {
        rewardMan.queueReward(0x100);
        CHECK(rewardMan.getQueuedCount() == 1);

        rewardMan.queueReward(0x101);
        CHECK(rewardMan.getQueuedCount() == 2);
    }

    SECTION("processQueuedRewards clears queue when enabled")
    {
        rewardMan.queueReward(0x100);
        rewardMan.queueReward(0x101);
        rewardMan.setGrantingEnabled(true);

        rewardMan.processQueuedRewards();

        CHECK(rewardMan.getQueuedCount() == 0);
    }

    SECTION("processQueuedRewards skips when disabled")
    {
        rewardMan.queueReward(0x100);
        rewardMan.setGrantingEnabled(false);

        rewardMan.processQueuedRewards();

        // Queue should still have the item
        CHECK(rewardMan.getQueuedCount() == 1);
    }

    SECTION("reset clears queue")
    {
        rewardMan.queueReward(0x100);
        rewardMan.queueReward(0x101);
        CHECK(rewardMan.getQueuedCount() == 2);

        rewardMan.reset();
        CHECK(rewardMan.getQueuedCount() == 0);
    }

    SECTION("reset disables granting")
    {
        rewardMan.setGrantingEnabled(true);
        rewardMan.reset();
        CHECK_FALSE(rewardMan.isGrantingEnabled());
    }

    wolf::mock::reset();
}

// ============================================================================
// Grant GameItemReward tests
// ============================================================================

TEST_CASE_METHOD(RewardManFixture, "Grant GameItemReward calls wolf::giveItem", "[rewardman][granting]")
{
    SetUp();

    // 0x42 is Canine Tracker - a GameItemReward
    auto result = rewardMan_->grantReward(0x42);

    REQUIRE(result.has_value());
    REQUIRE(wolf::mock::giveItemCalls.size() == 1);
    CHECK(wolf::mock::giveItemCalls[0].itemId == 0x42);
    CHECK(wolf::mock::giveItemCalls[0].count == 1);

    TearDown();
}

// ============================================================================
// Grant BrushReward tests
// ============================================================================

TEST_CASE_METHOD(RewardManFixture, "Grant BrushReward sets brush bitfields", "[rewardman][granting]")
{
    SetUp();

    // 0x100 is Sunrise (brush_index 0)
    CHECK_FALSE(isBrushUnlocked(0));

    auto result = rewardMan_->grantReward(0x100);

    REQUIRE(result.has_value());
    CHECK(isBrushUnlocked(0));

    TearDown();
}

TEST_CASE_METHOD(RewardManFixture, "Grant different brushes sets correct indices", "[rewardman][granting]")
{
    SetUp();

    // 0x101 is Rejuvenation (brush_index 1)
    auto result = rewardMan_->grantReward(0x101);
    REQUIRE(result.has_value());
    CHECK(isBrushUnlocked(1));

    TearDown();
}

// ============================================================================
// Grant ProgressiveBrushReward tests
// ============================================================================

TEST_CASE_METHOD(RewardManFixture, "Grant ProgressiveBrushReward progression", "[rewardman][granting]")
{
    SetUp();

    // 0x102 is Power Slash (progressive brush)
    int brushIdx = rewards::brushes::getBrushIndex(0x102); // = 2

    SECTION("First grant gives base brush")
    {
        CHECK_FALSE(isBrushUnlocked(brushIdx));

        auto result = rewardMan_->grantReward(0x102);
        REQUIRE(result.has_value());
        CHECK(isBrushUnlocked(brushIdx));
    }

    SECTION("Second grant gives first upgrade")
    {
        // Grant base first
        (void)rewardMan_->grantReward(0x102);
        CHECK(isBrushUnlocked(brushIdx));

        // Power Slash upgrade bit 0 = Power Slash 2
        CHECK_FALSE(isBrushUpgradeUnlocked(0));

        // Grant again for upgrade
        auto result = rewardMan_->grantReward(0x102);
        REQUIRE(result.has_value());
        CHECK(isBrushUpgradeUnlocked(0));
    }

    SECTION("Third grant gives second upgrade")
    {
        // Grant base and first upgrade
        (void)rewardMan_->grantReward(0x102);
        (void)rewardMan_->grantReward(0x102);

        // Power Slash upgrade bit 10 = Power Slash 3
        CHECK_FALSE(isBrushUpgradeUnlocked(10));

        auto result = rewardMan_->grantReward(0x102);
        REQUIRE(result.has_value());
        CHECK(isBrushUpgradeUnlocked(10));
    }

    SECTION("Grant at max level is no-op")
    {
        // Grant base and all upgrades (3 total)
        (void)rewardMan_->grantReward(0x102);
        (void)rewardMan_->grantReward(0x102);
        (void)rewardMan_->grantReward(0x102);

        // One more should be a no-op (no error)
        auto result = rewardMan_->grantReward(0x102);
        REQUIRE(result.has_value());
    }

    TearDown();
}

// ============================================================================
// Grant EventFlagReward tests
// ============================================================================

TEST_CASE_METHOD(RewardManFixture, "Grant EventFlagReward sets keyItemsAcquired", "[rewardman][granting]")
{
    SetUp();

    SECTION("Save Rei (0x303) sets bit 0")
    {
        CHECK_FALSE(isKeyItemAcquired(0));
        auto result = rewardMan_->grantReward(0x303);
        REQUIRE(result.has_value());
        CHECK(isKeyItemAcquired(0));
    }

    SECTION("Save Shin (0x304) sets bit 1")
    {
        CHECK_FALSE(isKeyItemAcquired(1));
        auto result = rewardMan_->grantReward(0x304);
        REQUIRE(result.has_value());
        CHECK(isKeyItemAcquired(1));
    }

    SECTION("Serpent Crystal (0x308) sets bit 5")
    {
        CHECK_FALSE(isKeyItemAcquired(5));
        auto result = rewardMan_->grantReward(0x308);
        REQUIRE(result.has_value());
        CHECK(isKeyItemAcquired(5));
    }

    TearDown();
}

// ============================================================================
// Grant ProgressiveWeaponReward tests
// ============================================================================

TEST_CASE_METHOD(RewardManFixture, "Grant ProgressiveWeaponReward progression", "[rewardman][granting]")
{
    SetUp();

    // 0x300 is Progressive Mirror: stages are 0x13 (Trinity Mirror), 0x14 (Solar Flare)
    constexpr uint8_t kTrinityMirror = 0x13;
    constexpr uint8_t kSolarFlare = 0x14;

    SECTION("First grant gives stage 1 (Trinity Mirror)")
    {
        wolf::mock::giveItemCalls.clear();

        auto result = rewardMan_->grantReward(0x300);
        REQUIRE(result.has_value());
        REQUIRE(wolf::mock::giveItemCalls.size() == 1);
        CHECK(wolf::mock::giveItemCalls[0].itemId == kTrinityMirror);
    }

    SECTION("Second grant gives stage 2 (Solar Flare)")
    {
        // Simulate having stage 1 in inventory
        apgame::collectionData->inventory[kTrinityMirror] = 1;

        wolf::mock::giveItemCalls.clear();

        auto result = rewardMan_->grantReward(0x300);
        REQUIRE(result.has_value());
        REQUIRE(wolf::mock::giveItemCalls.size() == 1);
        CHECK(wolf::mock::giveItemCalls[0].itemId == kSolarFlare);
    }

    SECTION("Grant at max stage is no-op")
    {
        // Simulate having all stages
        apgame::collectionData->inventory[kTrinityMirror] = 1;
        apgame::collectionData->inventory[kSolarFlare] = 1;

        wolf::mock::giveItemCalls.clear();

        auto result = rewardMan_->grantReward(0x300);
        REQUIRE(result.has_value());
        CHECK(wolf::mock::giveItemCalls.empty());
    }

    TearDown();
}

// ============================================================================
// Grant FillerReward tests
// ============================================================================

TEST_CASE_METHOD(RewardManFixture, "Grant FillerReward is no-op", "[rewardman][granting]")
{
    SetUp();

    // Unknown/filler items are silently skipped and return success
    auto result = rewardMan_->grantReward(0xDEAD);
    CHECK(result.has_value()); // Success - treated as no-op

    TearDown();
}

// ============================================================================
// Grant unknown item tests
// ============================================================================

TEST_CASE_METHOD(RewardManFixture, "Grant unknown item succeeds as no-op", "[rewardman][granting]")
{
    SetUp();

    // Unknown items are treated as filler and silently succeed
    auto result = rewardMan_->grantReward(0xDEAD);
    CHECK(result.has_value()); // Success - no error

    TearDown();
}

// ============================================================================
// Helper function tests
// ============================================================================

TEST_CASE("Helper functions", "[rewardman]")
{
    RewardMan rewardMan([](bool) {});

    SECTION("isGameItem returns true for direct inventory items")
    {
        // 0x42 is Canine Tracker - a direct game item
        CHECK(rewardMan.isGameItem(0x42));
        CHECK(rewardMan.isGameItem(0x00));
        CHECK(rewardMan.isGameItem(0xFF));
    }

    SECTION("isGameItem returns false for progressive weapons")
    {
        // Progressive weapons are NOT direct game items
        CHECK_FALSE(rewardMan.isGameItem(0x300));
        CHECK_FALSE(rewardMan.isGameItem(0x301));
        CHECK_FALSE(rewardMan.isGameItem(0x302));
    }

    SECTION("isGameItem returns false for brushes")
    {
        CHECK_FALSE(rewardMan.isGameItem(0x100));
        CHECK_FALSE(rewardMan.isGameItem(0x102));
    }

    SECTION("isGameItem returns false for event flags")
    {
        CHECK_FALSE(rewardMan.isGameItem(0x303));
    }

    SECTION("isGameItem returns false for unknown items")
    {
        CHECK_FALSE(rewardMan.isGameItem(0xDEAD));
    }

    SECTION("getGameItemId returns id for direct inventory items")
    {
        auto id = rewardMan.getGameItemId(0x42);
        REQUIRE(id.has_value());
        CHECK(*id == 0x42);
    }

    SECTION("getGameItemId returns nullopt for progressive weapons")
    {
        auto id = rewardMan.getGameItemId(0x300);
        CHECK_FALSE(id.has_value());
    }

    SECTION("getGameItemId returns nullopt for brushes")
    {
        auto id = rewardMan.getGameItemId(0x100);
        CHECK_FALSE(id.has_value());
    }

    SECTION("getGameItemId returns nullopt for unknown items")
    {
        auto id = rewardMan.getGameItemId(0xDEAD);
        CHECK_FALSE(id.has_value());
    }
}

// ============================================================================
// CheckSendingCallback tests
// ============================================================================

TEST_CASE_METHOD(RewardManFixture, "CheckSendingCallback is invoked during granting", "[rewardman][callback]")
{
    SetUp();

    checkSendingStates_.clear();

    // Grant a known item
    (void)rewardMan_->grantReward(0x42);

    // Callback should be invoked with false, then true
    REQUIRE(checkSendingStates_.size() == 2);
    CHECK(checkSendingStates_[0] == false); // Disabled before granting
    CHECK(checkSendingStates_[1] == true);  // Re-enabled after granting

    TearDown();
}

// ============================================================================
// Lifecycle callback tests
// ============================================================================

TEST_CASE("Lifecycle callbacks", "[rewardman]")
{
    wolf::mock::reset();

    RewardMan rewardMan([](bool) {});

    SECTION("onPlayStart enables granting")
    {
        CHECK_FALSE(rewardMan.isGrantingEnabled());
        wolf::mock::triggerPlayStart();
        CHECK(rewardMan.isGrantingEnabled());
    }

    SECTION("onReturnToMenu disables granting")
    {
        wolf::mock::triggerPlayStart();
        CHECK(rewardMan.isGrantingEnabled());
        wolf::mock::triggerReturnToMenu();
        CHECK_FALSE(rewardMan.isGrantingEnabled());
    }

    wolf::mock::reset();
}
