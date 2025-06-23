#pragma once
#include <cstdint>
#include <iostream>

#include "memoryaccessor.hpp"
#include "brushes.hpp"
#include "items.hpp"
#include "maps.hpp"
#include "dojotech.hpp"
#include "straybeads.hpp"
#include "travelguides.hpp"
#include "animals.hpp"
#include "treasures.hpp"
#include "fish.hpp"
#include "acquisition.hpp"

#include "data/structs.hpp"

namespace okami
{
    // Base addresses
    extern uintptr_t MainBase;
    extern uintptr_t FlowerBase;

    extern MemoryAccessor<CharacterStats> AmmyStats;
    extern MemoryAccessor<CollectionData> AmmyCollections;
    extern MemoryAccessor<TrackerData> AmmyTracker;
    extern MemoryAccessor<std::array<BitField<1824>, MapTypes::NUM_MAP_TYPES>> MapBits;
    extern MemoryAccessor<std::array<BitField<512>, MapTypes::NUM_MAP_TYPES>> IssunDialogBits;

    extern MemoryAccessor<float> AmmyPosX;
    extern MemoryAccessor<float> AmmyPosY;
    extern MemoryAccessor<float> AmmyPosZ;
    extern MemoryAccessor<int32_t> BrushTargetInk;
    extern MemoryAccessor<int32_t> BrushCurrentInk;
    extern MemoryAccessor<uint8_t> InkUpgradeCount;
    extern MemoryAccessor<uint8_t> CanvasBrushStrokes;
    extern MemoryAccessor<uint8_t> FoodUpgradeCount;
    extern MemoryAccessor<uint32_t> AmmyDisplayedMoney;
    extern MemoryAccessor<uint32_t> AmmyMaxMoney; // TODO: Make Array of length 4
    extern MemoryAccessor<int16_t> PraiseUpgradeBar;
    extern MemoryAccessor<int16_t> HealthUpgradeBar;
    extern MemoryAccessor<int16_t> FoodUpgradeBar;
    extern MemoryAccessor<int16_t> MoneyUpgradeBar;
    extern MemoryAccessor<int16_t> InkUpgradeBar;

    // Other important things
    extern MemoryAccessor<uint8_t> LoadingZoneTrigger;
    extern MemoryAccessor<float> CameraFOV;

    extern void *MaybeAmmyObject;
    extern void *MaybeInventoryStructPtr;
    extern void *MaybePlayerClassPtr;

    // Game function pointers
    extern void *D3D11PresentFnPtr;
    extern char *GetSaveDataRootDirectoryFnPtr;
    extern void *MainFlowerStartupFnPtr;
    extern void *MainFlowerStopFnPtr;
    extern void *MainFlowerTickFnPtr;
    extern void *MainFlowerLoadPtr;
    extern void *MainFlowerTreasurePickedUpPtr;
    extern void *MainFlowerItemPickupFnPtr;
    extern void *EditBrushesFnPtr;

    // Overlay structures
    extern MemoryAccessor<BitfieldFlags<BrushOverlay>> AmmyUsableBrushTechniques;
    extern MemoryAccessor<BitfieldFlags<BrushOverlay>> AmmyObtainedBrushTechniques;

    /**
     * @brief Initialize game variable pointers and overlays.
     * Must be called after `okami::MainBase` is assigned.
     */
    extern void initVariables();

    /**
     * @brief Initialize major engine function pointers from known okami::MainBase offsets.
     */
    extern void initFunctions();
} // namespace okami
