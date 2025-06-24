#pragma once

namespace okami
{
namespace MoveListTome
{
enum Enum
{
    FourWinds,
    FiveWinds,
    SpiritStorm,
    SpiritArmageddon,
    ThreeShears,
    FourShears,
    WailingMirror,
    BeadString,
    SwordDance,
    Fleetfoot,
    CounterDodge,
    DiggingChamp,
    HolyEagle,
    GoldenFury,
    BrownRage,
    Hardhead,
    HolyFalcon,
    NUM_MOVE_LIST_ENTRIES
};

const char *GetName(unsigned index);
} // namespace MoveListTome
} // namespace okami
