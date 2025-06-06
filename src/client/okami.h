#pragma once
#include "pch.h"

namespace okami
{
    bool initialize(HMODULE MainDllModuleHandle, HMODULE FlowerDllModuleHandle);
    void printMonitors();

    // Bases
    extern uintptr_t MainBase;
    extern uintptr_t FlowerBase;

    // Variable
    extern uint32_t *IngameTimeFramesPtr;
    extern float *AmmyPosXPtr;
    extern float *AmmyPosYPtr;
    extern float *AmmyPosZPtr;
    extern int16_t *AmmyCurrentHealthPtr;
    extern int16_t *AmmyMaxHealthPtr;
    extern uint8_t *HealthUpgradeCountPtr;
    extern int32_t *AmmyCurrentInkPtr;
    extern int32_t *AmmyMaxInkPtr;
    extern int32_t *BrushTargetInkPtr;
    extern int32_t *BrushCurrentInkPtr;
    extern uint8_t *InkUpgradeCountPtr;
    extern uint8_t *CanvasBrushStrokesPtr;
    extern int16_t *AmmyCurrentFoodPtr;
    extern int16_t *AmmyMaxFoodPtr;
    extern uint8_t *FoodUpgradeCountPtr;
    extern int16_t *AmmyGodhoodPtr;
    extern uint32_t *AmmyCurrentMoneyPtr;
    extern uint32_t *AmmyDisplayedMoneyPtr;
    extern uint32_t *AmmyMaxMoney; // Array of length 4
    extern uint32_t *AmmyTotalMoneyPtr;
    extern uint8_t *MoneyUpgradeCountPtr;
    extern int16_t *AmmyCurrentPraisePtr;
    extern int16_t *PraiseUpgradeBarPtr;
    extern int16_t *HealthUpgradeBarPtr;
    extern int16_t *FoodUpgradeBarPtr;
    extern int16_t *MoneyUpgradeBarPtr;
    extern int16_t *InkUpgradeBarPtr;
    extern int16_t *AmmyTotalPraisePtr;
    extern int16_t *AmmyCurrentDemonFangsPtr;
    extern uint16_t *AmmyTotalDemonFangsPtr;
    extern uint16_t *InventoryItemIDPtr;
    extern uint16_t *InventoryStringIDPtr;
    extern uint8_t *InventoryFirstItemOffsetPtr;
    extern uint8_t *InventoryColumnPtr;
    extern uint8_t *InventorySelectedItemRowPtr;
    extern int16_t *AmmyCurrentSunFragmentsPtr;
    extern int16_t *InventoryItemQualities[256];
    extern uint8_t *AmmyUsableBrushTechniques[4];
    extern uint8_t *AmmyObtainedBrushTechniques[4];
    extern uint8_t *AmmyPowerSlashUpgradeCountPtr;
    extern int32_t *AmmyMaxCherryBombCountPtr;
    extern uint16_t *ExeriorMapIDPtr; // Loading zone sequence 3
    extern uint16_t *CurrentMapIDPtr; // Loading zone sequence 1
    extern uint16_t *VestigialMapID1Ptr;
    extern uint16_t *VsstigialMapID2Ptr; // Loading zone sequence 2
    extern uint8_t *AmmyUsableDojoTechniques[4];
    extern uint8_t *CollectionStrayBeads[16];
    extern uint8_t *CollectionTravelGuides[4];
    extern uint8_t *CollectionTravelGuidesViewed[4];
    extern uint8_t *CollectionMoveList[4];
    extern uint8_t *CollectionMoveListViewed[4];
    extern uint8_t *CollectionFish[8];
    extern uint8_t *CollectionFishViewed[8];
    extern uint8_t *CollectionAnimals[4];
    extern uint8_t *CollectionAnimalsViewed[4];
    extern uint8_t *CollectionTreasure[8];
    extern uint8_t *CollectionTreasureViewed[8];
    extern uint8_t *ItemFirstTimeAcquisition[32];

    // Functions
    extern LPVOID MainFlowerStartupFunctionPtr;
    extern LPVOID MainFlowerStopFunctionPtr;
    extern LPVOID MainFlowerTickFunctionPtr;
}