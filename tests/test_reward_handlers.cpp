#include <catch2/catch_test_macros.hpp>

#include "rewards/brushes.hpp"
#include "rewards/event_flags.hpp"
#include "rewards/game_items.hpp"
#include "rewards/reward_types.hpp"

// ============================================================================
// Compile-time verification of range constants
// ============================================================================

static_assert(rewards::kGameItemMax == 0xFF);
static_assert(rewards::kBrushBase == 0x100);
static_assert(rewards::kBrushEnd == 0x11E);
static_assert(rewards::kProgressiveWeaponBase == 0x300);
static_assert(rewards::kProgressiveWeaponEnd == 0x302);
static_assert(rewards::kEventFlagBase == 0x303);
static_assert(rewards::kEventFlagEnd == 0x308);

// ============================================================================
// Category detection boundary tests
// ============================================================================

TEST_CASE("Reward category boundaries", "[rewards][category]")
{
    // Test transitions between categories - catches off-by-one errors

    SECTION("GameItem boundaries")
    {
        CHECK(rewards::getCategory(-1) == rewards::RewardCategory::Unknown);
        CHECK(rewards::getCategory(0x00) == rewards::RewardCategory::GameItem);
        CHECK(rewards::getCategory(0xFF) == rewards::RewardCategory::GameItem);
        CHECK(rewards::getCategory(0x100) == rewards::RewardCategory::Brush); // NOT GameItem
    }

    SECTION("Brush boundaries")
    {
        CHECK(rewards::getCategory(0x0FF) == rewards::RewardCategory::GameItem);
        CHECK(rewards::getCategory(0x100) == rewards::RewardCategory::Brush);
        CHECK(rewards::getCategory(0x11E) == rewards::RewardCategory::Brush);
        CHECK(rewards::getCategory(0x11F) == rewards::RewardCategory::Unknown); // Gap before weapons
    }

    SECTION("Progressive weapon boundaries")
    {
        CHECK(rewards::getCategory(0x2FF) == rewards::RewardCategory::Unknown);
        CHECK(rewards::getCategory(0x300) == rewards::RewardCategory::GameItem);
        CHECK(rewards::getCategory(0x302) == rewards::RewardCategory::GameItem);
        CHECK(rewards::getCategory(0x303) == rewards::RewardCategory::EventFlag); // NOT GameItem
    }

    SECTION("Event flag boundaries")
    {
        CHECK(rewards::getCategory(0x302) == rewards::RewardCategory::GameItem);
        CHECK(rewards::getCategory(0x303) == rewards::RewardCategory::EventFlag);
        CHECK(rewards::getCategory(0x308) == rewards::RewardCategory::EventFlag);
        CHECK(rewards::getCategory(0x309) == rewards::RewardCategory::Unknown);
    }
}

// ============================================================================
// Progressive item detection
// ============================================================================

TEST_CASE("Progressive item detection boundaries", "[rewards]")
{
    SECTION("Progressive weapons")
    {
        CHECK_FALSE(rewards::game_items::isProgressiveWeapon(0x2FF));
        CHECK(rewards::game_items::isProgressiveWeapon(0x300));
        CHECK(rewards::game_items::isProgressiveWeapon(0x302));
        CHECK_FALSE(rewards::game_items::isProgressiveWeapon(0x303));
    }

    SECTION("Progressive brushes are exactly Power Slash and Cherry Bomb")
    {
        CHECK_FALSE(rewards::brushes::isProgressive(0x10B)); // Before Power Slash
        CHECK(rewards::brushes::isProgressive(0x10C));       // Power Slash (bit 12)
        CHECK_FALSE(rewards::brushes::isProgressive(0x10D)); // After Power Slash
        CHECK_FALSE(rewards::brushes::isProgressive(0x118)); // Before Cherry Bomb
        CHECK(rewards::brushes::isProgressive(0x119));       // Cherry Bomb (bit 25)
        CHECK_FALSE(rewards::brushes::isProgressive(0x11A)); // After Cherry Bomb
    }

    SECTION("Direct game items exclude progressive weapons")
    {
        CHECK(rewards::game_items::isDirectGameItem(0xFF));
        CHECK_FALSE(rewards::game_items::isDirectGameItem(0x100)); // Brush
        CHECK_FALSE(rewards::game_items::isDirectGameItem(0x300)); // Progressive weapon
    }
}

