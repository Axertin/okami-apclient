#include <catch2/catch_test_macros.hpp>
#include <okami/bitfield.hpp>
#include <okami/structs.hpp>
#include <wolf_framework.hpp>

#include "apitem.h"
#include "gamestate_accessors.hpp"

class ApItemTestFixture
{
  public:
    ApItemTestFixture()
    {
        wolf::mock::reset();
        wolf::mock::reserveMemory(20 * 1024 * 1024);
        apgame::initialize();
    }

    ~ApItemTestFixture()
    {
        wolf::mock::reset();
    }

    bool isBrushUnlocked(int index)
    {
        return apgame::usableBrushTechniques->IsSet(index) && apgame::obtainedBrushTechniques->IsSet(index);
    }

    bool isBrushUpgradeUnlocked(int bit)
    {
        return apgame::brushUpgrades->IsSet(bit);
    }

    uint16_t getInventoryCount(int itemId)
    {
        return apgame::collectionData->inventory[itemId];
    }

    void setInventoryCount(int itemId, uint16_t count)
    {
        apgame::collectionData->inventory[itemId] = count;
    }
};

// Pure function tests - getReward type conversion
TEST_CASE("ap::getReward returns correct variant type", "[apitem]")
{
    SECTION("Brush item returns BrushReward")
    {
        auto reward = ap::getReward(0x100); // Sunrise brush
        REQUIRE(reward.has_value());
        REQUIRE(std::holds_alternative<ap::BrushReward>(reward->data));

        auto *brush = std::get_if<ap::BrushReward>(&reward->data);
        REQUIRE(brush != nullptr);
        CHECK(brush->brush_index == 0);
    }

    SECTION("Inventory item returns GameItemReward")
    {
        auto reward = ap::getReward(0x42); // Canine Tracker
        REQUIRE(reward.has_value());
        REQUIRE(std::holds_alternative<ap::GameItemReward>(reward->data));

        auto *gameItem = std::get_if<ap::GameItemReward>(&reward->data);
        REQUIRE(gameItem != nullptr);
        CHECK(gameItem->item_id == 0x42);
    }

    SECTION("Progressive brush returns ProgressiveBrushReward")
    {
        auto reward = ap::getReward(0x102); // Power Slash
        REQUIRE(reward.has_value());
        REQUIRE(std::holds_alternative<ap::ProgressiveBrushReward>(reward->data));

        auto *progBrush = std::get_if<ap::ProgressiveBrushReward>(&reward->data);
        REQUIRE(progBrush != nullptr);
        CHECK(progBrush->brush_index == 2);
        CHECK(progBrush->upgrades.size() > 0);
    }

    SECTION("Event flag item returns EventFlagReward")
    {
        auto reward = ap::getReward(0x303); // Save Rei
        REQUIRE(reward.has_value());
        REQUIRE(std::holds_alternative<ap::EventFlagReward>(reward->data));

        auto *eventFlag = std::get_if<ap::EventFlagReward>(&reward->data);
        REQUIRE(eventFlag != nullptr);
        CHECK(eventFlag->accessor == "keyItemsAcquired");
        CHECK(eventFlag->bit == 0);
    }

    SECTION("Progressive weapon returns ProgressiveWeaponReward")
    {
        auto reward = ap::getReward(0x300); // Progressive Mirror
        REQUIRE(reward.has_value());
        REQUIRE(std::holds_alternative<ap::ProgressiveWeaponReward>(reward->data));

        auto *progWeapon = std::get_if<ap::ProgressiveWeaponReward>(&reward->data);
        REQUIRE(progWeapon != nullptr);
        CHECK(progWeapon->stages.size() >= 2);
    }
}

TEST_CASE("ap::getReward returns nullopt for unknown item", "[apitem]")
{
    auto reward = ap::getReward(0xFFFF);
    CHECK_FALSE(reward.has_value());
}

// Stateful tests - grantReward
TEST_CASE_METHOD(ApItemTestFixture, "ap::grantReward with GameItemReward", "[apitem]")
{
    ap::Reward reward{.ap_item_id = 0x42, .data = ap::GameItemReward{.item_id = 0x42}};

    auto result = ap::grantReward(reward);

    REQUIRE(result.has_value());
    REQUIRE(wolf::mock::giveItemCalls.size() == 1);
    CHECK(wolf::mock::giveItemCalls[0].itemId == 0x42);
    CHECK(wolf::mock::giveItemCalls[0].count == 1);
}

TEST_CASE_METHOD(ApItemTestFixture, "ap::grantReward with BrushReward", "[apitem]")
{
    ap::Reward reward{.ap_item_id = 0x100, .data = ap::BrushReward{.brush_index = 5}};

    auto result = ap::grantReward(reward);

    REQUIRE(result.has_value());
    CHECK(isBrushUnlocked(5));
}

TEST_CASE_METHOD(ApItemTestFixture, "ap::grantReward with ProgressiveBrushReward", "[apitem]")
{
    static constexpr uint32_t upgrades[] = {0, 10}; // Power Slash 2, Power Slash 3

    SECTION("First grant gives base brush")
    {
        ap::Reward reward{.ap_item_id = 0x102, .data = ap::ProgressiveBrushReward{.brush_index = 2, .upgrades = std::span<const uint32_t>(upgrades)}};

        auto result = ap::grantReward(reward);

        REQUIRE(result.has_value());
        CHECK(isBrushUnlocked(2));
        CHECK_FALSE(isBrushUpgradeUnlocked(0));
    }

    SECTION("Second grant gives first upgrade")
    {
        // Set up base brush
        apgame::usableBrushTechniques->Set(2);
        apgame::obtainedBrushTechniques->Set(2);

        ap::Reward reward{.ap_item_id = 0x102, .data = ap::ProgressiveBrushReward{.brush_index = 2, .upgrades = std::span<const uint32_t>(upgrades)}};

        auto result = ap::grantReward(reward);

        REQUIRE(result.has_value());
        CHECK(isBrushUpgradeUnlocked(0));
        CHECK_FALSE(isBrushUpgradeUnlocked(10));
    }

    SECTION("Third grant gives second upgrade")
    {
        // Set up base brush and first upgrade
        apgame::usableBrushTechniques->Set(2);
        apgame::obtainedBrushTechniques->Set(2);
        apgame::brushUpgrades->Set(0);

        ap::Reward reward{.ap_item_id = 0x102, .data = ap::ProgressiveBrushReward{.brush_index = 2, .upgrades = std::span<const uint32_t>(upgrades)}};

        auto result = ap::grantReward(reward);

        REQUIRE(result.has_value());
        CHECK(isBrushUpgradeUnlocked(10));
    }
}

