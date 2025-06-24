#pragma once
#include <cstdint>

namespace okami
{
/**
 * @brief Enum representing all travel guides by bit index across 4 bytes.
 *
 * Bit values represent contiguous bits starting from bit 0 (lowest bit of byte
 * 0), up through bit 31 (highest bit of byte 3).
 */
enum class TravelGuideOverlay : uint8_t
{
    // Byte 0
    travel_tips = 0,
    greensprout_tips = 1,
    digging_tips = 2,
    fleeing_battle = 3,
    feeding = 4,
    legend_of_orochi = 5,
    enhancing_weapons = 6,
    battle_tips = 7,

    // Byte 1
    enhancing_divinity = 8,
    godhood_tips = 9,
    ink_bullet_tips = 10,
    brush_tips = 11,
    mother_tree = 12,
    land_of_the_gods = 13,
    celestial_envoy = 14,
    veil_of_mist_tech = 15,

    // Byte 2
    holy_artifacts = 16,
    northern_land = 17,
    inferno_tech = 18,
    mark_of_kabegami = 19,
    galestorm_tech = 20,
    power_slash_2 = 21,
    cherry_bomb_2 = 22,
    waterspout_tech_1 = 23,

    // Byte 3
    waterspout_tech_2 = 24,
    thunderstorm_tech = 25,
    another_civilization = 26,
    cherry_bomb_3 = 27,
    tribe_of_the_moon = 28,
    power_slash_3 = 29
};
} // namespace okami
