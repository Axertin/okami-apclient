#include "okami/data/brushtype.hpp"

namespace okami
{
namespace BrushTypes
{
static const char *name[NUM_BRUSH_TYPES] = {
    "Unknown0",     "Catwalk",    "Unknown2",          "Unknown3",    "Sunrise (Kamiki)", "Unknown5",      "Cherry Bomb",  "Ice Storm", "Blizzard",
    "Rejuvenation", "Unknown10",  "Vine (Holy Smoke)", "Vine (Base)", "Crescent",         "Mist Warp",     "Veil of Mist", "Fountain",  "Deluge",
    "Watersprout",  "PowerSlash", "Fireburst",         "Inferno",     "Thunderbolt",      "Thunder Storm", "Whirlwind",    "Galestorm", "Water Lily",
    "Bloom",        "Dot Trees",  "Greensprout",       "Sunrise",     "Unknown31",
};

const char *GetName(unsigned index)
{
    if (index < NUM_BRUSH_TYPES)
    {
        return name[index];
    }
    return "invalid";
}
} // namespace BrushTypes
} // namespace okami