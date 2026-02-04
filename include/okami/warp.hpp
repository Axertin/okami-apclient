#pragma once

#include <cstdint>
#include <vector>

namespace okami
{

/// Warp data structure at main.dll+0xB65E64 (20 bytes)
/// Used to set the destination for map transitions
struct WarpData
{
    float x;               // +0x00: Target X coordinate
    float y;               // +0x04: Target Y coordinate
    float z;               // +0x08: Target Z coordinate
    float facingDirection; // +0x0C: Target facing angle
    uint16_t mapID;        // +0x10: Target map ID
    uint8_t jumpID;        // +0x12: Loading zone ID (0xFF = use map default spawn)
    uint8_t flipCamera;    // +0x13: Interior loading zone ID
    uint32_t unknown;      // +0x14: Unknown purpose (possibly padding or reserved)
};

static_assert(sizeof(WarpData) == 24, "WarpData must be 24 bytes (20 bytes of data + 4 byte alignment)");

/// Preset warp location with a descriptive name
struct WarpPreset
{
    const char *name;
    WarpData data;
};

/// All warp presets organized by category
/// Extracted from OkamiHD-RE-Table-V0.2.CT
namespace WarpPresets
{

// Mist Warp destinations (jumpID=0xFF) - main overworld fast travel points
inline const std::vector<WarpPreset> MistWarp = {
    {"Kamiki", {1816.0f, -441.0f, 2456.0f, 0.192f, 0x0102, 0xFF, 0x00, 0}},
    {"Shinshu Field", {596.3f, 59.6f, -1558.63f, 0.733f, 0x0F02, 0xFF, 0x00, 0}},
    {"Agata Forest", {-2335.07f, 120.55f, 3289.59f, 3.142f, 0x0F03, 0xFF, 0x00, 0}},
    {"Taka Pass", {-799.0f, 627.0f, 1313.0f, -2.967f, 0x0F07, 0xFF, 0x00, 0}},
    {"Kusa Village", {951.504f, -35.03f, 1227.21f, -0.524f, 0x0108, 0xFF, 0x00, 0}},
    {"Sasa Sanctuary", {421.0f, 78.0f, -243.0f, 2.733f, 0x0109, 0xFF, 0x00, 0}},
    {"City Checkpoint", {264.61f, 0.0f, 225.07f, 0.0f, 0x0105, 0xFF, 0x00, 0}},
    {"Ryoshima Coast", {2676.86f, 412.04f, 1945.14f, 0.192f, 0x0F09, 0xFF, 0x00, 0}},
    {"North Ryoshima Coast", {916.93f, -67.26f, -758.75f, 2.716f, 0x0F0C, 0xFF, 0x00, 0}},
    {"North Ryoshima (Rocky Area)", {2005.59f, 612.28f, -565.02f, 1.748f, 0x0F0C, 0xFF, 0x00, 0}},
    {"Kamui", {-1187.26f, -15.96f, -285.56f, 0.026f, 0x0F11, 0xFF, 0x00, 0}},
    {"Wep'keer", {-186.48f, 0.0f, -129.47f, -1.571f, 0x0301, 0xFF, 0x00, 0}},
    {"Inner Yoshpet Spirit Gate", {-951.22f, 803.0f, -15280.16f, -3.142f, 0x0311, 0xFF, 0x00, 0}},
    {"Ezofuji (Rocky Area)", {1096.19f, -35.6f, 92.85f, -0.061f, 0x0F13, 0xFF, 0x00, 0}},
    {"Dragon Palace", {399.0f, 86.0f, -895.0f, 2.762f, 0x0203, 0xFF, 0x00, 0}},
};

// Story progression locations - River of the Heavens, intro areas
inline const std::vector<WarpPreset> StoryProgression = {
    {"Game Start (Title Screen)", {10.0f, -1256.0f, 1311.5f, 3.089f, 0x0100, 0xFF, 0x00, 0}},
    {"River of the Heavens", {-13.0f, -26.0f, 238.0f, -3.089f, 0x0122, 0x00, 0x00, 0}},
    {"Cave of Nagi", {725.0f, 248.0f, -1262.0f, -2.7f, 0x0101, 0x00, 0x00, 0}},
    {"Kamiki 100 Years Past", {0.0f, 208.0f, -769.0f, 0.0f, 0x0302, 0x01, 0x00, 0}},
    {"Shinshu 100 Years Past", {-763.0f, -185.0f, -767.0f, 0.873f, 0x0F20, 0x00, 0x00, 0}},
    {"Moon Cave 100 Years Past", {-6.0f, -178.0f, 1378.0f, -3.142f, 0x0306, 0x00, 0x00, 0}},
};

// Dungeon entrances and interiors
inline const std::vector<WarpPreset> Dungeons = {
    {"Tsuta Ruins", {3.0f, 202.0f, 521.0f, -3.089f, 0x0104, 0x00, 0x00, 0}},
    {"Gale Shrine", {-6003.5f, 11795.0f, 10023.0f, 1.571f, 0x0107, 0x00, 0x00, 0}},
    {"Moon Cave Interior", {0.0f, -24.0f, 1188.0f, 0.0f, 0x0110, 0x00, 0x00, 0}},
    {"Moon Cave Orochi Arena", {25.0f, -177.0f, 1366.0f, 3.142f, 0x0111, 0x02, 0x00, 0}},
    {"Calcified Cavern", {-26.0f, 0.0f, 356.0f, 3.142f, 0x010E, 0x01, 0x00, 0}},
    {"Sunken Ship", {-514.0f, -54.0f, 195.0f, 0.524f, 0x0205, 0x01, 0x00, 0}},
    {"Imperial Palace (Ammy)", {0.0f, 97.0f, 467.0f, 3.142f, 0x0206, 0x00, 0x00, 0}},
    {"Imperial Palace (Issun)", {-4.56f, -18.26f, -235.87f, -3.089f, 0x0207, 0xFF, 0x00, 0}},
    {"Catcall Tower", {-88.0f, 4.0f, 167.0f, 3.142f, 0x020A, 0x00, 0x00, 0}},
    {"Wawku Shrine", {-879.0f, 855.0f, -3165.0f, 3.142f, 0x0303, 0x00, 0x00, 0}},
    {"Yoshpet Forest", {-45.0f, -78.0f, 1573.0f, 3.142f, 0x0310, 0x00, 0x00, 0}},
    {"Inner Yoshpet", {-23.0f, -143.0f, 1681.0f, 3.142f, 0x0311, 0x00, 0x00, 0}},
};

// Oni Island areas
inline const std::vector<WarpPreset> OniIsland = {
    {"Oni Island Exterior", {0.0f, -48.0f, 1457.0f, 3.142f, 0x020D, 0x00, 0x00, 0}},
    {"Oni Island Lower Interior", {0.0f, 0.0f, -100.0f, 3.142f, 0x0208, 0x00, 0x00, 0}},
    {"Oni Island Upper Interior", {-630.0f, 0.0f, 411.0f, 1.571f, 0x020E, 0x00, 0x00, 0}},
    {"Oni Island Sidescroller", {-19041.0f, -4977.0f, -5496.0f, -1.571f, 0x020F, 0x00, 0x00, 0}},
    {"Oni Island Ninetails Arena", {0.0f, -436.0f, 1289.0f, 0.0f, 0x0209, 0x00, 0x00, 0}},
};

// Ark of Yamato areas
inline const std::vector<WarpPreset> ArkOfYamato = {
    {"Ark of Yamato", {0.0f, -84.0f, -147.0f, 0.0f, 0x0307, 0x00, 0x00, 0}},
    {"Ark - Orochi Arena", {-7.0f, 6.0f, 503.0f, 3.089f, 0x0308, 0x00, 0x00, 0}},
    {"Ark - Spider Queen Arena", {-1015.0f, 11795.0f, -15344.0f, -3.089f, 0x0308, 0x00, 0x00, 0}},
    {"Ark - Blight Arena", {-179.0f, 0.0f, -60.0f, 2.356f, 0x0309, 0x00, 0x00, 0}},
    {"Ark - Crimson Helm Arena", {-1015.0f, 186.0f, -658.0f, -1.571f, 0x030B, 0x00, 0x00, 0}},
    {"Ark - Ninetails Arena", {0.0f, 8.0f, 424.0f, 3.142f, 0x030A, 0x00, 0x00, 0}},
    {"Ark - Yami Arena", {0.0f, 4.0f, -299.0f, 0.0f, 0x0312, 0x00, 0x00, 0}},
};

// Boss arenas (non-Ark)
inline const std::vector<WarpPreset> BossArenas = {
    {"Spider Queen Arena", {0.0f, 0.0f, 0.0f, 0.0f, 0x0106, 0x00, 0x00, 0}},
    {"Crimson Helm Arena", {0.0f, 0.0f, 0.0f, 0.0f, 0x010D, 0x00, 0x00, 0}},
    {"Blight Arena", {0.0f, 0.0f, 0.0f, 0.0f, 0x020B, 0x00, 0x00, 0}},
    {"Wawku Nechku Arena", {0.0f, 0.0f, 0.0f, 0.0f, 0x0304, 0x00, 0x00, 0}},
    {"Lechku & Nechku Arena", {0.0f, 0.0f, 0.0f, 0.0f, 0x0314, 0x00, 0x00, 0}},
};

// Sei-an City areas
inline const std::vector<WarpPreset> SeianCity = {
    {"Sei-an Commoners' Quarter", {0.0f, 0.0f, 1097.0f, 3.142f, 0x0201, 0x00, 0x00, 0}},
    {"Sei-an Aristocratic Quarter", {0.0f, 0.0f, -513.0f, 0.0f, 0x0200, 0x00, 0x00, 0}},
    {"Himiko's Palace", {0.0f, 0.0f, 0.0f, 0.0f, 0x0202, 0x00, 0x00, 0}},
    {"Dragon Palace Interior", {0.0f, -36.0f, -416.0f, 3.142f, 0x0203, 0x02, 0x00, 0}},
    {"Inside the Dragon", {0.0f, -50.0f, 0.0f, 3.142f, 0x0204, 0x00, 0x00, 0}},
};

// Kamui region areas
inline const std::vector<WarpPreset> KamuiRegion = {
    {"Kamui (Healed)", {0.0f, -48.0f, -515.0f, 3.142f, 0x0F12, 0xFF, 0x00, 0}},
    {"Wep'keer Square", {6.0f, 0.0f, 193.0f, -3.142f, 0x0313, 0x00, 0x00, 0}},
    {"Ponc'tan", {-21.0f, 207.0f, 713.0f, 3.142f, 0x0305, 0x00, 0x00, 0}},
    {"Mrs. Seal's House", {0.0f, 3.0f, 35.0f, 3.142f, 0x030C, 0x00, 0x00, 0}},
};

// Special caves and grotto locations
inline const std::vector<WarpPreset> Caves = {
    {"Stray Bead #26 Cave (Taka)", {0.0f, 31.0f, -170.0f, -3.142f, 0x0114, 0x00, 0x00, 0}},
    {"Power Slash 2 Cave (NRyo)", {0.0f, 31.0f, -170.0f, -3.142f, 0x0116, 0x00, 0x00, 0}},
    {"Cherry Bomb 2 Cave (NRyo)", {0.0f, 31.0f, -170.0f, -3.142f, 0x0117, 0x00, 0x00, 0}},
    {"Cherry Bomb 3 Cave (Kamui)", {0.0f, 31.0f, -170.0f, -3.142f, 0x0115, 0x00, 0x00, 0}},
    {"Power Slash 3 Cave (Kamui)", {0.0f, 31.0f, -170.0f, -3.142f, 0x0118, 0x00, 0x00, 0}},
    {"Blockhead Grande Cave", {0.0f, 27.0f, -28.0f, 3.142f, 0x0119, 0x00, 0x00, 0}},
    {"Dragon Palace Stray Bead Cave", {0.0f, 31.0f, -170.0f, -3.142f, 0x011C, 0x00, 0x00, 0}},
    {"NRyo Stray Bead #63 Cave", {0.0f, 31.0f, -170.0f, -3.142f, 0x011D, 0x00, 0x00, 0}},
    {"Madame Fawn's House", {0.0f, 1.0f, -50.0f, 3.142f, 0x010A, 0x01, 0x00, 0}},
};

// Bandit spider arena locations
inline const std::vector<WarpPreset> BanditSpiderArenas = {
    {"Ryoshima Bandit Spider Arena", {0.0f, 0.0f, 0.0f, 0.0f, 0x0113, 0x00, 0x00, 0}},
    {"NRyo Bandit Spider Arena", {0.0f, 0.0f, 0.0f, 0.0f, 0x011A, 0x00, 0x00, 0}},
    {"Kamui Bandit Spider Arena", {0.0f, 0.0f, 0.0f, 0.0f, 0x011B, 0x00, 0x00, 0}},
};

// Misc/utility locations
inline const std::vector<WarpPreset> Misc = {
    {"Origin Mirror (RotH)", {-51.0f, 10.0f, -201.0f, -1.571f, 0x0122, 0xFF, 0x00, 0}},
    {"Moon Cave Kitchen", {240.0f, 119.0f, -159.0f, -2.433f, 0x0110, 0xFF, 0x00, 0}},
    {"Onigiri Dojo Lesson Room", {0.0f, 0.0f, 0.0f, 0.0f, 0x010C, 0x00, 0x00, 0}},
    {"Digging Minigame", {0.0f, 0.0f, 0.0f, 0.0f, 0x010B, 0x00, 0x00, 0}},
    {"Kimono Shop", {0.0f, 0.0f, 0.0f, 0.0f, 0x020C, 0x00, 0x00, 0}},
};

/// All preset categories for UI dropdown organization
struct Category
{
    const char *name;
    const std::vector<WarpPreset> *presets;
};

inline const std::vector<Category> AllCategories = {
    {"Mist Warp", &MistWarp},
    {"Story Progression", &StoryProgression},
    {"Dungeons", &Dungeons},
    {"Oni Island", &OniIsland},
    {"Ark of Yamato", &ArkOfYamato},
    {"Boss Arenas", &BossArenas},
    {"Sei-an City", &SeianCity},
    {"Kamui Region", &KamuiRegion},
    {"Caves", &Caves},
    {"Bandit Spider Arenas", &BanditSpiderArenas},
    {"Misc", &Misc},
};

} // namespace WarpPresets

namespace main
{
/// Warp data structure address
constexpr uintptr_t warpData = 0xB65E64;

/// Map load trigger flag address (bit 1 triggers warp)
constexpr uintptr_t mapLoadFlags = 0xB6B2AF;
} // namespace main

} // namespace okami
