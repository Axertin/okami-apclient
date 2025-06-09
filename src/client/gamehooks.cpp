#include "gamehooks.h"

#include "okami/okami.hpp"

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
    std::cout << "Exterior MapID: " << okami::ExeriorMapID.get() << "                                                                          " << std::endl;
    std::cout << "Current MapID: " << okami::CurrentMapID.get() << "                                                                          " << std::endl;
    std::cout << "VMID1: " << okami::VestigialMapID1.get() << "                                                                          " << std::endl;
    std::cout << "VMID2: " << okami::VestigialMapID2.get() << "                                                                          " << std::endl;
    std::cout << "Map Name: " << okami::decodeMapName(okami::CurrentMapID.get()) << "                                                                          " << std::endl;
    std::cout << "Exterior Map Name: " << okami::decodeMapName(okami::ExeriorMapID.get()) << "                                                                          " << std::endl;
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
    printMonitors();
    Main_FlowerTickOrigin();
}

using SetBitFieldEntry = void(__fastcall *)(void *rcx, int edx, void *r8, void *r9);
static SetBitFieldEntry TreasurePickupOrigin = nullptr;
/**
 * @brief Hook for the function that sets the collection bit for treasures
 * @param rcx Pointer to byte to set
 * @param edx ID of picked up item (see Item Table)
 * @param r8 Unused but needed by ABI calling convention
 * @param r9 Unused but needed by ABI calling convention
 * @return Void
 *
 */
void __fastcall GameHooks::onTreasurePickup(void *rcx, int edx, void *r8, void *r9)
{
    auto ID = static_cast<uint16_t>(edx);
    std::cout << "[apclient] Treasure Picked Up: 0x" << std::hex << ID << std::dec << std::endl;

    TreasurePickupOrigin(rcx, edx, r8, r9);
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
    MH_CreateHook(okami::MainFlowerItemPickedUpPtr, reinterpret_cast<LPVOID>(&onTreasurePickup), reinterpret_cast<LPVOID *>(&TreasurePickupOrigin));

    MH_EnableHook(MH_ALL_HOOKS);

    std::cout << "[apclient] Game hooks initialized." << std::endl;
}

void GameHooks::cleanup()
{
}
