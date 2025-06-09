#pragma once
#include <cstdint>
namespace okami
{
    /**
     * @brief Enum representing all dojo techniques by bit index across 4 bytes.
     *
     * Bit values represent contiguous bits starting from bit 0 (lowest bit of byte 0),
     * up through bit 31 (highest bit of byte 3).
     */
    enum class DojoOverlay : uint8_t
    {
        // Byte 1 (unused)

        // Byte 2
        holy_falcon = 12,
        hard_head = 13,
        brown_rage = 14,
        golden_fury = 15,

        // Byte 3
        holy_eagle = 16, // doesn't effect double-jump directly
        digging_champ = 18,
        counter_dodge = 19,
        fleet_foot = 20,
        sword_dance = 21,
        bead_string = 22,
        wailing_mirror = 23,

        // Byte 4
        four_shears = 24,
        three_shears = 25,
        spirit_armageddon = 27,
        spirit_storm = 28,
        five_winds = 29,
        four_winds = 30
    };
} // namespace okami
