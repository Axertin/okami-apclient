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

namespace okami
{
    // Base addresses
    inline uintptr_t MainBase;
    inline uintptr_t FlowerBase;

    // Raw memory pointers
    inline MemoryAccessor<uint32_t> IngameTimeFrames;

    inline MemoryAccessor<float> AmmyPosX;
    inline MemoryAccessor<float> AmmyPosY;
    inline MemoryAccessor<float> AmmyPosZ;
    inline MemoryAccessor<int16_t> AmmyCurrentHealth;
    inline MemoryAccessor<int16_t> AmmyMaxHealth;
    inline MemoryAccessor<uint8_t> HealthUpgradeCount;
    inline MemoryAccessor<int32_t> AmmyCurrentInk;
    inline MemoryAccessor<int32_t> AmmyMaxInk;
    inline MemoryAccessor<int32_t> BrushTargetInk;
    inline MemoryAccessor<int32_t> BrushCurrentInk;
    inline MemoryAccessor<uint8_t> InkUpgradeCount;
    inline MemoryAccessor<uint8_t> CanvasBrushStrokes;
    inline MemoryAccessor<int16_t> AmmyCurrentFood;
    inline MemoryAccessor<int16_t> AmmyMaxFood;
    inline MemoryAccessor<uint8_t> FoodUpgradeCount;
    inline MemoryAccessor<int16_t> AmmyGodhood;
    inline MemoryAccessor<uint32_t> AmmyCurrentMoney;
    inline MemoryAccessor<uint32_t> AmmyDisplayedMoney;
    inline MemoryAccessor<uint32_t> AmmyMaxMoney; // Array of length 4
    inline MemoryAccessor<uint32_t> AmmyTotalMoney;
    inline MemoryAccessor<uint8_t> MoneyUpgradeCount;
    inline MemoryAccessor<int16_t> AmmyCurrentPraise;
    inline MemoryAccessor<int16_t> PraiseUpgradeBar;
    inline MemoryAccessor<int16_t> HealthUpgradeBar;
    inline MemoryAccessor<int16_t> FoodUpgradeBar;
    inline MemoryAccessor<int16_t> MoneyUpgradeBar;
    inline MemoryAccessor<int16_t> InkUpgradeBar;
    inline MemoryAccessor<int16_t> AmmyTotalPraise;
    inline MemoryAccessor<int16_t> AmmyCurrentDemonFangs;
    inline MemoryAccessor<uint16_t> AmmyTotalDemonFangs;

    // Game function pointers
    inline void *MainFlowerStartupFnPtr;
    inline void *MainFlowerStopFnPtr;
    inline void *MainFlowerTickFnPtr;
    inline void *MainFlowerLoadPtr;
    inline void *MainFlowerItemPickedUpPtr;

    // Overlay structures
    inline MemoryAccessor<BitfieldFlags<BrushOverlay>> AmmyUsableBrushTechniques;
    inline MemoryAccessor<BitfieldFlags<BrushOverlay>> AmmyObtainedBrushTechniques;
    inline MemoryAccessor<BitfieldFlags<DojoOverlay>> AmmyUsableDojoTechniques;
    inline MemoryAccessor<BitfieldFlags<StrayBeadOverlay>> CollectionStrayBeads;
    inline MemoryAccessor<BitfieldFlags<TravelGuideOverlay>> CollectionTravelGuides;

