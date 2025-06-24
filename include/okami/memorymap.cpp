#include "memorymap.hpp"

#include <array>
#include <cstdint>
#include <iostream>

#include "acquisition.hpp"
#include "animals.hpp"
#include "brushes.hpp"
#include "data/structs.hpp"
#include "dojotech.hpp"
#include "fish.hpp"
#include "items.hpp"
#include "maps.hpp"
#include "memoryaccessor.hpp"
#include "straybeads.hpp"
#include "travelguides.hpp"
#include "treasures.hpp"

namespace okami
{
// Base addresses
uintptr_t MainBase;
uintptr_t FlowerBase;

MemoryAccessor<CharacterStats> AmmyStats;
MemoryAccessor<CollectionData> AmmyCollections;
MemoryAccessor<TrackerData> AmmyTracker;
MemoryAccessor<std::array<BitField<1824>, MapTypes::NUM_MAP_TYPES>> MapBits;
MemoryAccessor<std::array<BitField<512>, MapTypes::NUM_MAP_TYPES>>
    IssunDialogBits;

MemoryAccessor<float> AmmyPosX;
MemoryAccessor<float> AmmyPosY;
MemoryAccessor<float> AmmyPosZ;
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
MemoryAccessor<uint8_t> LoadingZoneTrigger;
MemoryAccessor<float> CameraFOV;

void *MaybeAmmyObject;
void *MaybeInventoryStructPtr;
void *MaybePlayerClassPtr;

// Game function pointers

void *D3D11PresentFnPtr;
char *GetSaveDataRootDirectoryFnPtr;
void *MainFlowerStartupFnPtr;
void *MainFlowerStopFnPtr;
void *MainFlowerTickFnPtr;
void *MainFlowerLoadPtr;
void *MainFlowerTreasurePickedUpPtr;
void *MainFlowerItemPickupFnPtr;
void *EditBrushesFnPtr;

/**
 * @brief Initialize game variable pointers and overlays.
 * Must be called after `okami::MainBase` is assigned.
 */
void initVariables()
{
    AmmyPosX.bind(okami::MainBase + 0xB1EC10 + 0); // probably read-only copies
    AmmyPosY.bind(okami::MainBase + 0xB1EC10 + 4); // probably read-only copies
    AmmyPosZ.bind(okami::MainBase + 0xB1EC10 + 8); // probably read-only copies

    // everything needed for a save file vvvvvvvvv
    AmmyStats.bind(okami::MainBase + 0xB4DF90);
    AmmyCollections.bind(okami::MainBase + 0xB205D0);
    AmmyTracker.bind(okami::MainBase + 0xB21780);
    MapBits.bind(okami::MainBase + 0xB322B0);
    IssunDialogBits.bind(okami::MainBase + 0xB36CF0);
    // everything needed for a save file ^^^^^^^^

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

    LoadingZoneTrigger.bind(okami::MainBase + 0xB6B2AF);
    CameraFOV.bind(okami::MainBase + 0xB663B0);

    MaybeAmmyObject = reinterpret_cast<void *>(okami::MainBase + 0xB6B2D0);
    MaybeInventoryStructPtr =
        reinterpret_cast<void *>(okami::MainBase + 0xB66670);
    MaybePlayerClassPtr = reinterpret_cast<void *>(okami::MainBase + 0x8909C0);
}

/**
 * @brief Initialize major engine function pointers from known okami::MainBase
 * offsets.
 */
void initFunctions()
{
    // edit brushbitfield = mainBase + 17C270 (void setBrushes(void* <ammy
    // class? this?>, uint32_t index, int op)) --> op
    GetSaveDataRootDirectoryFnPtr =
        reinterpret_cast<char *>(okami::FlowerBase + 0x22F7);
    MainFlowerStartupFnPtr =
        reinterpret_cast<void *>(okami::MainBase + 0x4B6240);
    MainFlowerStopFnPtr = reinterpret_cast<void *>(okami::MainBase + 0x4B6230);
    MainFlowerTickFnPtr = reinterpret_cast<void *>(okami::MainBase + 0x4B63B0);
    MainFlowerLoadPtr = reinterpret_cast<void *>(okami::MainBase + 0x4390A0);
    MainFlowerTreasurePickedUpPtr =
        reinterpret_cast<void *>(okami::MainBase + 0x436AE0);
    MainFlowerItemPickupFnPtr =
        reinterpret_cast<void *>(okami::MainBase + 0x4965D0);

    EditBrushesFnPtr = reinterpret_cast<void *>(okami::MainBase + 0x17C270);
}
} // namespace okami
