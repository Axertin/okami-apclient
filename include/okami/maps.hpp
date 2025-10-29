
#pragma once
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>
#include <unordered_map>

namespace okami
{
namespace main
{
/// Current exterior map ID (Loading zone sequence 3)
constexpr uintptr_t exteriorMapID = 0xB6B240; // uint16_t

/// Current active map ID (Loading zone sequence 1)
constexpr uintptr_t currentMapID = 0xB65E74; // uint16_t

/// Legacy map ID references
constexpr uintptr_t vestigialMapID1 = 0xB4F0B4; // uint16_t
constexpr uintptr_t vestigialMapID2 = 0xB6B246; // uint16_t (Loading zone sequence 2)

/// Map ID references (additional)
constexpr uintptr_t exteriorMapIDCopy = 0xB6B248; // uint16_t (lastMapId reference)
} // namespace main
} // namespace okami

namespace MapID
{
enum Enum
{
    EndlessLoadingScreen = 0x6,
    PrototypeIntroCutscene2005 = 0x7,
    ItemTestMap1 = 0x10,
    ItemTestMap2 = 0x11,
    BrushInteractionTestMap1 = 0x20,
    BrushInteractionTestMap2 = 0x21,
    BrushInteractionTestMap3 = 0x30,
    BrushInteractionTestMap4 = 0x31,
    BrushInteractionTestMap5 = 0x32,
    BrushInteractionTestMap6 = 0x33,
    BrushInteractionTestMap7 = 0x34,
    CollisionAndMovementTestMap = 0xFF,
    KamikiVillageCursed = 0x100,
    CaveofNagi = 0x101,
    KamikiVillage = 0x102,
    HanaValley = 0x103,
    TsutaRuins = 0x104,
    CityCheckpoint = 0x105,
    TsutaRuinsSpiderQueenArena = 0x106,
    GaleShrine = 0x107,
    KusaVillage = 0x108,
    SasaSanctuary = 0x109,
    AgataForestMadameFawnsHouse = 0x10A,
    DiggingMinigame = 0x10B,
    OnigiriDojoLessonRoom = 0x10C,
    GaleShrineCrimsonHelmArena = 0x10D,
    CalcifiedCavern = 0x10E,
    MoonCaveInterior = 0x110,
    MoonCaveStaircaseAndOrochiArena = 0x111,
    KamikiVillagePostTei = 0x112,
    RyoshimaCoastBanditSpiderArena = 0x113,
    TakaPassStrayBead26Cave = 0x114,
    KamuiCherryBomb3Cave = 0x115,
    NorthRyoshimaCoastPowerSlash2Cave = 0x116,
    NorthRyoshimaCoastCherryBomb2Cave = 0x117,
    KamuiPowerSlash3Cave = 0x118,
    KamuiBlockheadGrandeCave = 0x119,
    NorthRyoshimaCoastBanditSpiderArena = 0x11A,
    KamuiBanditSpiderArena = 0x11B,
    DragonPalaceStrayBeadCave = 0x11C,
    NorthRyoshimaCoastStrayBeadCave = 0x11D,
    NewGameIntroCutscene = 0x120,
    BetaKamiki = 0x121,
    RiveroftheHeavens = 0x122,
    SeianCityAristocraticQuarter = 0x200,
    SeianCityCommonersQuarter = 0x201,
    SeianCityAristocraticQuarterHimikosPalace = 0x202,
    DragonPalace = 0x203,
    InsidetheDragon = 0x204,
    SunkenShip = 0x205,
    ImperialPalaceAmmySize = 0x206,
    ImperialPalaceIssunSize = 0x207,
    OniIslandLowerInterior = 0x208,
    OniIslandNinetailsArena = 0x209,
    CatcallTower = 0x20A,
    ImperialPalaceBlightArena = 0x20B,
    SeianCityCommonersQuarterKimonoShop = 0x20C,
    OniIslandExterior = 0x20D,
    OniIslandUpperInterior = 0x20E,
    OniIslandSidescroller = 0x20F,
    Wepkeer = 0x301,
    KamikiVillagePast = 0x302,
    WawkuShrine = 0x303,
    WawkuShrineNechkuArena = 0x304,
    Ponctan = 0x305,
    MoonCavePast = 0x306,
    ArkofYamato = 0x307,
    ArkofYamatoOrochiArena = 0x308,
    ArkofYamatoBlightArena = 0x309,
    ArkofYamatoNinetailsArena = 0x30A,
    ArkofYamatoCrimsonHelmArena = 0x30B,
    PonctanMrsSealsHouse = 0x30C,
    Yoshpet = 0x310,
    InnerYoshpet = 0x311,
    ArkofYamatoYamiArena = 0x312,
    WepkeerSquare = 0x313,
    WawkuShrineLechkuAndNechkuArena = 0x314,
    TitleScreen = 0xC00,
    Unknown = 0xC01,
    PresentsfromIssun = 0xC02,
    BetaShinshu = 0xD00,
    BetaHana = 0xD01,
    BetaTsuta = 0xD02,
    BetaAgata = 0xD03,
    BetaRyoshima = 0xD04,
    BetaKamui = 0xD05,
    BetaTaka = 0xD06,
    TitleScreenDemoCutscene = 0xD07,
    FishingSeianBridge = 0xE00,
    FishingHimikosPalace = 0xE01,
    FishingAgata = 0xE02,
    FishingKamui = 0xE03,
    FishingNorthRyoshima = 0xE04,
    ShinshuFieldCursed = 0xF01,
    ShinshuFieldHealed = 0xF02,
    AgataForestCursed = 0xF03,
    AgataForestHealed = 0xF04,
    MoonCaveEntrance = 0xF06,
    TakaPassCursed = 0xF07,
    TakaPassHealed = 0xF08,
    RyoshimaCoastCursed = 0xF09,
    RyoshimaCoastHealed = 0xF0A,
    NRyoshimaCoast = 0xF0C,
    KamuiCursed = 0xF11,
    KamuiHealed = 0xF12,
    KamuiEzofuji = 0xF13,
    ShinshuFieldPast = 0xF20,
    MoonCaveEntrancePast = 0xF21,
};
} // namespace MapID

