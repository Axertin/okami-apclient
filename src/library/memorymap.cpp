#include "okami/memorymap.hpp"

#include <array>
#include <cstdint>
#include <iostream>

#include "okami/acquisition.hpp"
#include "okami/animals.hpp"
#include "okami/brushes.hpp"
#include "okami/data/structs.hpp"
#include "okami/dojotech.hpp"
#include "okami/fish.hpp"
#include "okami/items.hpp"
#include "okami/maps.hpp"
#include "okami/memoryaccessor.hpp"
#include "okami/straybeads.hpp"
#include "okami/travelguides.hpp"
#include "okami/treasures.hpp"

namespace okami
{
// Base addresses
uintptr_t MainBase;

MemoryAccessor<CharacterStats> AmmyStats;
MemoryAccessor<CollectionData> AmmyCollections;
MemoryAccessor<TrackerData> AmmyTracker;
MemoryAccessor<std::array<MapState, MapTypes::NUM_MAP_TYPES>> MapData;
MemoryAccessor<std::array<BitField<512>, MapTypes::NUM_MAP_TYPES>> DialogBits;
static MemoryAccessor<std::array<ItemParam, ItemTypes::NUM_ITEM_TYPES>> ItemParams;

MemoryAccessor<BitField<86>> GlobalGameStateFlags;

MemoryAccessor<cAmmyModel *> AmmyModel;
MemoryAccessor<int32_t> BrushTargetInk;
MemoryAccessor<int32_t> BrushCurrentInk;
MemoryAccessor<uint8_t> InkUpgradeCount;
MemoryAccessor<uint8_t> CanvasBrushStrokes;
MemoryAccessor<uint8_t> FoodUpgradeCount;
MemoryAccessor<uint32_t> AmmyDisplayedMoney;
MemoryAccessor<uint32_t> AmmyMaxMoney; // TODO: Make Array of length 4
MemoryAccessor<int16_t> PraiseUpgradeBar;
MemoryAccessor<int16_t> HealthUpgradeBar;
MemoryAccessor<int16_t> FoodUpgradeBar;
MemoryAccessor<int16_t> MoneyUpgradeBar;
MemoryAccessor<int16_t> InkUpgradeBar;

// Other important things
MemoryAccessor<float> CameraFOV;

void *MaybeAmmyObject;
void *MaybeInventoryStructPtr;
void *MaybePlayerClassPtr;

// Game function pointers

void *D3D11PresentFnPtr = nullptr;

MemoryAccessor<BitField<32>> AmmyUsableBrushes;
MemoryAccessor<BitField<32>> AmmyObtainedBrushes;
MemoryAccessor<std::array<uint8_t, 64>> AmmyBrushUpgrades; // why are there even 64 wtf??

/**
 * @brief Initialize game variable pointers and overlays.
 * Must be called after `okami::MainBase` is assigned.
 */
void initVariables()
{
    AmmyModel.bind(okami::MainBase + 0xB6B2D0);

    // everything needed for a save file vvvvvvvvv
    AmmyStats.bind(okami::MainBase + 0xB4DF90);
    AmmyCollections.bind(okami::MainBase + 0xB205D0);
    AmmyTracker.bind(okami::MainBase + 0xB21780);
    MapData.bind(okami::MainBase + 0xB322B0);
    DialogBits.bind(okami::MainBase + 0xB36CF0);
    // everything needed for a save file ^^^^^^^^

    GlobalGameStateFlags.bind(okami::MainBase + 0xB6B2AC);

    BrushTargetInk.bind(okami::MainBase + 0x8928A4);
    BrushCurrentInk.bind(okami::MainBase + 0x8928A8);
    InkUpgradeCount.bind(okami::MainBase + 0xB1F208);
    CanvasBrushStrokes.bind(okami::MainBase + 0x888C58);

    // NOTE: this gets computed via max food value in CharacterStats and is
    // strictly used for inventory
    FoodUpgradeCount.bind(okami::MainBase + 0xB1F207);

    AmmyDisplayedMoney.bind(okami::MainBase + 0xB1CFE4);
    AmmyMaxMoney.bind(okami::MainBase + 0x6B22A8);

    PraiseUpgradeBar.bind(okami::MainBase + 0xB1F1F4);
    HealthUpgradeBar.bind(okami::MainBase + 0xB1F1F6);
    FoodUpgradeBar.bind(okami::MainBase + 0xB1F1F8);
    MoneyUpgradeBar.bind(okami::MainBase + 0xB1F1FA);
    InkUpgradeBar.bind(okami::MainBase + 0xB1F1FC);

    InventoryItemID.bind(okami::MainBase + 0x7A9814);
    InventoryStringID.bind(okami::MainBase + 0x9C148E);

    ExteriorMapID.bind(okami::MainBase + 0xB6B240);
    CurrentMapID.bind(okami::MainBase + 0xB65E74);
    VestigialMapID1.bind(okami::MainBase + 0xB4F0B4);
    VestigialMapID2.bind(okami::MainBase + 0xB6B246);

    CameraFOV.bind(okami::MainBase + 0xB663B0);

    MaybeAmmyObject = reinterpret_cast<void *>(okami::MainBase + 0xB6B2D0);
    MaybeInventoryStructPtr = reinterpret_cast<void *>(okami::MainBase + 0xB66670);
    MaybePlayerClassPtr = reinterpret_cast<void *>(okami::MainBase + 0x8909C0);

    AmmyUsableBrushes.bind(okami::MainBase + 0x890A30);
    AmmyObtainedBrushes.bind(okami::MainBase + 0x890A38);
    AmmyBrushUpgrades.bind(okami::MainBase + 0x8909C0 + 0x80);
    ItemParams.bind(okami::MainBase + 0x7AB220);

    // TODO test each value up to and including 7
    // 0 = grey
    // 1 = peach - pickups, food
    // 2 = peach - consumable items, KT
    // 3 = cyan
    // 4 = red - divine instrument
    // 5 = purple - treasure
    // 6 = cyan - fish
    // 7 = yellow - key item, travel guide, map, stray bead
    ItemParams->at(ItemTypes::ArchipelagoTestItem1).category = 1;
    ItemParams->at(ItemTypes::ArchipelagoTestItem2).category = 2;
    ItemParams->at(ItemTypes::ArchipelagoTestItem3).category = 3;
    ItemParams->at(ItemTypes::ArchipelagoTestItem4).category = 4;
    ItemParams->at(ItemTypes::ArchipelagoTestItem5).category = 5;
    ItemParams->at(ItemTypes::ArchipelagoTestItem6).category = 6;
    ItemParams->at(ItemTypes::ArchipelagoTestItem7).category = 7;
}

} // namespace okami
