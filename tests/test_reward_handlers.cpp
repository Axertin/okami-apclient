#include <catch2/catch_test_macros.hpp>

#include "rewards/brushes.hpp"
#include "rewards/event_flags.hpp"
#include "rewards/game_items.hpp"
#include "rewards/reward_types.hpp"

// ============================================================================
// Category detection tests
// ============================================================================

TEST_CASE("getCategory returns correct category for AP ID ranges", "[rewards][category]")
{
    SECTION("Game items (0x00-0xFF)")
    {
        CHECK(rewards::getCategory(0x00) == rewards::RewardCategory::GameItem);
        CHECK(rewards::getCategory(0x42) == rewards::RewardCategory::GameItem);
        CHECK(rewards::getCategory(0xFF) == rewards::RewardCategory::GameItem);
    }

    SECTION("Brush techniques (0x100-0x115)")
    {
        CHECK(rewards::getCategory(0x100) == rewards::RewardCategory::Brush);
        CHECK(rewards::getCategory(0x102) == rewards::RewardCategory::Brush);
        CHECK(rewards::getCategory(0x115) == rewards::RewardCategory::Brush);
    }

    SECTION("Progressive weapons are GameItems (0x300-0x302)")
    {
        CHECK(rewards::getCategory(0x300) == rewards::RewardCategory::GameItem);
        CHECK(rewards::getCategory(0x301) == rewards::RewardCategory::GameItem);
        CHECK(rewards::getCategory(0x302) == rewards::RewardCategory::GameItem);
    }

    SECTION("Event flags (0x303-0x308)")
    {
        CHECK(rewards::getCategory(0x303) == rewards::RewardCategory::EventFlag);
        CHECK(rewards::getCategory(0x308) == rewards::RewardCategory::EventFlag);
    }

    SECTION("Unknown ranges return Unknown")
    {
        CHECK(rewards::getCategory(-1) == rewards::RewardCategory::Unknown);
        CHECK(rewards::getCategory(0x116) == rewards::RewardCategory::Unknown);
        CHECK(rewards::getCategory(0x2FF) == rewards::RewardCategory::Unknown);
        CHECK(rewards::getCategory(0x309) == rewards::RewardCategory::Unknown);
        CHECK(rewards::getCategory(0xDEAD) == rewards::RewardCategory::Unknown);
    }
}

TEST_CASE("isKnownItem returns true for valid ranges", "[rewards][category]")
{
    CHECK(rewards::isKnownItem(0x00));
    CHECK(rewards::isKnownItem(0x42));
    CHECK(rewards::isKnownItem(0xFF));
    CHECK(rewards::isKnownItem(0x100));
    CHECK(rewards::isKnownItem(0x115));
    CHECK(rewards::isKnownItem(0x300));
    CHECK(rewards::isKnownItem(0x308));

    CHECK_FALSE(rewards::isKnownItem(-1));
    CHECK_FALSE(rewards::isKnownItem(0x116));
    CHECK_FALSE(rewards::isKnownItem(0x309));
    CHECK_FALSE(rewards::isKnownItem(0xDEAD));
}

// ============================================================================
// Game items handler tests
// ============================================================================

TEST_CASE("game_items::getItemId extracts correct ID", "[rewards][game_items]")
{
    CHECK(rewards::game_items::getItemId(0x00) == 0x00);
    CHECK(rewards::game_items::getItemId(0x42) == 0x42);
    CHECK(rewards::game_items::getItemId(0xFF) == 0xFF);
}

TEST_CASE("game_items::isProgressiveWeapon detects progressive weapons", "[rewards][game_items]")
{
    CHECK(rewards::game_items::isProgressiveWeapon(0x300));
    CHECK(rewards::game_items::isProgressiveWeapon(0x301));
    CHECK(rewards::game_items::isProgressiveWeapon(0x302));

    CHECK_FALSE(rewards::game_items::isProgressiveWeapon(0x00));
    CHECK_FALSE(rewards::game_items::isProgressiveWeapon(0x42));
    CHECK_FALSE(rewards::game_items::isProgressiveWeapon(0x2FF));
    CHECK_FALSE(rewards::game_items::isProgressiveWeapon(0x303));
}

TEST_CASE("game_items::isDirectGameItem detects direct game items", "[rewards][game_items]")
{
    CHECK(rewards::game_items::isDirectGameItem(0x00));
    CHECK(rewards::game_items::isDirectGameItem(0x42));
    CHECK(rewards::game_items::isDirectGameItem(0xFF));

    // Progressive weapons are NOT direct game items
    CHECK_FALSE(rewards::game_items::isDirectGameItem(0x300));
    CHECK_FALSE(rewards::game_items::isDirectGameItem(0x301));
    CHECK_FALSE(rewards::game_items::isDirectGameItem(0x302));

    // Brushes and event flags are not direct game items
    CHECK_FALSE(rewards::game_items::isDirectGameItem(0x100));
    CHECK_FALSE(rewards::game_items::isDirectGameItem(0x303));
}

// ============================================================================
// Brushes handler tests
// ============================================================================

TEST_CASE("brushes::getBrushIndex extracts correct index", "[rewards][brushes]")
{
    CHECK(rewards::brushes::getBrushIndex(0x100) == 0);  // Sunrise
    CHECK(rewards::brushes::getBrushIndex(0x101) == 1);  // Rejuvenation
    CHECK(rewards::brushes::getBrushIndex(0x102) == 2);  // Power Slash
    CHECK(rewards::brushes::getBrushIndex(0x103) == 3);  // Cherry Bomb
    CHECK(rewards::brushes::getBrushIndex(0x115) == 21); // Icestorm
}

TEST_CASE("brushes::isProgressive detects progressive brushes", "[rewards][brushes]")
{
    // Power Slash and Cherry Bomb are progressive
    CHECK(rewards::brushes::isProgressive(0x102));
    CHECK(rewards::brushes::isProgressive(0x103));

    // All others are not
    CHECK_FALSE(rewards::brushes::isProgressive(0x100));
    CHECK_FALSE(rewards::brushes::isProgressive(0x101));
    CHECK_FALSE(rewards::brushes::isProgressive(0x104));
    CHECK_FALSE(rewards::brushes::isProgressive(0x115));
}

// ============================================================================
// Event flags handler tests
// ============================================================================

TEST_CASE("event_flags::getFlagBit extracts correct bit", "[rewards][event_flags]")
{
    CHECK(rewards::event_flags::getFlagBit(0x303) == 0); // Save Rei
    CHECK(rewards::event_flags::getFlagBit(0x304) == 1); // Save Shin
    CHECK(rewards::event_flags::getFlagBit(0x305) == 2); // Save Chi
    CHECK(rewards::event_flags::getFlagBit(0x306) == 3); // Save Ko
    CHECK(rewards::event_flags::getFlagBit(0x307) == 4); // Save Tei
    CHECK(rewards::event_flags::getFlagBit(0x308) == 5); // Serpent Crystal
}