namespace okami
{
inline static const std::unordered_map<uint16_t, std::string> MapNames = {
    {MapID::EndlessLoadingScreen, "Endless loading screen"},
    {MapID::PrototypeIntroCutscene2005, "2005 Prototype Intro Cutscene"},
    {MapID::ItemTestMap1, "Item Test Map #1"},
    {MapID::ItemTestMap2, "Item Test Map #2"},
    {MapID::BrushInteractionTestMap1, "Brush Interaction Test Map #1"},
    {MapID::BrushInteractionTestMap2, "Brush Interaction Test Map #2"},
    {MapID::BrushInteractionTestMap3, "Brush Interaction Test Map #3"},
    {MapID::BrushInteractionTestMap4, "Brush Interaction Test Map #4"},
    {MapID::BrushInteractionTestMap5, "Brush Interaction Test Map #5"},
    {MapID::BrushInteractionTestMap6, "Brush Interaction Test Map #6"},
    {MapID::BrushInteractionTestMap7, "Brush Interaction Test Map #7"},
    {MapID::CollisionAndMovementTestMap, "Collision and Movement Test Map"},
    {MapID::KamikiVillageCursed, "Kamiki Village - Cursed"},
    {MapID::CaveofNagi, "Cave of Nagi"},
    {MapID::KamikiVillage, "Kamiki Village"},
    {MapID::HanaValley, "Hana Valley"},
    {MapID::TsutaRuins, "Tsuta Ruins"},
    {MapID::CityCheckpoint, "City Checkpoint"},
    {MapID::TsutaRuinsSpiderQueenArena, "Tsuta Ruins - Spider Queen Arena"},
    {MapID::GaleShrine, "Gale Shrine"},
    {MapID::KusaVillage, "Kusa Village"},
    {MapID::SasaSanctuary, "Sasa Sanctuary"},
    {MapID::AgataForestMadameFawnsHouse, "Agata Forest - Madame Fawn's House"},
    {MapID::DiggingMinigame, "Digging Minigame"},
    {MapID::OnigiriDojoLessonRoom, "Onigiri Dojo Lesson Room"},
    {MapID::GaleShrineCrimsonHelmArena, "Gale Shrine - Crimson Helm Arena"},
    {MapID::CalcifiedCavern, "Calcified Cavern"},
    {MapID::MoonCaveInterior, "Moon Cave - Interior"},
    {MapID::MoonCaveStaircaseAndOrochiArena, "Moon Cave - Staircase and Orochi Arena"},
    {MapID::KamikiVillagePostTei, "Kamiki Village - Post-Tei"},
    {MapID::RyoshimaCoastBanditSpiderArena, "Ryoshima Coast - Bandit Spider Arena"},
    {MapID::TakaPassStrayBead26Cave, "Taka Pass - Stray Bead #26 Cave"},
    {MapID::KamuiCherryBomb3Cave, "Kamui - Cherry Bomb 3 Cave"},
    {MapID::NorthRyoshimaCoastPowerSlash2Cave, "North Ryoshima Coast - Power Slash 2 Cave"},
    {MapID::NorthRyoshimaCoastCherryBomb2Cave, "North Ryoshima Coast - Cherry Bomb 2 Cave"},
    {MapID::KamuiPowerSlash3Cave, "Kamui - Power Slash 3 Cave"},
    {MapID::KamuiBlockheadGrandeCave, "Kamui - Blockhead Grande Cave"},
    {MapID::NorthRyoshimaCoastBanditSpiderArena, "North Ryoshima Coast - Bandit Spider Arena"},
    {MapID::KamuiBanditSpiderArena, "Kamui - Bandit Spider Arena"},
    {MapID::DragonPalaceStrayBeadCave, "Dragon Palace - Stray Bead Cave"},
    {MapID::NorthRyoshimaCoastStrayBeadCave, "North Ryoshima Coast - Stray Bead Cave"},
    {MapID::NewGameIntroCutscene, "New Game Intro Cutscene"},
    {MapID::BetaKamiki, "Beta Kamiki"},
    {MapID::RiveroftheHeavens, "River of the Heavens"},
    {MapID::SeianCityAristocraticQuarter, "Sei-an City (Aristocratic Quarter)"},
    {MapID::SeianCityCommonersQuarter, "Sei-an City (Commoners' Quarter)"},
    {MapID::SeianCityAristocraticQuarterHimikosPalace, "Sei-an City (Aristocratic Quarter) - Himiko's Palace"},
    {MapID::DragonPalace, "Dragon Palace"},
    {MapID::InsidetheDragon, "Inside the Dragon"},
    {MapID::SunkenShip, "Sunken Ship"},
    {MapID::ImperialPalaceAmmySize, "Imperial Palace - Ammy Size"},
    {MapID::ImperialPalaceIssunSize, "Imperial Palace - Issun Size"},
    {MapID::OniIslandLowerInterior, "Oni Island - Lower Interior"},
    {MapID::OniIslandNinetailsArena, "Oni Island - Ninetails Arena"},
    {MapID::CatcallTower, "Catcall Tower"},
    {MapID::ImperialPalaceBlightArena, "Imperial Palace - Blight Arena"},
    {MapID::SeianCityCommonersQuarterKimonoShop, "Sei-an City (Commoners' Quarter) - Kimono Shop"},
    {MapID::OniIslandExterior, "Oni Island - Exterior"},
    {MapID::OniIslandUpperInterior, "Oni Island - Upper Interior"},
    {MapID::OniIslandSidescroller, "Oni Island - Sidescroller"},
    {MapID::Wepkeer, "Wep'keer"},
    {MapID::KamikiVillagePast, "Kamiki Village - Past"},
    {MapID::WawkuShrine, "Wawku Shrine"},
    {MapID::WawkuShrineNechkuArena, "Wawku Shrine - Nechku Arena"},
    {MapID::Ponctan, "Ponc'tan"},
    {MapID::MoonCavePast, "Moon Cave - Past"},
    {MapID::ArkofYamato, "Ark of Yamato"},
    {MapID::ArkofYamatoOrochiArena, "Ark of Yamato - Orochi Arena"},
    {MapID::ArkofYamatoBlightArena, "Ark of Yamato - Blight Arena"},
    {MapID::ArkofYamatoNinetailsArena, "Ark of Yamato - Ninetails Arena"},
    {MapID::ArkofYamatoCrimsonHelmArena, "Ark of Yamato - Crimson Helm Arena"},
    {MapID::PonctanMrsSealsHouse, "Ponc'tan - Mrs. Seal's House"},
    {MapID::Yoshpet, "Yoshpet"},
    {MapID::InnerYoshpet, "Inner Yoshpet"},
    {MapID::ArkofYamatoYamiArena, "Ark of Yamato - Yami Arena"},
    {MapID::WepkeerSquare, "Wep'keer Square"},
    {MapID::WawkuShrineLechkuAndNechkuArena, "Wawku Shrine - Lechku & Nechku Arena"},
    {MapID::TitleScreen, "Title Screen"},
    {MapID::Unknown, "Unknown"},
    {MapID::PresentsfromIssun, "Presents from Issun"},
    {MapID::BetaShinshu, "Beta Shinshu"},
    {MapID::BetaHana, "Beta Hana"},
    {MapID::BetaTsuta, "Beta Tsuta"},
    {MapID::BetaAgata, "Beta Agata"},
    {MapID::BetaRyoshima, "Beta Ryoshima"},
    {MapID::BetaKamui, "Beta Kamui"},
    {MapID::BetaTaka, "Beta Taka"},
    {MapID::TitleScreenDemoCutscene, "Title Screen Demo Cutscene"},
    {MapID::FishingSeianBridge, "Fishing (Sei-an Bridge)"},
    {MapID::FishingHimikosPalace, "Fishing (Himiko's Palace)"},
    {MapID::FishingAgata, "Fishing (Agata)"},
    {MapID::FishingKamui, "Fishing (Kamui)"},
    {MapID::FishingNorthRyoshima, "Fishing (North Ryoshima)"},
    {MapID::ShinshuFieldCursed, "Shinshu Field - Cursed"},
    {MapID::ShinshuFieldHealed, "Shinshu Field - Healed"},
    {MapID::AgataForestCursed, "Agata Forest - Cursed"},
    {MapID::AgataForestHealed, "Agata Forest - Healed"},
    {MapID::MoonCaveEntrance, "Moon Cave Entrance"},
    {MapID::TakaPassCursed, "Taka Pass - Cursed"},
    {MapID::TakaPassHealed, "Taka Pass - Healed"},
    {MapID::RyoshimaCoastCursed, "Ryoshima Coast - Cursed"},
    {MapID::RyoshimaCoastHealed, "Ryoshima Coast - Healed"},
    {MapID::NRyoshimaCoast, "N. Ryoshima Coast"},
    {MapID::KamuiCursed, "Kamui - Cursed"},
    {MapID::KamuiHealed, "Kamui - Healed"},
    {MapID::KamuiEzofuji, "Kamui (Ezofuji)"},
    {MapID::ShinshuFieldPast, "Shinshu Field - Past"},
    {MapID::MoonCaveEntrancePast, "Moon Cave Entrance - Past"},
};

/// @brief Decode Map name from ID
/// @param ID Map ID
/// @return String Map Name
inline std::string decodeMapName(uint16_t ID)
{
    if (auto it = okami::MapNames.find(ID); it != MapNames.end())
    {
        return it->second;
    }

    std::stringstream stream;
    stream << "Unknown Map 0x" << std::hex << ID << std::dec;
    return stream.str();
}
} // namespace okami
