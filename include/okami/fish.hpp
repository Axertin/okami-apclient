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
static const char *name[NUM_FISH_ENTRIES] = {
    "River Crab",    "Crawfish",       "Black Bass",     "Killifish",    "Smelt",       "Goby",     "Sweetfish",       "Trout",
    "Catfish",       "Loach",          "Freshwater Eel", "Huchen",       "Robalo",      "Koi",      "Salmon",          "Sturgeon",
    "Giant Catfish", "Mountain Trout", "Cutlass Fish",   "Giant Salmon", "Whopper",     "Starfish", "Moray",           "Loggerhead Turtle",
    "Sunfish",       "Nautilus",       "Clownfish",      "Scallop",      "Sea Horse",   "Blowfish", "Monkfish",        "Octopus",
    "Yellowtail",    "Flying Fish",    "Squid",          "Bonito",       "Red Snapper", "Lobster",  "Striped Snapper", "Supreme Tuna",
    "Manta",         "Oarfish",        "Marlin",
};

inline const char *GetName(unsigned index)
{
    if (index < NUM_FISH_ENTRIES)
    {
        return name[index];
    }
    return "invalid";
}
} // namespace FishTome
} // namespace okami
