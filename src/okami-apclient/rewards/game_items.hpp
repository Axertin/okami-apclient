#pragma once

#include <cstdint>
#include <expected>
#include <optional>

#include "reward_types.hpp"

/**
 * @brief Game item reward handler
 *
 * Handles granting of:
 * - Regular game items (0x00-0xFF): AP ID == game item ID
 * - Progressive weapons (0x300-0x302): determines next stage based on inventory
 */
namespace rewards::game_items
{

/**
 * @brief Extract game item ID from AP item ID
 *
 * Only valid for regular items (0x00-0xFF).
 * For progressive weapons, use grant() which determines the stage.
 *
 * @param apItemId The Archipelago item ID (must be 0x00-0xFF)
 * @return The game item ID
 */
[[nodiscard]] constexpr uint8_t getItemId(int64_t apItemId)
{
    return static_cast<uint8_t>(apItemId);
}

/**
 * @brief Check if an AP item ID is a progressive weapon
 *
 * @param apItemId The Archipelago item ID
 * @return true if this is a progressive weapon (0x300-0x302)
 */
[[nodiscard]] constexpr bool isProgressiveWeapon(int64_t apItemId)
{
    return apItemId >= kProgressiveWeaponBase && apItemId <= kProgressiveWeaponEnd;
}

/**
 * @brief Check if an AP item ID corresponds to a placeable game item
 *
 * Returns true for items that can be directly placed in containers/inventory.
 * Returns false for progressive weapons (they resolve to different items).
 *
 * @param apItemId The Archipelago item ID
 * @return true if the item ID directly maps to a game item
 */
[[nodiscard]] constexpr bool isDirectGameItem(int64_t apItemId)
{
    return apItemId >= 0 && apItemId <= kGameItemMax;
}

/**
 * @brief Grant a game item reward
 *
 * For regular items (0x00-0xFF): grants the item directly.
 * For progressive weapons (0x300-0x302): determines next stage based on
 * current inventory and grants that stage's item.
 *
 * @param apItemId The Archipelago item ID
 * @return Success or error
 */
[[nodiscard]] std::expected<void, RewardError> grant(int64_t apItemId);

/**
 * @brief Get the game item ID that would be granted for an AP item
 *
 * For regular items: returns the item ID directly.
 * For progressive weapons: determines current stage and returns next item.
 *
 * @param apItemId The Archipelago item ID
 * @return The game item ID that would be granted, or nullopt if at max stage
 */
[[nodiscard]] std::optional<uint8_t> getNextItemToGrant(int64_t apItemId);

} // namespace rewards::game_items
