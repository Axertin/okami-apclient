#pragma once
#include <cstdint>
#include "bitfieldflags.hpp"

namespace okami
{
    enum class BrushByte1 : uint8_t
    {
        // unknown = 1 << 0,
        sunrise = 1 << 1,
        greensprout = 1 << 2,
        dot_trees = 1 << 3,
        bloom = 1 << 4,
        water_lily = 1 << 5,
        galestorm = 1 << 6,
        whirlwind = 1 << 7,
    };

    enum class BrushByte2 : uint8_t
    {
        thunder_storm = 1 << 0,
        thunderbolt = 1 << 1,
        inferno = 1 << 2,
        fireburst = 1 << 3,
        power_slash = 1 << 4,
        watersprout = 1 << 5,
        deluge = 1 << 6,
        fountain = 1 << 7,
    };

    enum class BrushByte3 : uint8_t
    {
        veil_of_mist = 1 << 0,
        mist_warp = 1 << 1,
        crescent = 1 << 2,
        vine_base = 1 << 3,
        vine_holy_smoke = 1 << 4,
        // unknown = 1 << 5,
        rejuvenation = 1 << 6,
        blizzard = 1 << 7,
    };

    enum class BrushByte4 : uint8_t
    {
        icestorm = 1 << 0,
        cherry_bomb = 1 << 1,
        // unknown = 1 << 2,
        sunrise = 1 << 3, // doesn't allow sunrise use by itself, but is set when getting sunrise
        // unknown = 1 << 4,
        // unknown = 1 << 5,
        catwalk = 1 << 6,
        // unknown = 1 << 7,
    };

#pragma pack(push, 1)
    struct ObtainedBrushOverlay
    {
        BitfieldFlags<BrushByte1> byte1;
        BitfieldFlags<BrushByte2> byte2;
        BitfieldFlags<BrushByte3> byte3;
        BitfieldFlags<BrushByte4> byte4;
    };
#pragma pack(pop)

#pragma pack(push, 1)
    struct UsableBrushOverlay
    {
        BitfieldFlags<BrushByte1> byte1;
        BitfieldFlags<BrushByte2> byte2;
        BitfieldFlags<BrushByte3> byte3;
        BitfieldFlags<BrushByte4> byte4;
    };
#pragma pack(pop)

    inline ObtainedBrushOverlay *AmmyObtainedBrushTechniques;
    inline UsableBrushOverlay *AmmyUsableBrushTechniques;
} // namespace okami
