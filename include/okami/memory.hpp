
#pragma once
#include <cstdint>
#include <iostream>
#include "brushes.hpp"
#include "ammy.hpp"
#include "items.hpp"
#include "maps.hpp"
#include "dojotech.hpp"
#include "straybeads.hpp"
#include "travelguides.hpp"

namespace okami
{
    inline uintptr_t MainBase;
    inline uintptr_t FlowerBase;

    inline uint32_t *IngameTimeFramesPtr;

    inline void *MainFlowerStartupFunctionPtr;
    inline void *MainFlowerStopFunctionPtr;
    inline void *MainFlowerTickFunctionPtr;

    inline void *MainFlowerLoadPtr;         // ?
    inline void *MainFlowerItemPickedUpPtr; // ?

    /// @brief Initialize variable pointers
    inline void initVariables()
    {
        okami::IngameTimeFramesPtr = (uint32_t *)(okami::MainBase + 0xB217FC);
        okami::AmmyPosXPtr = (float *)(okami::MainBase + 0xB6B2D0 + 0x0000A8 + 0);
        okami::AmmyPosYPtr = (float *)(okami::MainBase + 0xB6B2D0 + 0x0000A8 + 4);
        okami::AmmyPosZPtr = (float *)(okami::MainBase + 0xB6B2D0 + 0x0000A8 + 8);
        okami::AmmyCurrentHealthPtr = (int16_t *)(okami::MainBase + 0xB4DF90);
        okami::AmmyMaxHealthPtr = (int16_t *)(okami::MainBase + 0xB4DF92);
        okami::HealthUpgradeCountPtr = (uint8_t *)(okami::MainBase + 0xB205E5);
        okami::AmmyCurrentInkPtr = (int32_t *)(okami::MainBase + 0xB205D8);
        okami::AmmyMaxInkPtr = (int32_t *)(okami::MainBase + 0xB205DC);
        okami::BrushTargetInkPtr = (int32_t *)(okami::MainBase + 0x8928A4);
        okami::BrushCurrentInkPtr = (int32_t *)(okami::MainBase + 0x8928A8);
        okami::InkUpgradeCountPtr = (uint8_t *)(okami::MainBase + 0xB1F208);
        okami::CanvasBrushStrokesPtr = (uint8_t *)(okami::MainBase + 0x888C58);
        okami::AmmyCurrentFoodPtr = (int16_t *)(okami::MainBase + 0xB4DF94);
        okami::AmmyMaxFoodPtr = (int16_t *)(okami::MainBase + 0xB4DF94);
        okami::FoodUpgradeCountPtr = (uint8_t *)(okami::MainBase + 0xB4DF96);
        okami::AmmyGodhoodPtr = (int16_t *)(okami::MainBase + 0xB4DFAC);
        okami::AmmyCurrentMoneyPtr = (uint32_t *)(okami::MainBase + 0xB205E0);
        okami::AmmyDisplayedMoneyPtr = (uint32_t *)(okami::MainBase + 0xB1CFE4);
        okami::AmmyMaxMoney = (uint32_t *)(okami::MainBase + 0x6B22A8);
        okami::AmmyTotalMoneyPtr = (uint32_t *)(okami::MainBase + 0xB21758);
        okami::MoneyUpgradeCountPtr = (uint8_t *)(okami::MainBase + 0xB205E4);
        okami::AmmyCurrentPraisePtr = (int16_t *)(okami::MainBase + 0xB4DF9A);
        okami::PraiseUpgradeBarPtr = (int16_t *)(okami::MainBase + 0xB1F1F4);
        okami::HealthUpgradeBarPtr = (int16_t *)(okami::MainBase + 0xB1F1F6);
        okami::FoodUpgradeBarPtr = (int16_t *)(okami::MainBase + 0xB1F1F8);
        okami::MoneyUpgradeBarPtr = (int16_t *)(okami::MainBase + 0xB1F1FA);
        okami::InkUpgradeBarPtr = (int16_t *)(okami::MainBase + 0xB1F1FC);
        okami::AmmyTotalPraisePtr = (int16_t *)(okami::MainBase + 0xB4DF9C);
        okami::AmmyCurrentDemonFangsPtr = (int16_t *)(okami::MainBase + 0xB2066E);
        okami::AmmyTotalDemonFangsPtr = (uint16_t *)(okami::MainBase + 0xB2175C);
        okami::InventoryItemIDPtr = (uint16_t *)(okami::MainBase + 0x7A9814);
        okami::InventoryStringIDPtr = (uint16_t *)(okami::MainBase + 0x9C148E);
        okami::InventoryFirstItemOffsetPtr = (uint8_t *)(okami::MainBase + 0xB1F401);
        okami::InventoryColumnPtr = (uint8_t *)(okami::MainBase + 0xB1F402);
        okami::InventorySelectedItemRowPtr = (uint8_t *)(okami::MainBase + 0xB1F403);
        okami::AmmyCurrentSunFragmentsPtr = (int16_t *)(okami::MainBase + 0xB2063A);
        okami::InventoryItemQualities = (int16_t *)(okami::MainBase + 0xB20630);
        okami::AmmyUsableBrushTechniques = (UsableBrushOverlay *)(okami::MainBase + 0x890A30);
        okami::AmmyObtainedBrushTechniques = (ObtainedBrushOverlay *)(okami::MainBase + 0x890A38);
        // okami::AmmyPowerSlashUpgradeCountPtr = (uint8_t *)(okami::MainBase + 0x890A4C);
        // okami::AmmyMaxCherryBombCountPtr = (int32_t *)(okami::MainBase + 0x892B88);
        okami::ExeriorMapIDPtr = (uint16_t *)(okami::MainBase + 0xB6B240);
        okami::CurrentMapIDPtr = (uint16_t *)(okami::MainBase + 0xB65E74);
        okami::VestigialMapID1Ptr = (uint16_t *)(okami::MainBase + 0xB4F0B4);
        okami::VsstigialMapID2Ptr = (uint16_t *)(okami::MainBase + 0xB6B246);
        okami::AmmyUsableDojoTechniques = (UsableDojoOverlay *)(okami::MainBase + 0xB4DFA0);
        okami::CollectionStrayBeads = (StrayBeadOverlay *)(okami::MainBase + 0xB205E8);
        okami::CollectionTravelGuides = (TravelGuideOverlay *)(okami::MainBase + 0xB205F8);
        // uint8_t CollectionTravelGuidesViewed[4];
        // uint8_t CollectionMoveList[4];
        // uint8_t CollectionMoveListViewed[4];
        // uint8_t CollectionFish[8];
        // uint8_t CollectionFishViewed[8];
        // uint8_t CollectionAnimals[4];
        // uint8_t CollectionAnimalsViewed[4];
        // uint8_t CollectionTreasure[8];
        // uint8_t CollectionTreasureViewed[8];
        // uint8_t ItemFirstTimeAcquisition[32];
    }

