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

const char *GetName(unsigned value);
} // namespace DojoTechs
} // namespace okami
