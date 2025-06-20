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

namespace okami
{
    // Base addresses
    extern uintptr_t MainBase;
    extern uintptr_t FlowerBase;

    extern MemoryAccessor<uint32_t> IngameTimeFrames;

    extern MemoryAccessor<float> AmmyPosX;
    extern MemoryAccessor<float> AmmyPosY;
    extern MemoryAccessor<float> AmmyPosZ;
    extern MemoryAccessor<int16_t> AmmyCurrentHealth;
    extern MemoryAccessor<int16_t> AmmyMaxHealth;
    extern MemoryAccessor<uint8_t> HealthUpgradeCount;
    extern MemoryAccessor<int32_t> AmmyCurrentInk;
    extern MemoryAccessor<int32_t> AmmyMaxInk;
    extern MemoryAccessor<int32_t> BrushTargetInk;
    extern MemoryAccessor<int32_t> BrushCurrentInk;
    extern MemoryAccessor<uint8_t> InkUpgradeCount;
    extern MemoryAccessor<uint8_t> CanvasBrushStrokes;
    extern MemoryAccessor<int16_t> AmmyCurrentFood;
    extern MemoryAccessor<int16_t> AmmyMaxFood;
    extern MemoryAccessor<uint8_t> FoodUpgradeCount;
    extern MemoryAccessor<int16_t> AmmyGodhood;
    extern MemoryAccessor<uint32_t> AmmyCurrentMoney;
    extern MemoryAccessor<uint32_t> AmmyDisplayedMoney;
    extern MemoryAccessor<uint32_t> AmmyMaxMoney; // TODO: Make Array of length 4
    extern MemoryAccessor<uint32_t> AmmyTotalMoney;
    extern MemoryAccessor<uint8_t> MoneyUpgradeCount;
    extern MemoryAccessor<int16_t> AmmyCurrentPraise;
    extern MemoryAccessor<int16_t> PraiseUpgradeBar;
    extern MemoryAccessor<int16_t> HealthUpgradeBar;
    extern MemoryAccessor<int16_t> FoodUpgradeBar;
    extern MemoryAccessor<int16_t> MoneyUpgradeBar;
    extern MemoryAccessor<int16_t> InkUpgradeBar;
    extern MemoryAccessor<int16_t> AmmyTotalPraise;
    extern MemoryAccessor<int16_t> AmmyCurrentDemonFangs;
    extern MemoryAccessor<uint16_t> AmmyTotalDemonFangs;

    extern MemoryAccessor<uint16_t> AstralPouchAcqFlag;

    // Other important things
    extern MemoryAccessor<uint8_t> LoadingZoneTrigger;

    extern void *MaybeInventoryStructPtr;

    // Game function pointers
    extern void *D3D11PresentFnPtr;
    extern char *GetSaveDataRootDirectoryFnPtr;
    extern void *MainFlowerStartupFnPtr;
    extern void *MainFlowerStopFnPtr;
    extern void *MainFlowerTickFnPtr;
    extern void *MainFlowerLoadPtr;
    extern void *MainFlowerTreasurePickedUpPtr;
    extern void *MainFlowerItemPickupFnPtr;

    // Overlay structures
    extern MemoryAccessor<BitfieldFlags<BrushOverlay>> AmmyUsableBrushTechniques;
    extern MemoryAccessor<BitfieldFlags<BrushOverlay>> AmmyObtainedBrushTechniques;
    extern MemoryAccessor<BitfieldFlags<DojoOverlay>> AmmyUsableDojoTechniques;
    extern MemoryAccessor<BitfieldFlags<StrayBeadOverlay>> CollectionStrayBeads;
    extern MemoryAccessor<BitfieldFlags<TravelGuideOverlay>> CollectionTravelGuides;
    extern MemoryAccessor<BitfieldFlags<AnimalOverlay>> CollectionAninals;
    extern MemoryAccessor<BitfieldFlags<FishOverlay>> CollectionFish;
    extern MemoryAccessor<BitfieldFlags<TreasureOverlay>> CollectionTreasure;
    extern MemoryAccessor<BitfieldFlags<TreasureOverlay>> CollectionAcquisitions;

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
