#pragma once

#include <cstdint>
#include <expected>

#include "reward_types.hpp"

/**
 * @brief Brush technique reward handler
 *
 * Handles granting of:
 * - Simple brush techniques (most): brush_index = AP_ID - 0x100
 * - Progressive brushes (Power Slash 0x102, Cherry Bomb 0x103):
 *   First receipt grants base technique, subsequent receipts grant upgrades
 */
namespace rewards::brushes
{

/**
 * @brief Extract brush index from AP item ID
 *
 * @param apItemId The Archipelago item ID (must be 0x100-0x115)
 * @return The brush technique index
 */
[[nodiscard]] constexpr int getBrushIndex(int64_t apItemId)
{
    return static_cast<int>(apItemId - kBrushBase);
}

/**
 * @brief Check if a brush is progressive (has upgrades)
 *
 * @param apItemId The Archipelago item ID
 * @return true if this is Power Slash (0x102) or Cherry Bomb (0x103)
 */
[[nodiscard]] constexpr bool isProgressive(int64_t apItemId)
{
    return apItemId == 0x102 || apItemId == 0x103;
}

/**
 * @brief Grant a brush technique reward
 *
 * For simple brushes: unlocks the technique.
 * For progressive brushes: if base not owned, grants base technique.
 *                          Otherwise, grants next available upgrade.
 *
 * @param apItemId The Archipelago item ID
 * @return Success or error
 */
[[nodiscard]] std::expected<void, RewardError> grant(int64_t apItemId);

} // namespace rewards::brushes
