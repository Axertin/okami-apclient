#include "brushes.hpp"

#include <array>

#include <wolf_framework.hpp>

#include "../gamestate_accessors.hpp"

namespace rewards::brushes
{

namespace
{

// Progressive brush upgrade bit indices (in brushUpgrades bitfield)
// Power Slash: base brush_index = 2
// - Power Slash 2: bit 0
// - Power Slash 3: bit 10
constexpr std::array<uint32_t, 2> kPowerSlashUpgrades = {0, 10};

// Cherry Bomb: base brush_index = 3
// - Cherry Bomb 2: bit 6
// - Cherry Bomb 3: bit 11
constexpr std::array<uint32_t, 2> kCherryBombUpgrades = {6, 11};

/**
 * @brief Grant a simple (non-progressive) brush technique
 */
void grantSimpleBrush(int brushIndex)
{
    apgame::usableBrushTechniques->Set(brushIndex);
    apgame::obtainedBrushTechniques->Set(brushIndex);
}

/**
 * @brief Grant a progressive brush technique or upgrade
 *
 * @param brushIndex The base brush index
 * @param upgrades Array of upgrade bit indices
 * @return Success (always - no error cases)
 */
template <size_t N> std::expected<void, RewardError> grantProgressiveBrush(int brushIndex, const std::array<uint32_t, N> &upgrades)
{
    // Check if base brush is already obtained
    bool hasBase = apgame::obtainedBrushTechniques->IsSet(brushIndex);

    if (!hasBase)
    {
        apgame::usableBrushTechniques->Set(brushIndex);
        apgame::obtainedBrushTechniques->Set(brushIndex);
        return {};
    }

    // Find and grant next upgrade
    for (size_t i = 0; i < upgrades.size(); ++i)
    {
        uint32_t upgradeBit = upgrades[i];
        if (!apgame::brushUpgrades->IsSet(upgradeBit))
        {
            apgame::brushUpgrades->Set(upgradeBit);
            return {};
        }
    }

    return {}; // Already at max level
}

} // anonymous namespace

std::expected<void, RewardError> grant(int64_t apItemId)
{
    int brushIndex = getBrushIndex(apItemId);

    if (apItemId == 0x102)
    {
        return grantProgressiveBrush(brushIndex, kPowerSlashUpgrades);
    }
    if (apItemId == 0x103)
    {
        return grantProgressiveBrush(brushIndex, kCherryBombUpgrades);
    }

    // Simple brush
    grantSimpleBrush(brushIndex);
    return {};
}

} // namespace rewards::brushes
