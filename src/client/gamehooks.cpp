#include "gamehooks.h"

#include "archipelagosocket.h"
#include "checks.h"
#include "devdatafinder.h"
#include "logger.h"
#include "okami/memorymap.hpp"
#include "okami/shopdata.h"
#include "shop.h"

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
    okami::devDataFinder_OnGameTick();
    // checkBrushes();
    auto &apsocket = ArchipelagoSocket::instance();
    apsocket.poll();
    apsocket.processMainThreadTasks();
    Main_FlowerTickOrigin();
}

typedef void(__fastcall *ItemPickupFn)(void *rcx, int edx, int r8);
static ItemPickupFn oItemPickup = nullptr;
void __fastcall GameHooks::onItemPickup(void *MaybeInventoryStruct, int ItemID, int NumItems)
{
    logDebug("[gamehooks] Picked up %d %s", NumItems, okami::ItemTypes::GetName(ItemID));
    if (NumItems > 0)
    {
        if (checkItems(ItemID))
        {
            return;
        }
    }

    return oItemPickup(MaybeInventoryStruct, ItemID, NumItems);
}

typedef bool(__fastcall *EditBrushesFn)(void *MaybeInventoryStruct, int BitIndex, int Operation);
static EditBrushesFn oEditBrushes = nullptr;
bool __fastcall GameHooks::onBrushEdit(void *MaybeInventoryStruct, int BitIndex, int Operation)
{
    logDebug("[gamehooks] EditBrushes called with 0x%p, 0x%X, 0x%X", MaybeInventoryStruct, BitIndex, Operation);
    checkBrushes(BitIndex);

    // return oEditBrushes(MaybeInventoryStruct, BitIndex, Operation);

    return true;
}

static int64_t(__fastcall *oGetShopVariation)(void *, uint32_t shopNum, char **pszShopTextureName);
static const void *(__fastcall *oGetShopMetadata)(void *, uint32_t shopNum, uint32_t *pNumEvents, char **pszShopTextureName);
static const void *(__fastcall *oLoadRsc)(void *pRscPackage, const char *pszType, uint32_t nIdx);
static okami::ItemShopStock *(__fastcall *ocKibaShop__GetShopStockList)(void *, uint32_t *numItems);

static int64_t __fastcall onGetShopVariation(void *pUnk, uint32_t shopNum, char **pszShopTextureName)
{
    // Call the original metadata function but drop most of its details.
    // This prevents the use of variations (and therefore requiring defining multiple duplicate shops with just 1 item difference)
    uint32_t n;
    oGetShopMetadata(pUnk, shopNum, &n, pszShopTextureName);

    return 0;
}

static const void *__fastcall onLoadRsc(void *pRscPackage, const char *pszType, uint32_t nIdx)
{
    if (strcmp(pszType, "ISL") == 0)
    {
        const void *pResult = GetCurrentItemShopData(nIdx);
        if (pResult == nullptr)
        {
            pResult = oLoadRsc(pRscPackage, pszType, nIdx);
        }
        return pResult;
    }
    else if (strcmp(pszType, "SSL") == 0)
    {
        // Skill shop
        // TODO
        return oLoadRsc(pRscPackage, pszType, nIdx);
    }
    else
    {
        return oLoadRsc(pRscPackage, pszType, nIdx);
    }
}

static okami::ItemShopStock *__fastcall onCKibaShop__GetShopStockList(void *pKibaShop, uint32_t *numItems)
{
    okami::ItemShopStock *pResult = GetCurrentDemonFangShopData(numItems);
    if (pResult == nullptr)
    {
        pResult = ocKibaShop__GetShopStockList(pKibaShop, numItems);
    }
    return pResult;
}

void GameHooks::giveItem(int ItemID, int NumItems)
{
    oItemPickup(okami::MaybeInventoryStructPtr, ItemID, NumItems);
}

template <class T> void CreateHook(uintptr_t base, uintptr_t offset, T *pDetour, T **ppOriginal)
{
    MH_CreateHook(reinterpret_cast<void *>(base + offset), reinterpret_cast<LPVOID>(pDetour), reinterpret_cast<LPVOID *>(ppOriginal));
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

    CreateHook(okami::MainBase, 0x4420C0, &onGetShopVariation, &oGetShopVariation);
    CreateHook(okami::MainBase, 0x1B1770, &onLoadRsc, &oLoadRsc);
    CreateHook(okami::MainBase, 0x43F5A0, &onCKibaShop__GetShopStockList, &ocKibaShop__GetShopStockList);

    oGetShopMetadata = reinterpret_cast<decltype(oGetShopMetadata)>(okami::MainBase + 0x441E40);

    MH_EnableHook(MH_ALL_HOOKS);

    logInfo("[gamehooks] Game hooks initialized.");

    // TODO: TEMPORARY for testing, move to archipelago connect when it works
    InitializeShopData();
}

void GameHooks::cleanup()
{
}
