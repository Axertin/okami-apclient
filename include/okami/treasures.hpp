#pragma once
#include <cstdint>

namespace okami
{
    /**
     * @brief Enum representing all treasures by bit index across 8 bytes.
     *
     * Bit values represent contiguous bits starting from bit 0 (lowest bit of byte 0),
     * up through bit 63 (highest bit of byte 7).
     */
    enum class TreasureOverlay : uint8_t
    {
        // Byte 0
        emerald_tassels = 0,
        sapphire_tassels = 1,
        ruby_tassels = 2,
        turqoise_tassels = 3,
        agate_tassels = 4,
        cats_eye_tassels = 5,
        jade_tassels = 6,
        amber_tassels = 7,

        // Byte 1
        amethyst_tassels = 8,
        pearl = 9,
        crystal = 10,
        cat_statue = 11,
        boar_statue = 12,
        dog_statue = 13,
        rooster_statue = 14,
        monkey_statue = 15,

        // Byte 2
        sheep_statue = 16,
        horse_statue = 17,
        snake_statue = 18,
        dragon_statue = 19,
        rabbit_statue = 20,
        tiger_statue = 21,
        bull_statue = 22,
        rat_statue = 23,

        // Byte 3
        bull_horn = 24,
        lacquerware_set = 25,
        incense_burner = 26,
        vase = 27,
        coral_fragment = 28,
        wooden_bear = 29,
        dragonfly_bead = 30,
        glass_beads = 31,

        // Byte 4
        // All unused
        // Byte 5
        // All unused
        // Byte 6
        // All unused

        // Byte 7
        silver_pocket_watch = 60,
        etched_glass = 61,
        white_porcelain_pot = 62,
        kutani_pottery = 63
    };
} // namespace okami
