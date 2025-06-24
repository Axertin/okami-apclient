#pragma once
#include <cstdint>

namespace okami
{
/**
 * @brief Enum representing all fish by bit index across 8 bytes.
 *
 * Bit values represent contiguous bits starting from bit 0 (lowest bit of byte
 * 0), up through bit 63 (highest bit of byte 7).
 */
enum class FishOverlay : uint8_t
{
    // Byte 0
    octopus = 0,
    monkfish = 1,
    blowfish = 2,
    sea_horse = 3,
    scallop = 4,
    clownfish = 5,
    nautilus = 6,
    sunfish = 7,

    // Byte 1
    loggerhead_turtle = 8,
    moray = 9,
    starfish = 10,
    whopper = 11,
    giant_salmon = 12,
    cutlass_fish = 13,
    mountain_trout = 14,
    giant_catfish = 15,

    // Byte 2
    sturgeon = 16,
    salmon = 17,
    koi = 18,
    robalo = 19,
    huchen = 20,
    freshwater_eel = 21,
    loach = 22,
    catfish = 23,

    // Byte 3
    trout = 24,
    sweetfish = 25,
    goby = 26,
    smelt = 27,
    killifish = 28,
    black_bass = 29,
    crawfish = 30,
    river_crab = 31,

    // Byte 4
    // Unused

    // Byte 5
    // Unused

    // Byte 6
    marlin = 53,
    oarfish = 54,
    manta = 55,

    // Byte 7
    supreme_tuna = 56,
    striped_snapper = 57,
    lobster = 58,
    red_snapper = 59,
    bonito = 60,
    squid = 61,
    flying_fish = 62,
    yellowtail = 63
};
} // namespace okami
