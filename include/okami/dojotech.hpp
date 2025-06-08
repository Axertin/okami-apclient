#pragma once
#include <cstdint>
#include "bitfieldflags.hpp"

namespace okami
{
    enum class DojoByte1 : uint8_t
    {
        // All unknown or unused
    };

    enum class DojoByte2 : uint8_t
    {
        holy_falcon = 1 << 4,
        hard_head = 1 << 5,
        brown_rage = 1 << 6,
        golden_fury = 1 << 7,
    };

    enum class DojoByte3 : uint8_t
    {
        holy_eagle = 1 << 0, // Doesn't affect double jump directly (main + )
        digging_champ = 1 << 2,
        counter_dodge = 1 << 3,
        fleet_foot = 1 << 4,
        sword_dance = 1 << 5,
        bead_string = 1 << 6,
        wailing_mirror = 1 << 7,
    };

    enum class DojoByte4 : uint8_t
    {
        four_shears = 1 << 0,
        three_shears = 1 << 1,
        spirit_armageddon = 1 << 3,
        spirit_storm = 1 << 4,
        five_winds = 1 << 5,
        four_winds = 1 << 6,
    };

#pragma pack(push, 1)
    struct UsableDojoOverlay
    {
        BitfieldFlags<DojoByte1> byte1;
        BitfieldFlags<DojoByte2> byte2;
        BitfieldFlags<DojoByte3> byte3;
        BitfieldFlags<DojoByte4> byte4;
    };
#pragma pack(pop)

    inline UsableDojoOverlay *AmmyUsableDojoTechniques;
} // namespace okami
