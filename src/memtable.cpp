
#include "memtable.h"

const auto BaseAddress = (uintptr_t)(GetModuleHandleW(L"main.dll"));

memtable MemoryTable;

uint32_t readIngameTime(void) { return 0; }

float readAmmyPosX(void)
{
    float *ptr = (float *)(BaseAddress + 0xB6B2D0 + 0x000000A8 + 0);
    return *ptr;
}

float readAmmyPosY(void)
{
    float *ptr = (float *)(BaseAddress + 0xB6B2D0 + 0x000000A8 + 4);
    return *ptr;
}

float readAmmyPosZ(void)
{
    float *ptr = (float *)(BaseAddress + 0xB6B2D0 + 0x000000A8 + 8);
    return *ptr;
}

int32_t readAmmyYen(void)
{
    return *(int32_t *)(BaseAddress + 0xB205E0);
}

uintptr_t getBaseAddress(void)
{
    return BaseAddress;
}

memtable getMemTable(void)
{
    memtable mem;

    mem.IngameTimeFrames = *(uint32_t *)(BaseAddress + 0xB217FC);
    mem.AmmyPosX = *(float *)(BaseAddress + 0xB6B2D0 + 0x0000A8 + 0);
    mem.AmmyPosY = *(float *)(BaseAddress + 0xB6B2D0 + 0x0000A8 + 4);
    mem.AmmyPosZ = *(float *)(BaseAddress + 0xB6B2D0 + 0x0000A8 + 8);
    mem.AmmyCurrentHealth = *(int16_t *)(BaseAddress + 0xB4DF90);
    mem.AmmyMaxHealth = *(int16_t *)(BaseAddress + 0xB4DF92);
    mem.HealthUpgradeCount = *(uint8_t *)(BaseAddress + 0xB205E5);
    mem.AmmyCurrentInk = *(int32_t *)(BaseAddress + 0xB205D8);
    mem.AmmyMaxInk = *(int32_t *)(BaseAddress + 0xB205DC);
    mem.BrushTargetInk = *(int32_t *)(BaseAddress + 0x8928A4);
    mem.BrushCurrentInk = *(int32_t *)(BaseAddress + 0x8928A8);
    mem.InkUpgradeCount = *(uint8_t *)(BaseAddress + 0xB1F208);
    mem.CanvasBrushStrokes = *(uint8_t *)(BaseAddress + 0x888C58);
    mem.AmmyCurrentFood = *(int16_t *)(BaseAddress + 0xB4DF94);
    mem.AmmyMaxFood = *(int16_t *)(BaseAddress + 0xB4DF94);
    mem.FoodUpgradeCount = *(uint8_t *)(BaseAddress + 0xB4DF96);
    mem.AmmyGodhood = *(int16_t *)(BaseAddress + 0xB4DFAC);
    mem.AmmyCurrentMoney = *(uint32_t *)(BaseAddress + 0xB205E0);
    mem.AmmyDisplayedMoney = *(uint32_t *)(BaseAddress + 0xB1CFE4);
    mem.AmmyMaxMoney[4] = ((uint32_t *)(BaseAddress + 0x6B22A8))[4];
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
    mem.ExeriorMapID = *(uint16_t *)(BaseAddress + 0xB6B240);
    mem.CurrentMapID = *(uint16_t *)(BaseAddress + 0xB65E74);
    mem.VestigialMapID1 = *(uint16_t *)(BaseAddress + 0xB4F0B4);
    mem.VsstigialMapID2 = *(uint16_t *)(BaseAddress + 0xB6B246);
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

    return mem;
}

void printMemTable(memtable mem)
{
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
    GetConsoleScreenBufferInfo(h, &bufferInfo);

    std::cout << "IGT: " << mem.IngameTimeFrames << std::endl;
    std::cout << "Pos X: " << mem.AmmyPosX << std::endl;
    std::cout << "Pos Y: " << mem.AmmyPosY << std::endl;
    std::cout << "Pos Z: " << mem.AmmyPosZ << std::endl;
    std::cout << "Health: " << mem.AmmyCurrentHealth << std::endl;
    std::cout << "Ink: " << mem.AmmyCurrentInk << std::endl;
    std::cout << "Food: " << mem.AmmyCurrentFood << std::endl;
    std::cout << "Money: " << mem.AmmyCurrentMoney << std::endl;
    std::cout << "Exterior MapID: " << mem.ExeriorMapID << std::endl;
    std::cout << "Current MapID: " << mem.CurrentMapID << std::endl;
    std::cout << "VMID1: " << mem.VestigialMapID1 << std::endl;
    std::cout << "VMID2: " << mem.VsstigialMapID2 << std::endl;

    SetConsoleCursorPosition(h, bufferInfo.dwCursorPosition);
}
