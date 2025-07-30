#pragma once

// Source: https://okami.speedruns.wiki/Bitfield_Table

namespace okami
{
namespace BrushTypes
{
enum Enum
{
    Unknown1,
    Catwalk,
    Unknown2,
    Unknown3,
    SunriseKamiki,
    Unknown4,
    CherryBomb,
    IceStorm,
    Blizzard,
    Rejuvenation,
    Unknown5,
    VineHolySmoke,
    VineBase,
    Crescent,
    MistWarp,
    VeilOfMist,
    Fountain,
    Deluge,
    Waterspout,
    PowerSlash,
    Fireburst,
    Inferno,
    Thunderbolt,
    ThunderStorm,
    Whirlwind,
    Galestorm,
    WaterLily,
    Bloom,
    DotTrees,
    Greensprout,
    Sunrise,
    Unknown6,
    NUM_BRUSH_TYPES
};
const char *GetName(unsigned index);
} // namespace BrushTypes
} // namespace okami
