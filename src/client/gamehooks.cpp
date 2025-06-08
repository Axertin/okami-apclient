#include "gamehooks.h"

typedef void(__cdecl *FunctionType)();

void printMonitors()
{
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
    GetConsoleScreenBufferInfo(h, &bufferInfo);

    std::cout << "IGT: " << *okami::IngameTimeFramesPtr << "                                                                          " << std::endl;
    std::cout << "Pos X: " << *okami::AmmyPosXPtr << "                                                                          " << std::endl;
    std::cout << "Pos Y: " << *okami::AmmyPosYPtr << "                                                                          " << std::endl;
    std::cout << "Pos Z: " << *okami::AmmyPosZPtr << "                                                                          " << std::endl;
    std::cout << "Health: " << *okami::AmmyCurrentHealthPtr << "                                                                          " << std::endl;
    std::cout << "Ink: " << *okami::AmmyCurrentInkPtr << "                                                                          " << std::endl;
    std::cout << "Food: " << *okami::AmmyCurrentFoodPtr << "                                                                          " << std::endl;
    std::cout << "Money: " << *okami::AmmyCurrentMoneyPtr << "                                                                          " << std::endl;
    std::cout << "Exterior MapID: " << *okami::ExeriorMapIDPtr << "                                                                          " << std::endl;
    std::cout << "Current MapID: " << *okami::CurrentMapIDPtr << "                                                                          " << std::endl;
    std::cout << "VMID1: " << *okami::VestigialMapID1Ptr << "                                                                          " << std::endl;
    std::cout << "VMID2: " << *okami::VsstigialMapID2Ptr << "                                                                          " << std::endl;
    std::cout << "Map Name: " << okami::decodeMapName(*okami::CurrentMapIDPtr) << "                                                                          " << std::endl;
    std::cout << "Exterior Map Name: " << okami::decodeMapName(*okami::ExeriorMapIDPtr) << "                                                                          " << std::endl;
    std::cout << "Inventory ItemID: 0x" << std::hex << *okami::InventoryItemIDPtr << "                                                                          " << std::dec << std::endl;
    std::cout << "Inventory StringID: 0x" << std::hex << *okami::InventoryItemIDPtr << "                                                                          " << std::dec << std::endl;

    SetConsoleCursorPosition(h, bufferInfo.dwCursorPosition);
}

FunctionType Flower_ReturnToMenu = nullptr;
/// @brief Hook for the function called when "return to menu" is selected from an active save
/// @return void
void __cdecl GameHooks::onReturnToMenu()
{
    Flower_ReturnToMenu();
}

FunctionType Main_FlowerStopOrigin = nullptr;
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

FunctionType Main_FlowerTickOrigin = nullptr;
/// @brief Hook for the game's mainloop. Runs every frame.
/// @return void
void __cdecl GameHooks::onGameTick()
{
    printMonitors();
    Main_FlowerTickOrigin();
}

using SetBitFieldEntry = void(__fastcall *)(void *rcx, int edx, void *r8, void *r9);
SetBitFieldEntry TreasurePickupOrigin = nullptr;
/// @brief Hook for the function that sets the collection bit for treasures
/// @param rcx Pointer to byte to set
/// @param edx ID of picked up item (see Item Table)
/// @param r8 Unused but needed by ABI calling convention
/// @param r9 Unused but needed by ABI calling convention
/// @return Void
void __fastcall GameHooks::onTreasurePickup(void *rcx, int edx, void *r8, void *r9)
{
    auto ID = (uint16_t)edx;
    std::cout << "[apclient] Treasure Picked Up: 0x" << std::hex << ID << std::dec << std::endl;

    TreasurePickupOrigin(rcx, edx, r8, r9);
}

using SetBitFieldEntry = void(__fastcall *)(void *rcx, int edx, void *r8, void *r9);
SetBitFieldEntry TreasurePickupOrigin = nullptr;
/// @brief Hook for the function that sets the collection bit for treasures
/// @param rcx Pointer to byte to set
/// @param edx ID of picked up item (see Item Table)
/// @param r8 Unused but needed by ABI calling convention
/// @param r9 Unused but needed by ABI calling convention
/// @return Void
void __fastcall GameHooks::onTreasurePickup(void *rcx, int edx, void *r8, void *r9)
{
    auto ID = (uint16_t)edx;
    std::cout << "[apclient] Treasure Picked Up: 0x" << std::hex << ID << std::dec << std::endl;

    TreasurePickupOrigin(rcx, edx, r8, r9);
}

void GameHooks::setup()
{
    std::cout << "[apclient] Initializing MinHook...";
    if (MH_Initialize() != MH_OK)
    {
        std::cout << "Failed!" << std::endl;
        return;
    }

    MH_CreateHook(okami::MainFlowerStartupFunctionPtr, reinterpret_cast<LPVOID>(&onReturnToMenu), (LPVOID *)(&Flower_ReturnToMenu));
    MH_CreateHook(okami::MainFlowerStopFunctionPtr, reinterpret_cast<LPVOID>(&onGameStop), (LPVOID *)(&Main_FlowerStopOrigin));
    MH_CreateHook(okami::MainFlowerTickFunctionPtr, reinterpret_cast<LPVOID>(&onGameTick), (LPVOID *)(&Main_FlowerTickOrigin));
    MH_CreateHook(okami::MainFlowerItemPickedUpPtr, reinterpret_cast<LPVOID>(&onTreasurePickup), (LPVOID *)(&TreasurePickupOrigin));

    MH_EnableHook(MH_ALL_HOOKS);
    std::cout << "Done!" << std::endl;
}

void GameHooks::cleanup()
{
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
}
