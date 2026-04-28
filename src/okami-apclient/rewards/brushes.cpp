#include "brushes.hpp"

#include <array>
#include <cstdint>

#include <wolf_framework.hpp>

#include "../gamestate_accessors.hpp"

namespace rewards::brushes
{

namespace
{

// Progressive brush upgrade bit indices (in brushUpgrades bitfield)
// Power Slash: base brush_index = 12 (BrushOverlay::power_slash)
// - Power Slash 2: bit 0
// - Power Slash 3: bit 10
constexpr std::array<uint32_t, 2> kPowerSlashUpgrades = {0, 10};

// Cherry Bomb: base brush_index = 25 (BrushOverlay::cherry_bomb)
// - Cherry Bomb 2: bit 6
// - Cherry Bomb 3: bit 11
constexpr std::array<uint32_t, 2> kCherryBombUpgrades = {6, 11};

// The brush bitfields (BrushData source + WorldStateData copies + brushUpgrades)
// are addressed by the game with LSB-first-within-byte semantics: bit X means
// `bytes[X/8] & (1 << (X%8))`. The C++ BitField<N> struct uses MSB-first within
// each 32-bit word, which after little-endian storage maps bit X to a different
// physical bit. Use these helpers when touching any brush bitfield so we stay
// in the game's convention.
template <typename Accessor> void setGameBit(Accessor &accessor, unsigned int bitIdx)
{
    auto *bytes = reinterpret_cast<volatile uint8_t *>(accessor.get_ptr());
    bytes[bitIdx / 8] |= static_cast<uint8_t>(1u << (bitIdx % 8));
}

template <typename Accessor> bool isGameBitSet(const Accessor &accessor, unsigned int bitIdx)
{
    const auto *bytes = reinterpret_cast<const volatile uint8_t *>(accessor.get_ptr());
    return (bytes[bitIdx / 8] & static_cast<uint8_t>(1u << (bitIdx % 8))) != 0;
}

/**
 * @brief Grant a simple (non-progressive) brush technique
 *
 * Writes to both the BrushData source and the WorldStateData copy. The game
 * propagates BrushData -> WorldStateData on sync, so writing only the copy
 * gets clobbered. BrushMan's monitor must be muted while this runs (see
 * RewardMan -> CheckMan::enableSending wiring).
 */
void grantSimpleBrush(int brushIndex)
{
    setGameBit(apgame::usableBrushesSource, static_cast<unsigned int>(brushIndex));
    setGameBit(apgame::obtainedBrushesSource, static_cast<unsigned int>(brushIndex));
    setGameBit(apgame::usableBrushTechniques, static_cast<unsigned int>(brushIndex));
    setGameBit(apgame::obtainedBrushTechniques, static_cast<unsigned int>(brushIndex));
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
    // Check if base brush is already obtained (source is authoritative)
    bool hasBase = isGameBitSet(apgame::obtainedBrushesSource, static_cast<unsigned int>(brushIndex));

    if (!hasBase)
    {
        grantSimpleBrush(brushIndex);
        return {};
    }

    // Find and grant next upgrade
    for (size_t i = 0; i < upgrades.size(); ++i)
    {
        uint32_t upgradeBit = upgrades[i];
        if (!isGameBitSet(apgame::brushUpgrades, upgradeBit))
        {
            setGameBit(apgame::brushUpgrades, upgradeBit);
            return {};
        }
    }

    return {}; // Already at max level
}

} // anonymous namespace

std::expected<void, RewardError> grant(int64_t apItemId)
{
    int brushIndex = getBrushIndex(apItemId);

    wolf::logDebug("Granting brush with AP ID 0x%X, brush index 0x%X", apItemId, brushIndex);

    if (apItemId == 0x10C) // Power Slash (bit 12)
    {
        return grantProgressiveBrush(brushIndex, kPowerSlashUpgrades);
    }
    if (apItemId == 0x119) // Cherry Bomb (bit 25)
    {
        return grantProgressiveBrush(brushIndex, kCherryBombUpgrades);
    }

    // Simple brush
    grantSimpleBrush(brushIndex);
    return {};
}

} // namespace rewards::brushes
