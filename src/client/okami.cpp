#include "okami.h"

namespace okami
{
    uintptr_t MainBase;
    uintptr_t FlowerBase;

    uint32_t *IngameTimeFramesPtr;
    float *AmmyPosXPtr;
    float *AmmyPosYPtr;
    float *AmmyPosZPtr;
    int16_t *AmmyCurrentHealthPtr;
    int16_t *AmmyMaxHealthPtr;
    uint8_t *HealthUpgradeCountPtr;
    int32_t *AmmyCurrentInkPtr;
    int32_t *AmmyMaxInkPtr;
    int32_t *BrushTargetInkPtr;
    int32_t *BrushCurrentInkPtr;
    uint8_t *InkUpgradeCountPtr;
    uint8_t *CanvasBrushStrokesPtr;
    int16_t *AmmyCurrentFoodPtr;
    int16_t *AmmyMaxFoodPtr;
    uint8_t *FoodUpgradeCountPtr;
    int16_t *AmmyGodhoodPtr;
    uint32_t *AmmyCurrentMoneyPtr;
    uint32_t *AmmyDisplayedMoneyPtr;
    uint32_t *AmmyMaxMoney;
    uint32_t *AmmyTotalMoneyPtr;
    uint8_t *MoneyUpgradeCountPtr;
    int16_t *AmmyCurrentPraisePtr;
    int16_t *PraiseUpgradeBarPtr;
    int16_t *HealthUpgradeBarPtr;
    int16_t *FoodUpgradeBarPtr;
    int16_t *MoneyUpgradeBarPtr;
    int16_t *InkUpgradeBarPtr;
    int16_t *AmmyTotalPraisePtr;
    int16_t *AmmyCurrentDemonFangsPtr;
    uint16_t *AmmyTotalDemonFangsPtr;
    uint16_t *InventoryItemIDPtr;
    uint16_t *InventoryStringIDPtr;
    uint8_t *InventoryFirstItemOffsetPtr;
    uint8_t *InventoryColumnPtr;
    uint8_t *InventorySelectedItemRowPtr;
    int16_t *AmmyCurrentSunFragmentsPtr;
    int16_t *InventoryItemQualities[256];
    uint8_t *AmmyUsableBrushTechniques[4];
    uint8_t *AmmyObtainedBrushTechniques[4];
    uint8_t *AmmyPowerSlashUpgradeCountPtr;
    int32_t *AmmyMaxCherryBombCountPtr;
    uint16_t *ExeriorMapIDPtr; // Loading zone sequence 3
    uint16_t *CurrentMapIDPtr; // Loading zone sequence 1
    uint16_t *VestigialMapID1Ptr;
    uint16_t *VsstigialMapID2Ptr; // Loading zone sequence 2
    uint8_t *AmmyUsableDojoTechniques[4];
    uint8_t *CollectionStrayBeads[16];
    uint8_t *CollectionTravelGuides[4];
    uint8_t *CollectionTravelGuidesViewed[4];
    uint8_t *CollectionMoveList[4];
    uint8_t *CollectionMoveListViewed[4];
    uint8_t *CollectionFish[8];
    uint8_t *CollectionFishViewed[8];
    uint8_t *CollectionAnimals[4];
    uint8_t *CollectionAnimalsViewed[4];
    uint8_t *CollectionTreasure[8];
    uint8_t *CollectionTreasureViewed[8];
    uint8_t *ItemFirstTimeAcquisition[32];

    LPVOID MainFlowerStartupFunctionPtr;
    LPVOID MainFlowerStopFunctionPtr;
    LPVOID MainFlowerTickFunctionPtr;
};

void initVariables();
void initFunctions();

