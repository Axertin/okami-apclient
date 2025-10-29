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

static const char *names[NUM_BRUSH_TYPES] = {
    "Unknown0",     "Catwalk",     "Unknown2",          "Unknown3",    "Sunrise (Kamiki)", "Unknown5",      "Cherry Bomb",  "Ice Storm", "Blizzard",
    "Rejuvenation", "Unknown10",   "Vine (Holy Smoke)", "Vine (Base)", "Crescent",         "Mist Warp",     "Veil of Mist", "Fountain",  "Deluge",
    "Waterspout",   "Power Slash", "Fireburst",         "Inferno",     "Thunderbolt",      "Thunder Storm", "Whirlwind",    "Galestorm", "Water Lily",
    "Bloom",        "Dot Trees",   "Greensprout",       "Sunrise",     "Unknown31",
};

inline const char *GetName(unsigned index)
{
    if (index < NUM_BRUSH_TYPES)
    {
        return names[index];
    }
    return "invalid";
}
} // namespace BrushTypes
} // namespace okami
