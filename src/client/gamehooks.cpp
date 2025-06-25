#include "gamehooks.h"

#include "archipelagosocket.h"
#include "checks.h"
#include "devdatafinder.h"
#include "logger.h"
#include "okami/memorymap.hpp"

#include <MinHook.h>

typedef void(__cdecl *FunctionType)();

static FunctionType Flower_ReturnToMenu = nullptr;
/// @brief Hook for the function called when "return to menu" is selected from
/// an active save
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
    logInfo("[gamehooks] Cleaning Up...");
    GameHooks::cleanup();
    Main_FlowerStopOrigin();
}

static FunctionType Main_FlowerTickOrigin = nullptr;
/// @brief Hook for the game's mainloop. Runs every frame.
/// @return void
void __cdecl GameHooks::onGameTick()
{
    devDataFinder_OnGameTick();
    // checkBrushes();
    ArchipelagoSocket::instance().poll();
    Main_FlowerTickOrigin();
}

typedef void(__fastcall *ItemPickupFn)(void *rcx, int edx, int r8);
static ItemPickupFn oItemPickup = nullptr;
void __fastcall GameHooks::onItemPickup(void *MaybeInventoryStruct, int ItemID, int NumItems)
{
    logDebug("[gamehooks] ItemPickup called with 0x%p, 0x%X, 0x%X", ItemID, NumItems);
    if (checkItems(ItemID))
    {
        return;
    }

    return oItemPickup(MaybeInventoryStruct, ItemID, NumItems);
}

typedef bool(__fastcall *EditBrushesFn)(void *MaybeInventoryStruct, int BitIndex, int Operation);
static EditBrushesFn oEditBrushes = nullptr;
bool __fastcall GameHooks::onBrushEdit(void *MaybeInventoryStruct, int BitIndex, int Operation)
{
    logDebug("[gamehooks] EditBrushes called with 0x%p, 0x%X, 0x%X", MaybeInventoryStruct, BitIndex, Operation);
    checkBrushes(BitIndex);

    return true;
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
    logInfo("[gamehooks] Initializing MinHook...");
    if (MH_Initialize() != MH_OK)
    {
        logError("[gamehooks] Failed to initialize MinHook!");
        return;
    }

    MH_CreateHook(okami::MainFlowerStartupFnPtr, reinterpret_cast<LPVOID>(&onReturnToMenu), reinterpret_cast<LPVOID *>(&Flower_ReturnToMenu));
    MH_CreateHook(okami::MainFlowerStopFnPtr, reinterpret_cast<LPVOID>(&onGameStop), reinterpret_cast<LPVOID *>(&Main_FlowerStopOrigin));
    MH_CreateHook(okami::MainFlowerTickFnPtr, reinterpret_cast<LPVOID>(&onGameTick), reinterpret_cast<LPVOID *>(&Main_FlowerTickOrigin));
    MH_CreateHook(okami::MainFlowerItemPickupFnPtr, reinterpret_cast<LPVOID>(&onItemPickup), reinterpret_cast<LPVOID *>(&oItemPickup));
    MH_CreateHook(okami::EditBrushesFnPtr, reinterpret_cast<LPVOID>(&onBrushEdit), reinterpret_cast<LPVOID *>(&oEditBrushes));

    MH_EnableHook(MH_ALL_HOOKS);

    logInfo("[gamehooks] Game hooks initialized.");
}

void GameHooks::cleanup()
{
}