bool okami::initialize(HMODULE MainDllModuleHandle, HMODULE FlowerDllModuleHandle)
{
    std::cout << "[apclient] Initializing Modules...";
    okami::MainBase = (uintptr_t)(MainDllModuleHandle);
    if (okami::MainBase == 0)
    {
        std::cout << std::endl
                  << "[apclient] Main.dll BaseAddress not found!" << std::endl;
        return false;
    }
    okami::FlowerBase = (uintptr_t)(FlowerDllModuleHandle);
    if (okami::FlowerBase == 0)
    {
        std::cout << std::endl
                  << "[apclient] flower_kernel.dll BaseAddress not found!" << std::endl;
        return false;
    }

    initVariables();
    initFunctions();

    std::cout << "Done!" << std::endl;

    std::cout << std::hex << "[apclient] Module Addresses: main.dll->0x" << okami::MainBase << " flower_kernel.dll->0x" << okami::FlowerBase << std::dec << std::endl;

    return true;
}

void okami::printMonitors()
{
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
    GetConsoleScreenBufferInfo(h, &bufferInfo);

    std::cout << "IGT: " << *okami::IngameTimeFramesPtr << std::endl;
    std::cout << "Pos X: " << *okami::AmmyPosXPtr << std::endl;
    std::cout << "Pos Y: " << *okami::AmmyPosYPtr << std::endl;
    std::cout << "Pos Z: " << *okami::AmmyPosZPtr << std::endl;
    std::cout << "Health: " << *okami::AmmyCurrentHealthPtr << std::endl;
    std::cout << "Ink: " << *okami::AmmyCurrentInkPtr << std::endl;
    std::cout << "Food: " << *okami::AmmyCurrentFoodPtr << std::endl;
    std::cout << "Money: " << *okami::AmmyCurrentMoneyPtr << std::endl;
    std::cout << "Exterior MapID: " << *okami::ExeriorMapIDPtr << std::endl;
    std::cout << "Current MapID: " << *okami::CurrentMapIDPtr << std::endl;
    std::cout << "VMID1: " << *okami::VestigialMapID1Ptr << std::endl;
    std::cout << "VMID2: " << *okami::VsstigialMapID2Ptr << std::endl;

    SetConsoleCursorPosition(h, bufferInfo.dwCursorPosition);
}

void initVariables()
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
    // uint32_t AmmyTotalMoney;
    // uint8_t MoneyUpgradeCount;
    // int16_t AmmyCurrentPraise;
    // int16_t PraiseUpgradeBar;
    // int16_t HealthUpgradeBar;
    // int16_t FoodUpgradeBar;
    // int16_t MoneyUpgradeBar;
    // int16_t InkUpgradeBar;
    // int16_t AmmyTotalPraise;
    // int16_t AmmyCurrentDemonFangs;
    // uint16_t AmmyTotalDemonFangs;
    // uint16_t InventoryItemID;
    // uint16_t InventoryStringID;
    // uint8_t InventoryFirstItemOffset;
    // uint8_t InventoryColumn;
    // uint8_t InventorySelectedItemRow;
    // int16_t AmmyCurrentSunFragments;
    // int16_t InventoryItemQualities[256];
    // uint8_t AmmyUsableBrushTechniques[4];
    // uint8_t AmmyObtainedBrushTechniques[4];
    // uint8_t AmmyPowerSlashUpgradeCount;
    // int32_t AmmyMaxCherryBombCount;
    okami::ExeriorMapIDPtr = (uint16_t *)(okami::MainBase + 0xB6B240);
    okami::CurrentMapIDPtr = (uint16_t *)(okami::MainBase + 0xB65E74);
    okami::VestigialMapID1Ptr = (uint16_t *)(okami::MainBase + 0xB4F0B4);
    okami::VsstigialMapID2Ptr = (uint16_t *)(okami::MainBase + 0xB6B246);
    // uint8_t AmmyUsableDojoTechniques[4];
    // uint8_t CollectionStrayBeads[16];
    // uint8_t CollectionTravelGuides[4];
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

void initFunctions()
{
    okami::MainFlowerStartupFunctionPtr = (LPVOID)(uintptr_t)(okami::MainBase + 0x4B6240);
    okami::MainFlowerStopFunctionPtr = (LPVOID)(uintptr_t)(okami::MainBase + 0x4B6230);
    okami::MainFlowerTickFunctionPtr = (LPVOID)(uintptr_t)(okami::MainBase + 0x4B63B0);
}