TEST_CASE_METHOD(ApItemTestFixture, "ap::grantReward with EventFlagReward", "[apitem]")
{
    SECTION("Sets keyItemsAcquired flag")
    {
        ap::Reward reward{.ap_item_id = 0x303, .data = ap::EventFlagReward{.accessor = "keyItemsAcquired", .bit = 3}};

        auto result = ap::grantReward(reward);

        REQUIRE(result.has_value());
        CHECK(apgame::keyItemsAcquired->IsSet(3));
    }

    SECTION("Sets goldDustsAcquired flag")
    {
        ap::Reward reward{.ap_item_id = 0x400, .data = ap::EventFlagReward{.accessor = "goldDustsAcquired", .bit = 5}};

        auto result = ap::grantReward(reward);

        REQUIRE(result.has_value());
        CHECK(apgame::goldDustsAcquired->IsSet(5));
    }

    SECTION("Unknown accessor returns error")
    {
        ap::Reward reward{.ap_item_id = 0x999, .data = ap::EventFlagReward{.accessor = "unknownAccessor", .bit = 0}};

        auto result = ap::grantReward(reward);

        REQUIRE_FALSE(result.has_value());
        CHECK(result.error().message.find("Unknown accessor") != std::string::npos);
    }
}

TEST_CASE_METHOD(ApItemTestFixture, "ap::grantReward with ProgressiveWeaponReward", "[apitem]")
{
    static constexpr int stages[] = {0x13, 0x14}; // Trinity Mirror, Solar Flare

    SECTION("First grant gives first stage")
    {
        ap::Reward reward{.ap_item_id = 0x300, .data = ap::ProgressiveWeaponReward{.stages = std::span<const int>(stages)}};

        auto result = ap::grantReward(reward);

        REQUIRE(result.has_value());
        REQUIRE(wolf::mock::giveItemCalls.size() == 1);
        CHECK(wolf::mock::giveItemCalls[0].itemId == 0x13);
    }

    SECTION("Second grant gives second stage")
    {
        setInventoryCount(0x13, 1); // Already have Trinity Mirror

        ap::Reward reward{.ap_item_id = 0x300, .data = ap::ProgressiveWeaponReward{.stages = std::span<const int>(stages)}};

        auto result = ap::grantReward(reward);

        REQUIRE(result.has_value());
        REQUIRE(wolf::mock::giveItemCalls.size() == 1);
        CHECK(wolf::mock::giveItemCalls[0].itemId == 0x14);
    }

    SECTION("At max stage does nothing")
    {
        setInventoryCount(0x13, 1);
        setInventoryCount(0x14, 1);

        ap::Reward reward{.ap_item_id = 0x300, .data = ap::ProgressiveWeaponReward{.stages = std::span<const int>(stages)}};

        auto result = ap::grantReward(reward);

        REQUIRE(result.has_value());
        CHECK(wolf::mock::giveItemCalls.empty());
    }
}

TEST_CASE_METHOD(ApItemTestFixture, "ap::grantReward with FillerReward", "[apitem]")
{
    ap::Reward reward{.ap_item_id = 0x999, .data = ap::FillerReward{.message = "You found junk!"}};

    auto result = ap::grantReward(reward);

    REQUIRE(result.has_value());
    CHECK(wolf::mock::giveItemCalls.empty()); // Nothing granted
}

// Helper function tests
TEST_CASE("ap::isGameItem", "[apitem]")
{
    SECTION("Returns true for inventory items")
    {
        CHECK(ap::isGameItem(0x42)); // Canine Tracker
    }

    SECTION("Returns false for brush items")
    {
        CHECK_FALSE(ap::isGameItem(0x100)); // Sunrise
    }

    SECTION("Returns false for progressive brush")
    {
        CHECK_FALSE(ap::isGameItem(0x102)); // Power Slash
    }

    SECTION("Returns false for event flags")
    {
        CHECK_FALSE(ap::isGameItem(0x303)); // Save Rei
    }

    SECTION("Returns false for unknown items")
    {
        CHECK_FALSE(ap::isGameItem(0xFFFF));
    }
}

TEST_CASE("ap::getGameItemId", "[apitem]")
{
    SECTION("Returns item_id for inventory items")
    {
        auto id = ap::getGameItemId(0x42);
        REQUIRE(id.has_value());
        CHECK(*id == 0x42);
    }

    SECTION("Returns nullopt for non-inventory items")
    {
        CHECK_FALSE(ap::getGameItemId(0x100).has_value()); // Brush
        CHECK_FALSE(ap::getGameItemId(0x102).has_value()); // Progressive brush
        CHECK_FALSE(ap::getGameItemId(0x303).has_value()); // Event flag
    }

    SECTION("Returns nullopt for unknown items")
    {
        CHECK_FALSE(ap::getGameItemId(0xFFFF).has_value());
    }
}
