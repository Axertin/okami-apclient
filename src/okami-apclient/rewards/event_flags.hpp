#pragma once

#include <cstdint>
#include <expected>

#include "reward_types.hpp"

/**
 * @brief Event flag reward handler
 *
 * Handles setting event flags in game state (0x303-0x308).
 * These are used for quest progression flags like "Save Rei", "Serpent Crystal", etc.
 */
namespace rewards::event_flags
{

/**
 * @brief Extract flag bit index from AP item ID
 *
 * @param apItemId The Archipelago item ID (must be 0x303-0x308)
 * @return The bit index in keyItemsAcquired
 */
[[nodiscard]] constexpr uint32_t getFlagBit(int64_t apItemId)
{
    return static_cast<uint32_t>(apItemId - kEventFlagBase);
}

/**
 * @brief Grant an event flag reward
 *
 * Sets the appropriate bit in the keyItemsAcquired bitfield.
 *
 * @param apItemId The Archipelago item ID
 * @return Success or error
 */
[[nodiscard]] std::expected<void, RewardError> grant(int64_t apItemId);

} // namespace rewards::event_flags