    /**
     * @brief Initialize game variable pointers and overlays.
     * Must be called after `okami::MainBase` is assigned.
     */
    inline void initVariables()
    {
        IngameTimeFrames.bind(okami::MainBase + 0xB217FC);

        AmmyPosX.bind(okami::MainBase + 0xB6B2D0 + 0xA8 + 0);
        AmmyPosY.bind(okami::MainBase + 0xB6B2D0 + 0xA8 + 4);
        AmmyPosZ.bind(okami::MainBase + 0xB6B2D0 + 0xA8 + 8);

        AmmyCurrentHealth.bind(okami::MainBase + 0xB4DF90);
        AmmyMaxHealth.bind(okami::MainBase + 0xB4DF92);
        HealthUpgradeCount.bind(okami::MainBase + 0xB205E5);

        AmmyCurrentInk.bind(okami::MainBase + 0xB205D8);
        AmmyMaxInk.bind(okami::MainBase + 0xB205DC);
        BrushTargetInk.bind(okami::MainBase + 0x8928A4);
        BrushCurrentInk.bind(okami::MainBase + 0x8928A8);
        InkUpgradeCount.bind(okami::MainBase + 0xB1F208);
        CanvasBrushStrokes.bind(okami::MainBase + 0x888C58);

        AmmyCurrentFood.bind(okami::MainBase + 0xB4DF94);
        AmmyMaxFood.bind(okami::MainBase + 0xB4DF94); // TODO: I forgor
        FoodUpgradeCount.bind(okami::MainBase + 0xB4DF96);

        AmmyGodhood.bind(okami::MainBase + 0xB4DFAC);

        AmmyCurrentMoney.bind(okami::MainBase + 0xB205E0);
        AmmyDisplayedMoney.bind(okami::MainBase + 0xB1CFE4);
        AmmyMaxMoney.bind(okami::MainBase + 0x6B22A8);
        AmmyTotalMoney.bind(okami::MainBase + 0xB21758);
        MoneyUpgradeCount.bind(okami::MainBase + 0xB205E4);

        AmmyCurrentPraise.bind(okami::MainBase + 0xB4DF9A);
        AmmyTotalPraise.bind(okami::MainBase + 0xB4DF9C);
        AmmyCurrentDemonFangs.bind(okami::MainBase + 0xB2066E);
        AmmyTotalDemonFangs.bind(okami::MainBase + 0xB2175C);

        PraiseUpgradeBar.bind(okami::MainBase + 0xB1F1F4);
        HealthUpgradeBar.bind(okami::MainBase + 0xB1F1F6);
        FoodUpgradeBar.bind(okami::MainBase + 0xB1F1F8);
        MoneyUpgradeBar.bind(okami::MainBase + 0xB1F1FA);
        InkUpgradeBar.bind(okami::MainBase + 0xB1F1FC);

        InventoryItemID.bind(okami::MainBase + 0x7A9814);
        InventoryStringID.bind(okami::MainBase + 0x9C148E);
        InventoryFirstItemOffset.bind(okami::MainBase + 0xB1F401);
        InventoryColumn.bind(okami::MainBase + 0xB1F402);
        InventorySelectedItemRow.bind(okami::MainBase + 0xB1F403);
        AmmyCurrentSunFragments.bind(okami::MainBase + 0xB2063A);
        InventoryItemQualities.bind(okami::MainBase + 0xB20630);

        ExeriorMapID.bind(okami::MainBase + 0xB6B240);
        CurrentMapID.bind(okami::MainBase + 0xB65E74);
        VestigialMapID1.bind(okami::MainBase + 0xB4F0B4);
        VestigialMapID2.bind(okami::MainBase + 0xB6B246);

        // Overlay memory helpers
        AmmyUsableBrushTechniques.bind(okami::MainBase + 0x890A30, 4);
        AmmyObtainedBrushTechniques.bind(okami::MainBase + 0x890A38, 4);
        AmmyUsableDojoTechniques.bind(okami::MainBase + 0xB4DFA0, 4);
        CollectionStrayBeads.bind(okami::MainBase + 0xB205E8, 32);
        CollectionTravelGuides.bind(okami::MainBase + 0xB205F8, 4);

        // TODO: TravelGuidesViewed, MoveList, Fish, Animals, Treasure, etc.
    }

    /**
     * @brief Initialize major engine function pointers from known okami::MainBase offsets.
     */
    inline void initFunctions()
    {
        MainFlowerStartupFnPtr = reinterpret_cast<void *>(okami::MainBase + 0x4B6240);
        MainFlowerStopFnPtr = reinterpret_cast<void *>(okami::MainBase + 0x4B6230);
        MainFlowerTickFnPtr = reinterpret_cast<void *>(okami::MainBase + 0x4B63B0);
        MainFlowerLoadPtr = reinterpret_cast<void *>(okami::MainBase + 0x4390A0);
        MainFlowerItemPickedUpPtr = reinterpret_cast<void *>(okami::MainBase + 0x436AE0);
    }
} // namespace okami
