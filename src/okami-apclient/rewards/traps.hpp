#pragma once

#include <cstdint>
#include <expected>

#include "reward_types.hpp"

/**
 * @brief Trap reward handler
 *
 * Handles trap effects (0x400-0x4FF), which apply negative effects to the
 * player: Evil Charm (damage), Dry Inkwell (ink drain), Hungry Spirit (food drain).
 */
namespace rewards::traps
{

inline constexpr int64_t kTrapDamage = 0x400;   // Evil Charm:    deal 50% max HP (never lethal)
inline constexpr int64_t kTrapInkLoss = 0x401;  // Dry Inkwell:   drain all current ink
inline constexpr int64_t kTrapFoodLoss = 0x402; // Hungry Spirit: drain the Astral Pouch (food)

/**
 * @brief Grant a trap reward
 *
 * Applies the trap's negative effect based on the AP item ID.
 *
 * @param apItemId The Archipelago item ID (0x400-0x4FF)
 * @return Success or error
 */
[[nodiscard]] std::expected<void, RewardError> grant(int64_t apItemId);

} // namespace rewards::traps
