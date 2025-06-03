#include "pch.h"

struct memtable
{
    uint32_t IngameTimeFrames;
    float AmmyPosX;
    float AmmyPosY;
    float AmmyPosZ;
    int16_t AmmyCurrentHealth;
    int16_t AmmyMaxHealth;
    uint8_t HealthUpgradeCount;
    int32_t AmmyCurrentInk;
    int32_t AmmyMaxInk;
    int32_t BrushTargetInk;
    int32_t BrushCurrentInk;
    uint8_t InkUpgradeCount;
    uint8_t CanvasBrushStrokes;
    int16_t AmmyCurrentFood;
    int16_t AmmyMaxFood;
    uint8_t FoodUpgradeCount;
    int16_t AmmyGodhood;
    uint32_t AmmyCurrentMoney;
    uint32_t AmmyDisplayedMoney;
    uint32_t AmmyMaxMoney[4];
    uint32_t AmmyTotalMoney;
    uint8_t MoneyUpgradeCount;
    int16_t AmmyCurrentPraise;
    int16_t PraiseUpgradeBar;
    int16_t HealthUpgradeBar;
    int16_t FoodUpgradeBar;
    int16_t MoneyUpgradeBar;
    int16_t InkUpgradeBar;
    int16_t AmmyTotalPraise;
    int16_t AmmyCurrentDemonFangs;
    uint16_t AmmyTotalDemonFangs;
    uint16_t InventoryItemID;
    uint16_t InventoryStringID;
    uint8_t InventoryFirstItemOffset;
    uint8_t InventoryColumn;
    uint8_t InventorySelectedItemRow;
    int16_t AmmyCurrentSunFragments;
    int16_t InventoryItemQualities[256];
    uint8_t AmmyUsableBrushTechniques[4];
    uint8_t AmmyObtainedBrushTechniques[4];
    uint8_t AmmyPowerSlashUpgradeCount;
    int32_t AmmyMaxCherryBombCount;
    uint16_t ExeriorMapID; // Loading zone sequence 3
    uint16_t CurrentMapID; // Loading zone sequence 1
    uint16_t VestigialMapID1;
    uint16_t VsstigialMapID2; // Loading zone sequence 2
    uint8_t AmmyUsableDojoTechniques[4];
    uint8_t CollectionStrayBeads[16];
    uint8_t CollectionTravelGuides[4];
    uint8_t CollectionTravelGuidesViewed[4];
    uint8_t CollectionMoveList[4];
    uint8_t CollectionMoveListViewed[4];
    uint8_t CollectionFish[8];
    uint8_t CollectionFishViewed[8];
    uint8_t CollectionAnimals[4];
    uint8_t CollectionAnimalsViewed[4];
    uint8_t CollectionTreasure[8];
    uint8_t CollectionTreasureViewed[8];
    uint8_t ItemFirstTimeAcquisition[32];
};

/// @brief Read In-Game Time from memory (at 60FPS)
/// @return IGT, in Frames
uint32_t readIngameTime(void);

float readAmmyPosX(void);
float readAmmyPosY(void);
float readAmmyPosZ(void);

int32_t readAmmyYen(void);
uintptr_t getBaseAddress(void);
memtable getMemTable(void);

void printMemTable(memtable mem);
