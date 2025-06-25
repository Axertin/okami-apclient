#include "fish.hpp"

namespace okami
{
namespace FishTome
{
static const char *name[NUM_FISH_ENTRIES] = {
    "River Crab",    "Crawfish",       "Black Bass",     "Killifish",    "Smelt",       "Goby",     "Sweetfish",       "Trout",
    "Catfish",       "Loach",          "Freshwater Eel", "Huchen",       "Robalo",      "Koi",      "Salmon",          "Sturgeon",
    "Giant Catfish", "Mountain Trout", "Cutlass Fish",   "Giant Salmon", "Whopper",     "Starfish", "Moray",           "Loggerhead Turtle",
    "Sunfish",       "Nautilus",       "Clownfish",      "Scallop",      "Sea Horse",   "Blowfish", "Monkfish",        "Octopus",
    "Yellowtail",    "Flying Fish",    "Squid",          "Bonito",       "Red Snapper", "Lobster",  "Striped Snapper", "Supreme Tuna",
    "Manta",         "Oarfish",        "Marlin",
};

const char *GetName(unsigned index)
{
    if (index < NUM_FISH_ENTRIES)
    {
        return name[index];
    }
    return "invalid";
}
} // namespace FishTome
} // namespace okami