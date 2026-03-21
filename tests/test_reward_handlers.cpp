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
static_assert(rewards::kBrushEnd == 0x115);
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
        CHECK(rewards::getCategory(0x115) == rewards::RewardCategory::Brush);
        CHECK(rewards::getCategory(0x116) == rewards::RewardCategory::Unknown); // Gap before weapons
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
        CHECK_FALSE(rewards::brushes::isProgressive(0x101)); // Before
        CHECK(rewards::brushes::isProgressive(0x102));       // Power Slash
        CHECK(rewards::brushes::isProgressive(0x103));       // Cherry Bomb
        CHECK_FALSE(rewards::brushes::isProgressive(0x104)); // After
    }

    SECTION("Direct game items exclude progressive weapons")
    {
        CHECK(rewards::game_items::isDirectGameItem(0xFF));
        CHECK_FALSE(rewards::game_items::isDirectGameItem(0x100)); // Brush
        CHECK_FALSE(rewards::game_items::isDirectGameItem(0x300)); // Progressive weapon
    }
}

// ============================================================================
// AP item flag classification tests
// ============================================================================

TEST_CASE("isTrap: flag bit 0x4 detection", "[rewards][flags]")
{
    CHECK_FALSE(rewards::isTrap(0x0));
    CHECK(rewards::isTrap(0x4));
    CHECK(rewards::isTrap(0x5));       // Progression + Trap
    CHECK(rewards::isTrap(0x7));       // All bits set
    CHECK_FALSE(rewards::isTrap(0x3)); // Progression + NeverExclude only
    CHECK_FALSE(rewards::isTrap(0x2)); // NeverExclude only
    CHECK_FALSE(rewards::isTrap(0x1)); // Progression only
}

TEST_CASE("isProgression: flag bit 0x1 detection", "[rewards][flags]")
{
    CHECK_FALSE(rewards::isProgression(0x0));
    CHECK(rewards::isProgression(0x1));
    CHECK(rewards::isProgression(0x5));       // Progression + Trap
    CHECK(rewards::isProgression(0x3));       // Progression + NeverExclude
    CHECK(rewards::isProgression(0xFF));      // All bits set
    CHECK_FALSE(rewards::isProgression(0x4)); // Trap only
    CHECK_FALSE(rewards::isProgression(0x2)); // NeverExclude only
}

TEST_CASE("isForeignItem: player slot comparison", "[rewards][flags]")
{
    CHECK_FALSE(rewards::isForeignItem(1, 1)); // Same slot → native
    CHECK(rewards::isForeignItem(2, 1));       // Different slot → foreign
    CHECK_FALSE(rewards::isForeignItem(0, 0)); // Both slot 0 → native
    CHECK(rewards::isForeignItem(0, 1));       // Src 0, my slot 1 → foreign
    CHECK(rewards::isForeignItem(1, 0));       // Src 1, my slot 0 → foreign
}
