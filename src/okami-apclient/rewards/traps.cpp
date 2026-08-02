#include "traps.hpp"

#include <cinttypes>
#include <cstdint>

#include <wolf_framework.hpp>

#include "../gamestate_accessors.hpp"
#include "../ui/notificationwindow.h"

namespace rewards::traps
{

namespace
{

/**
 * @brief Deal 50% of max HP, floored at 1 so it never kills
 */
void applyDamageTrap()
{
    auto *stats = apgame::characterStats.get_ptr();

    uint16_t damage = stats->maxHealth / 2;
    if (damage < 1)
        damage = 1;

    stats->currentHealth = (stats->currentHealth > damage) ? static_cast<uint16_t>(stats->currentHealth - damage) : 1;
    wolf::logInfo("[traps] Evil Charm: HP reduced by %u to %u", damage, stats->currentHealth);

    notificationwindow::queue("[Trap] Evil Charm: lost half your health");
}

/**
 * @brief Drain all current ink
 */
void applyInkLossTrap()
{
    apgame::collectionData->currentInk = 0;
    wolf::logInfo("[traps] Dry Inkwell: ink drained");

    notificationwindow::queue("[Trap] Dry Inkwell: ink drained");
}

/**
 * @brief Drain the Astral Pouch (food) reserve
 */
void applyFoodLossTrap()
{
    apgame::characterStats->currentFood = 0;
    wolf::logInfo("[traps] Hungry Spirit: food drained");

    notificationwindow::queue("[Trap] Hungry Spirit: Astral Pouch emptied");
}

} // namespace

std::expected<void, RewardError> grant(int64_t apItemId)
{
    switch (apItemId)
    {
    case kTrapDamage:
        applyDamageTrap();
        break;
    case kTrapInkLoss:
        applyInkLossTrap();
        break;
    case kTrapFoodLoss:
        applyFoodLossTrap();
        break;
    default:
        wolf::logWarning("[traps] Unknown trap ID 0x%" PRIX64, apItemId);
        break;
    }
    return {};
}

} // namespace rewards::traps
