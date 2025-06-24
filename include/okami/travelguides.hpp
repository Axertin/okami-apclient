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

const char *GetName(unsigned value);

} // namespace TravelGuides
} // namespace okami
