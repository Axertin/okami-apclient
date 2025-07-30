#pragma once
#include <cstdint>

namespace okami
{
/**
 * @brief Enum representing all brush techniques by bit index across 4 bytes.
 *
 * Bit values represent contiguous bits starting from bit 0 (lowest bit of byte
 * 0), up through bit 31 (highest bit of byte 3).
 */
enum class BrushOverlay : uint8_t
{
    // Byte 0
    sunrise_default = 1, // Set by default? both it and bit 27 must be set to use Sunrise
    greensprout = 2,
    dot_trees = 3,
    bloom = 4,
    water_lily = 5,
    galestorm = 6,
    whirlwind = 7,

    // Byte 1
    thunder_storm = 8,
    thunderbolt = 9,
    inferno = 10,
    fireburst = 11,
    power_slash = 12,
    waterspout = 13,
    deluge = 14,
    fountain = 15,

    // Byte 2
    veil_of_mist = 16,
    mist_warp = 17,
    crescent = 18,
    vine_base = 19,
    vine_holy_smoke = 20,
    rejuvenation = 22,
    blizzard = 23,

    // Byte 3
    icestorm = 24,
    cherry_bomb = 25,
    sunrise = 27, // Set during story. both it and bit 1 must be set to use Sunrise
    catwalk = 30
};

} // namespace okami
