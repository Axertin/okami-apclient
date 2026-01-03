#pragma once

#include <cstdint>
#include <expected>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <variant>

/**
 * @brief Archipelago reward system
 *
 * This module handles AP "items" which are abstract rewards that can be:
 * - Game items (directly swappable in containers)
 * - Brush techniques
 * - Progressive brush upgrades
 * - Event flags (unlocks, state changes)
 * - Filler (message only, no game effect)
 *
 * Design separates AP items from game items - an AP item might grant
 * a game item, or it might grant something else entirely.
 */
namespace ap
{

// Reward payload types - each represents a different kind of reward

/**
 * @brief A direct game item that can be placed in inventory
 */
struct GameItemReward
{
    uint8_t item_id; // Okami item ID (0x00-0xFF)
};

/**
 * @brief A brush technique unlock
 */
struct BrushReward
{
    int brush_index; // Index in brush techniques bitfield
};

/**
 * @brief A progressive brush with upgrades (Power Slash, Cherry Bomb, etc.)
 */
struct ProgressiveBrushReward
{
    int brush_index;                    // Base brush index
    std::span<const uint32_t> upgrades; // Upgrade bit indices
};

/**
 * @brief Event flag(s) to set in game state
 */
struct EventFlagReward
{
    std::string_view accessor; // Accessor name in apgame namespace
    uint32_t bit;              // Bit index to set
};

/**
 * @brief Multiple event flags bundled together
 */
struct MultiFlagReward
{
    struct Flag
    {
        std::string_view accessor;
        uint32_t bit;
    };
    std::span<const Flag> flags;
};

/**
 * @brief Progressive weapon stages (mirrors, rosaries, swords)
 */
struct ProgressiveWeaponReward
{
    std::span<const int> stages; // Item IDs for each stage
};

/**
 * @brief Filler/junk item - displays message but grants nothing
 */
struct FillerReward
{
    std::string_view message;
};

/**
 * @brief Type-safe variant of all reward types
 */
using RewardData = std::variant<GameItemReward, BrushReward, ProgressiveBrushReward, EventFlagReward, MultiFlagReward, ProgressiveWeaponReward, FillerReward>;

/**
 * @brief Complete reward definition
 */
struct Reward
{
    int64_t ap_item_id; // Archipelago item ID
    RewardData data;    // Type-specific reward data
};

/**
 * @brief Error type for reward granting failures
 */
struct RewardError
{
    std::string message;
};

/**
 * @brief Resolve an AP item ID to a Reward
 *
 * Uses the existing apitems registry to look up and convert to the new
 * Reward format.
 *
 * @param ap_item_id The Archipelago item ID
 * @return The reward definition, or nullopt if unknown item
 */
[[nodiscard]] std::optional<Reward> getReward(int64_t ap_item_id);

/**
 * @brief Grant a reward to the player
 *
 * Handles all reward types via std::visit dispatch. Automatically
 * disables location monitor during granting to prevent feedback loops.
 *
 * @param reward The reward to grant
 * @return Success or error with message
 */
[[nodiscard]] std::expected<void, RewardError> grantReward(const Reward &reward);

/**
 * @brief Check if an AP item ID corresponds to a game item
 *
 * Used to determine if an item can be directly swapped in containers
 * vs needing a dummy placeholder.
 *
 * @param ap_item_id The Archipelago item ID
 * @return true if this is a GameItemReward type
 */
[[nodiscard]] bool isGameItem(int64_t ap_item_id);

/**
 * @brief Get the game item ID for an AP item, if applicable
 *
 * @param ap_item_id The Archipelago item ID
 * @return The game item ID if this is a GameItemReward, nullopt otherwise
 */
[[nodiscard]] std::optional<uint8_t> getGameItemId(int64_t ap_item_id);

} // namespace ap
