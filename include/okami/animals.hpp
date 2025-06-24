#pragma once
#include <cstdint>

namespace okami
{
/**
 * @brief Enum representing all animals by bit index across 4 bytes.
 *
 * Bit values represent contiguous bits starting from bit 0 (lowest bit of byte
 * 0), up through bit 31 (highest bit of byte 3).
 */
enum class AnimalOverlay : uint8_t
{
    // Byte 0
    // All unused

    // Byte 1
    crane = 12,
    sheep = 13,
    bear = 14,
    cow = 15,

    // Byte 2
    cat = 16,
    mouse = 17,
    tiger = 18,
    horse = 19,
    raccoon = 20,
    fox = 21,
    nightingale = 22,
    deer = 23,

    // Byte 3
    pig = 24,
    monkey = 25,
    boar_piglet = 26,
    boar = 27,
    rabbit = 28,
    dog = 29,
    chicken = 30,
    sparrow = 31
};
} // namespace okami
