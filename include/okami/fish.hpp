#pragma once

namespace okami
{
namespace FishTome
{
/**
 * @brief Enum representing all fish.
 */
enum Enum
{
    RiverCrab,
    Crawfish,
    BlackBass,
    Killifish,
    Smelt,
    Goby,
    Sweetfish,
    Trout,
    Catfish,
    Loach,
    FreshwaterEel,
    Huchen,
    Robalo,
    Koi,
    Salmon,
    Sturgeon,
    GiantCatfish,
    MountainTrout,
    CutlassFish,
    GiantSalmon,
    Whopper,
    Starfish,
    Moray,
    LoggerheadTurtle,
    Sunfish,
    Nautilus,
    Clownfish,
    Scallop,
    SeaHorse,
    Blowfish,
    Monkfish,
    Octopus,
    Yellowtail,
    FlyingFish,
    Squid,
    Bonito,
    RedSnapper,
    Lobster,
    StripedSnapper,
    SupremeTuna,
    Manta,
    Oarfish,
    Marlin,
    NUM_FISH_ENTRIES
};
const char *GetName(unsigned index);
} // namespace FishTome
} // namespace okami
