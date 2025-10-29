#pragma once

namespace okami
{
namespace TravelGuides
{
/**
 * @brief Enum representing all travel guides.
 */
enum Enum
{
    TravelTips,
    GreensproutTips,
    DiggingTips,
    FleeingBattle,
    Feeding,
    LegendOfOrochi,
    EnhancingWeapons,
    BattleTips,
    EnhancingDivinity,
    GodhoodTips,
    InkBulletTips,
    BrushTips,
    MotherTree,
    LandOfTheGods,
    CelestialEnvoy,
    VeilOfMistTech,
    HolyArtifacts,
    NorthernLand,
    InfernoTech,
    MarkOfKabegami,
    GalestormTech,
    PowerSlash2,
    CherryBomb2,
    WaterspoutTech1,
    WaterspoutTech2,
    ThunderstormTech,
    AnotherCivilization,
    CherryBomb3,
    TribeOfTheMoon,
    PowerSlash3,
    NUM_TRAVEL_GUIDES
};

static const char *names[TravelGuides::NUM_TRAVEL_GUIDES] = {
    "Travel Tips",       "Greensprout Tips",  "Digging Tips",         "Fleeing Battle",    "Feeding",           "Legend of Orochi",
    "Enhancing Weapons", "Battle Tips",       "Enhancing Divinity",   "Godhood Tips",      "Ink Bullet Tips",   "Brush Tips",
    "Mother Tree",       "Land of The Gods",  "Celestial Envoy",      "Veil of Mist Tech", "Holy Artifacts",    "Northern Land",
    "Inferno Tech",      "Mark of Kabegami",  "Galestorm Tech",       "Power Slash 2",     "Cherry Bomb 2",     "Waterspout Tech 1",
    "Waterspout Tech 2", "Thunderstorm Tech", "Another Civilization", "Cherry Bomb 3",     "Tribe of the Moon", "Power Slash 3",
};

inline const char *GetName(unsigned value)
{
    if (value < TravelGuides::NUM_TRAVEL_GUIDES)
    {
        return names[value];
    }
    return "invalid";
}

} // namespace TravelGuides
} // namespace okami