    /// @brief Initialize game function pointers
    inline void initFunctions()
    {
        okami::MainFlowerStartupFunctionPtr = (void *)(uintptr_t)(okami::MainBase + 0x4B6240);
        okami::MainFlowerStopFunctionPtr = (void *)(uintptr_t)(okami::MainBase + 0x4B6230);
        okami::MainFlowerTickFunctionPtr = (void *)(uintptr_t)(okami::MainBase + 0x4B63B0);
        okami::MainFlowerLoadPtr = (void *)(uintptr_t)(okami::MainBase + 0x4390A0);
        okami::MainFlowerItemPickedUpPtr = (void *)(uintptr_t)(okami::MainBase + 0x436AE0);
    }

    /// @brief Find Base Addresses of both vanilla DLLs
    /// @param MainDllModuleHandle Handle of main.dll
    /// @param FlowerDllModuleHandle Handle of flower_kernel.dll
    /// @return success boolean
    inline bool initialize(void *MainDllModuleHandle, void *FlowerDllModuleHandle)
    {
        std::cout << "[okamigame] Initializing Modules...";
        okami::MainBase = (uintptr_t)(MainDllModuleHandle);
        if (okami::MainBase == 0)
        {
            std::cout << std::endl
                      << "[okamigame] Main.dll BaseAddress not found!" << std::endl;
            return false;
        }
        okami::FlowerBase = (uintptr_t)(FlowerDllModuleHandle);
        if (okami::FlowerBase == 0)
        {
            std::cout << std::endl
                      << "[okamigame] flower_kernel.dll BaseAddress not found!" << std::endl;
            return false;
        }

        initVariables();
        initFunctions();

        std::cout << "Done!" << std::endl;

        std::cout << std::hex << "[okamigame] Module Addresses: main.dll->0x" << okami::MainBase << " flower_kernel.dll->0x" << okami::FlowerBase << std::dec << std::endl;

        return true;
    }
}
