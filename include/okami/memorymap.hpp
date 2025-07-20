#pragma once
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
extern uintptr_t MainBase;
extern uintptr_t FlowerBase;

extern MemoryAccessor<CharacterStats> AmmyStats;
extern MemoryAccessor<CollectionData> AmmyCollections;
extern MemoryAccessor<TrackerData> AmmyTracker;
extern MemoryAccessor<std::array<MapState, MapTypes::NUM_MAP_TYPES>> MapData;
extern MemoryAccessor<std::array<BitField<512>, MapTypes::NUM_MAP_TYPES>> DialogBits;
extern MemoryAccessor<BitField<86>> GlobalGameStateFlags;

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

extern MemoryAccessor<BitField<32>> AmmyUsableBrushes;
extern MemoryAccessor<BitField<32>> AmmyObtainedBrushes;
extern MemoryAccessor<std::array<uint8_t, 64>> AmmyBrushUpgrades;

/**
 * @brief Initialize game variable pointers and overlays.
 * Must be called after `okami::MainBase` is assigned.
 */
extern void initVariables();

/**
 * @brief Initialize major engine function pointers from known okami::MainBase
 * offsets.
 */
extern void initFunctions();
} // namespace okami
