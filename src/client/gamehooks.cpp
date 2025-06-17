#include "gamehooks.h"

#include "okami/okami.hpp"
#include "checks.h"

typedef void(__cdecl *FunctionType)();

void printMonitors()
{
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
    GetConsoleScreenBufferInfo(h, &bufferInfo);

    std::cout << "IGT: " << okami::IngameTimeFrames.get() << "                                                                          " << std::endl;
    std::cout << "Pos X: " << okami::AmmyPosX.get() << "                                                                          " << std::endl;
    std::cout << "Pos Y: " << okami::AmmyPosY.get() << "                                                                          " << std::endl;
    std::cout << "Pos Z: " << okami::AmmyPosZ.get() << "                                                                          " << std::endl;
    std::cout << "Health: " << okami::AmmyCurrentHealth.get() << "                                                                          " << std::endl;
    std::cout << "Ink: " << okami::AmmyCurrentInk.get() << "                                                                          " << std::endl;
    std::cout << "Food: " << okami::AmmyCurrentFood.get() << "                                                                          " << std::endl;
    std::cout << "Money: " << okami::AmmyCurrentMoney.get() << "                                                                          " << std::endl;
    std::cout << "Exterior MapID: " << okami::ExteriorMapID.get() << "                                                                          " << std::endl;
    std::cout << "Current MapID: " << okami::CurrentMapID.get() << "                                                                          " << std::endl;
    std::cout << "VMID1: " << okami::VestigialMapID1.get() << "                                                                          " << std::endl;
    std::cout << "VMID2: " << okami::VestigialMapID2.get() << "                                                                          " << std::endl;
    std::cout << "Map Name: " << okami::decodeMapName(okami::CurrentMapID.get()) << "                                                                          " << std::endl;
    std::cout << "Exterior Map Name: " << okami::decodeMapName(okami::ExteriorMapID.get()) << "                                                                          " << std::endl;
    std::cout << "Inventory ItemID: 0x" << std::hex << okami::InventoryItemID.get() << "                                                                          " << std::dec << std::endl;
    std::cout << "Inventory StringID: 0x" << std::hex << okami::InventoryItemID.get() << "                                                                          " << std::dec << std::endl;

    SetConsoleCursorPosition(h, bufferInfo.dwCursorPosition);
}

static FunctionType Flower_ReturnToMenu = nullptr;
/// @brief Hook for the function called when "return to menu" is selected from an active save
/// @return void
void __cdecl GameHooks::onReturnToMenu()
{
    Flower_ReturnToMenu();
}

static FunctionType Main_FlowerStopOrigin = nullptr;
// TODO: Verify that this is what I think it is
/// @brief Hook for the function called when the game stops
/// @return void
void __cdecl GameHooks::onGameStop()
{
    std::cout << "Cleaning Up...";
    GameHooks::cleanup();
    std::cout << "Done!" << std::endl;
    Main_FlowerStopOrigin();
}

static FunctionType Main_FlowerTickOrigin = nullptr;
/// @brief Hook for the game's mainloop. Runs every frame.
/// @return void
void __cdecl GameHooks::onGameTick()
{
    checkBrushes();
    Main_FlowerTickOrigin();
}

typedef void(__fastcall *ItemPickupFn)(void *rcx, int edx, int r8);
static ItemPickupFn oItemPickup = nullptr;
void __fastcall GameHooks::onItemPickup(void *MaybeInventoryStruct, int ItemID, int NumItems)
{
    if (checkItems(ItemID))
    {
        return;
    }

    return oItemPickup(MaybeInventoryStruct, ItemID, NumItems);
}

void GameHooks::giveItem(int ItemID, int NumItems)
{
    oItemPickup(okami::MaybeInventoryStructPtr, ItemID, NumItems);
}

/**
 * @brief Setup game-related hooks
 *
 */
void GameHooks::setup()
{
    std::cout << "[apclient] Initializing MinHook...";
    if (MH_Initialize() != MH_OK)
    {
        std::cout << "Failed!" << std::endl;
        return;
    }
    std::cout << "Done!" << std::endl;

    MH_CreateHook(okami::MainFlowerStartupFnPtr, reinterpret_cast<LPVOID>(&onReturnToMenu), reinterpret_cast<LPVOID *>(&Flower_ReturnToMenu));
    MH_CreateHook(okami::MainFlowerStopFnPtr, reinterpret_cast<LPVOID>(&onGameStop), reinterpret_cast<LPVOID *>(&Main_FlowerStopOrigin));
    MH_CreateHook(okami::MainFlowerTickFnPtr, reinterpret_cast<LPVOID>(&onGameTick), reinterpret_cast<LPVOID *>(&Main_FlowerTickOrigin));
    MH_CreateHook(okami::MainFlowerItemPickupFnPtr, reinterpret_cast<LPVOID>(&onItemPickup), reinterpret_cast<LPVOID *>(&oItemPickup));

    MH_EnableHook(MH_ALL_HOOKS);

    std::cout << "[apclient] Game hooks initialized." << std::endl;
}

void GameHooks::cleanup()
{
}
