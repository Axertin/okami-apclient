#pragma once
namespace okami
{
namespace DojoTechs
{
/**
 * @brief Enum representing all dojo techniques.
 */
enum Enum
{
    Unknown0, // None?
    FourWinds,
    FiveWinds,
    SpiritStorm,
    SpiritArmageddon,
    Unknown5,
    ThreeShears,
    FourShears,
    WailingMirror,
    BeadString,
    SwordDance,
    FleetFoot,
    CounterDodge,
    DiggingChamp,
    Unknown14,
    HolyEagle,
    GoldenFury,
    BrownRage,
    HardHead,
    HolyFalcon,
    NUM_DOJO_TECHS
};

static const char *names[DojoTechs::NUM_DOJO_TECHS] = {
    "Unknown0",
    "Reflector Technique: 4 Winds",
    "Reflector Technique: 5 Winds",
    "Rosary Technique: Spirit Storm",
    "Rosary Technique: Spirit Armageddon",
    "Unknown5",
    "Glaive Technique: 3 Shears",
    "Glaive Technique: 4 Shears",
    "God Technique: Wailing Mirror",
    "God Technique: Bead String",
    "God Technique: Sword Dance",
    "God Technique: Fleetfoot",
    "God Technique: Counter Dodge",
    "God Technique: Digging Champ",
    "Unknown14",
    "God Technique: Holy Eagle",
    "God Technique: Golden Fury",
    "God Technique: Brown Rage",
    "God Technique: Hardhead",
    "God Technique: Holy Falcon",
};

inline const char *GetName(unsigned value)
{
    if (value < DojoTechs::NUM_DOJO_TECHS)
    {
        return names[value];
    }
    return "invalid";
}
} // namespace DojoTechs
} // namespace okami
