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
        wolf::mock::reserveMemory(0xB21780 + sizeof(okami::TrackerData) + 1024);
        apgame::initialize();

        rewardMan_ = std::make_unique<RewardMan>([this](bool enabled) { checkSendingStates_.push_back(enabled); });
    }

    void TearDown()
    {
        rewardMan_.reset();
        wolf::mock::reset();
    }

    bool isBrushUnlocked(int brushIndex)
    {
        return apgame::usableBrushTechniques->IsSet(brushIndex) && apgame::obtainedBrushTechniques->IsSet(brushIndex);
    }

    bool isBrushUpgradeUnlocked(int upgradeIndex)
    {
        return apgame::brushUpgrades->IsSet(upgradeIndex);
    }

    bool isKeyItemAcquired(int bit)
    {
        return apgame::keyItemsAcquired->IsSet(bit);
    }
};

// ============================================================================
// Queue behavior tests
// ============================================================================

TEST_CASE("Queue management", "[rewardman]")
{
    wolf::mock::reset();
    wolf::mock::reserveMemory(0xB21780 + sizeof(okami::TrackerData) + 1024);
    apgame::initialize();

    RewardMan rewardMan([](bool) {});

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

        CHECK(rewardMan.getQueuedCount() == 1);
    }

    SECTION("reset clears queue and disables granting")
    {
        rewardMan.queueReward(0x100);
        rewardMan.setGrantingEnabled(true);

        rewardMan.reset();

        CHECK(rewardMan.getQueuedCount() == 0);
        CHECK_FALSE(rewardMan.isGrantingEnabled());
    }

    wolf::mock::reset();
}

// ============================================================================
// Reward granting tests
// ============================================================================

TEST_CASE_METHOD(RewardManFixture, "Grant GameItemReward calls wolf::giveItem", "[rewardman][granting]")
{
    SetUp();

    auto result = rewardMan_->grantReward(0x42);

    REQUIRE(result.has_value());
    REQUIRE(wolf::mock::giveItemCalls.size() == 1);
    CHECK(wolf::mock::giveItemCalls[0].itemId == 0x42);
    CHECK(wolf::mock::giveItemCalls[0].count == 1);

    TearDown();
}

TEST_CASE_METHOD(RewardManFixture, "Grant BrushReward sets brush bitfields", "[rewardman][granting]")
{
    SetUp();

    CHECK_FALSE(isBrushUnlocked(0));
    auto result = rewardMan_->grantReward(0x100); // Sunrise

    REQUIRE(result.has_value());
    CHECK(isBrushUnlocked(0));

    TearDown();
}

TEST_CASE_METHOD(RewardManFixture, "Grant ProgressiveBrushReward progression", "[rewardman][granting]")
{
    SetUp();

    int brushIdx = rewards::brushes::getBrushIndex(0x102); // Power Slash = 2

    SECTION("First grant gives base brush")
    {
        CHECK_FALSE(isBrushUnlocked(brushIdx));
        auto result = rewardMan_->grantReward(0x102);
        REQUIRE(result.has_value());
        CHECK(isBrushUnlocked(brushIdx));
    }

    SECTION("Second grant gives first upgrade")
    {
        (void)rewardMan_->grantReward(0x102);
        CHECK_FALSE(isBrushUpgradeUnlocked(0));

        auto result = rewardMan_->grantReward(0x102);
        REQUIRE(result.has_value());
        CHECK(isBrushUpgradeUnlocked(0));
    }

    SECTION("Third grant gives second upgrade")
    {
        (void)rewardMan_->grantReward(0x102);
        (void)rewardMan_->grantReward(0x102);
        CHECK_FALSE(isBrushUpgradeUnlocked(10));

        auto result = rewardMan_->grantReward(0x102);
        REQUIRE(result.has_value());
        CHECK(isBrushUpgradeUnlocked(10));
    }

    SECTION("Grant at max level is no-op")
    {
        (void)rewardMan_->grantReward(0x102);
        (void)rewardMan_->grantReward(0x102);
        (void)rewardMan_->grantReward(0x102);

        auto result = rewardMan_->grantReward(0x102);
        REQUIRE(result.has_value());
    }

    TearDown();
}

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

    SECTION("Serpent Crystal (0x308) sets bit 5")
    {
        CHECK_FALSE(isKeyItemAcquired(5));
        auto result = rewardMan_->grantReward(0x308);
        REQUIRE(result.has_value());
        CHECK(isKeyItemAcquired(5));
    }

    TearDown();
}

TEST_CASE_METHOD(RewardManFixture, "Grant ProgressiveWeaponReward progression", "[rewardman][granting]")
{
    SetUp();

    constexpr uint8_t kTrinityMirror = 0x13;
    constexpr uint8_t kSolarFlare = 0x14;

    SECTION("First grant gives stage 1")
    {
        wolf::mock::giveItemCalls.clear();

        auto result = rewardMan_->grantReward(0x300);
        REQUIRE(result.has_value());
        REQUIRE(wolf::mock::giveItemCalls.size() == 1);
        CHECK(wolf::mock::giveItemCalls[0].itemId == kTrinityMirror);
    }

    SECTION("Second grant gives stage 2")
    {
        apgame::collectionData->inventory[kTrinityMirror] = 1;
        wolf::mock::giveItemCalls.clear();

        auto result = rewardMan_->grantReward(0x300);
        REQUIRE(result.has_value());
        REQUIRE(wolf::mock::giveItemCalls.size() == 1);
        CHECK(wolf::mock::giveItemCalls[0].itemId == kSolarFlare);
    }

    SECTION("Grant at max stage is no-op")
    {
        apgame::collectionData->inventory[kTrinityMirror] = 1;
        apgame::collectionData->inventory[kSolarFlare] = 1;
        wolf::mock::giveItemCalls.clear();

        auto result = rewardMan_->grantReward(0x300);
        REQUIRE(result.has_value());
        CHECK(wolf::mock::giveItemCalls.empty());
    }

    TearDown();
}

TEST_CASE_METHOD(RewardManFixture, "Grant unknown item succeeds as no-op", "[rewardman][granting]")
{
    SetUp();

    auto result = rewardMan_->grantReward(0xDEAD);
    CHECK(result.has_value());

    TearDown();
}

// ============================================================================
// Callback tests
// ============================================================================

TEST_CASE_METHOD(RewardManFixture, "CheckSendingCallback is invoked during granting", "[rewardman][callback]")
{
    SetUp();

    checkSendingStates_.clear();
    (void)rewardMan_->grantReward(0x42);

    REQUIRE(checkSendingStates_.size() == 2);
    CHECK(checkSendingStates_[0] == false); // Disabled before granting
    CHECK(checkSendingStates_[1] == true);  // Re-enabled after granting

    TearDown();
}

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
