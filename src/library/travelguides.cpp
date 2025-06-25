#include "okami/travelguides.hpp"

namespace okami
{
namespace TravelGuides
{
static const char *names[TravelGuides::NUM_TRAVEL_GUIDES] = {
    "Travel Tips",       "Greensprout Tips",  "Digging Tips",         "Fleeing Battle",    "Feeding",           "Legend of Orochi",
    "Enhancing Weapons", "Battle Tips",       "Enhancing Divinity",   "Godhood Tips",      "Ink Bullet Tips",   "Brush Tips",
    "Mother Tree",       "Land of The Gods",  "Celestial Envoy",      "Veil of Mist Tech", "Holy Artifacts",    "Northern Land",
    "Inferno Tech",      "Mark of Kabegami",  "Galestorm Tech",       "Power Slash 2",     "Cherry Bomb 2",     "Waterspout Tech 1",
    "Waterspout Tech 2", "Thunderstorm Tech", "Another Civilization", "Cherry Bomb 3",     "Tribe of the Moon", "Power Slash 3",
};

const char *GetName(unsigned value)
{
    if (value < TravelGuides::NUM_TRAVEL_GUIDES)
    {
        return names[value];
    }
    return "invalid";
}
} // namespace TravelGuides
} // namespace okami
